#include "CTextureOutputBase.hpp"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

CTextureOutputBase::CTextureOutputBase( CTextureDefinitionFile &definitionFile, std::string section, std::string filename ) :
	m_sSection( section ),
	m_sFilename( filename ),
	m_filePath( definitionFile.GetOutDir() / m_sFilename ),
	m_sOutputFormat( definitionFile.GetField<std::string>( m_sSection, "Format" ) ),
	m_fOutputFormat( LookupByName( m_sOutputFormat.c_str(), g_pFormats ) ),
	m_bIsSRGB( DirectX::IsSRGB( m_fOutputFormat ) )
{
}

void CTextureOutputBase::SaveFile()
{
	std::cout << PrettySection() << ": Saving file...";
	HRESULT hr = SaveToDDSFile(
		GetImages(),
		GetImageCount(),
		GetMetadata(),
		DDS_FLAGS_NONE,
		GetOutputPath().wstring().c_str()
	);

	if ( FAILED( hr ) ) {
		std::cerr << "Failed to save file to " << GetOutputPath() << "with error" << hr << std::endl;
		throw std::exception( "File save error" );
	}

	std::cout << "done!" << std::endl;
}

void CTextureOutputBase::PostProcess( const DirectX::Image *srcImages, size_t nimages, const DirectX::TexMetadata &metadata, ComPtr<ID3D11Device> pDevice )
{
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