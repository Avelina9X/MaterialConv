#pragma once

#include "ITextureOutput.hpp"
#include "CTextureDefinitionFile.hpp"
#include <wrl\client.h>

class CTextureOutputBase : public ITextureOutput
{
public:
	CTextureOutputBase( CTextureDefinitionFile &definitionFile, std::string section, std::string filename ) :
		m_sSection( section ),
		m_sFilename( definitionFile.GetField<std::string>( m_sSection, "FileName", filename ) ),
		m_filePath( definitionFile.GetOutDir() / m_sFilename ),
		m_sOutputFormat( definitionFile.GetField<std::string>( m_sSection, "Format" ) ),
		m_fOutputFormat( LookupByName( m_sOutputFormat.c_str(), g_pFormats ) ),
		m_bIsSRGB( DirectX::IsSRGB( m_fOutputFormat ) )
	{

	}

	const std::string PrettySection() { return "[\033[1m" + m_sSection + "\033[0m]"; }

	DXGI_FORMAT GetOutputFormat() { return m_fOutputFormat; }
	std::filesystem::path GetOutputPath() { return m_filePath; }

	const DirectX::TexMetadata &GetMetadata() override { return m_compressedTexture->GetMetadata(); }
	const DirectX::Image *GetImages() override { return m_compressedTexture->GetImages(); }
	const size_t GetImageCount() override { return m_compressedTexture->GetImageCount(); }

	const bool IsSRGB() override { return m_bIsSRGB; }

protected:
	const std::string m_sSection;
	const std::string m_sFilename;
	std::filesystem::path m_filePath;
	const std::string m_sOutputFormat;
	const DXGI_FORMAT m_fOutputFormat;
	const bool m_bIsSRGB;

	std::unique_ptr<DirectX::ScratchImage> m_mipmappedTexture;
	std::unique_ptr<DirectX::ScratchImage> m_compressedTexture;

	void PostProcess( const DirectX::Image *srcImages, size_t nimages, const DirectX::TexMetadata &metadata, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr ) override {
		using namespace DirectX;
		
		m_mipmappedTexture = std::make_unique<ScratchImage>();

		std::cout << PrettySection() << ": Generating MipMaps...";

		HRESULT hr = GenerateMipMaps(
			srcImages,
			nimages,
			metadata,
			IsSRGB() ? TEX_FILTER_SRGB : TEX_FILTER_DEFAULT,
			0,
			*m_mipmappedTexture.get()
		);

		if ( FAILED( hr ) ) {
			std::cerr << "Failed to generate mipmaps in section [" << m_sSection << "]" << std::endl;
			throw std::exception( "Mipmap error" );
		}

		std::cout << "done!" << std::endl;

		m_compressedTexture = std::make_unique<ScratchImage>();

		if ( MakeTypeless( GetOutputFormat() ) == DXGI_FORMAT_BC7_TYPELESS ) {

			if ( !pDevice ) {
				std::cerr << "No GPU detected! Cannot use BC7 lest your computer explode!" << std::endl;
				throw std::exception( "Compress error" );
			}

			std::cout << PrettySection() << ": Compressing Texture on GPU...";

			hr = Compress(
				pDevice.Get(),
				m_mipmappedTexture->GetImages(),
				m_mipmappedTexture->GetImageCount(),
				m_mipmappedTexture->GetMetadata(),
				GetOutputFormat(),
				TEX_COMPRESS_PARALLEL | ( IsSRGB() ? TEX_COMPRESS_SRGB : TEX_COMPRESS_DEFAULT ),
				1.0,
				*m_compressedTexture.get()
			);
		}
		else {

			std::cout << PrettySection() << ": Compressing Texture...";		

			hr = Compress(
				m_mipmappedTexture->GetImages(),
				m_mipmappedTexture->GetImageCount(),
				m_mipmappedTexture->GetMetadata(),
				GetOutputFormat(),
				TEX_COMPRESS_PARALLEL | ( IsSRGB() ? TEX_COMPRESS_SRGB : TEX_COMPRESS_DEFAULT ),
				TEX_THRESHOLD_DEFAULT,
				*m_compressedTexture.get()
			);
		}

		if ( FAILED( hr ) ) {
			std::cerr << "Failed to compress texture in section [" << m_sSection << "]" << std::endl;
			throw std::exception( "Compress error" );
		}

		std::cout << "done!" << std::endl;

	}
};