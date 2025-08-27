#pragma once

#include "CTextureOutputBase.hpp"
#include "CTextureInputRMA.hpp"

class CTextureOutputRMA : public CTextureOutputBase
{
public:
	CTextureOutputRMA(
		CTextureDefinitionFile &definitionFile,
		CTextureInputRoughness &roughnessTexture,
		CTextureInputMetalness &metalnessTexture,
		CTextureInputAmbient &ambientTexture,
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr
	) :
		CTextureOutputBase( definitionFile, "OutRMA", "rma.dds" )
	{
		using namespace DirectX;

		if ( roughnessTexture.IsSRGB() != IsSRGB() ) {
			std::cerr << "Roughness input SRGB mismatches output format!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( metalnessTexture.IsSRGB() != IsSRGB() ) {
			std::cerr << "Metalness input SRGB mismatches output format!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( ambientTexture.IsSRGB() != IsSRGB() ) {
			std::cerr << "Ambient input SRGB mismatches output format!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( InputChannels( GetOutputFormat() ) < 3 ) {
			std::cerr << "Output format must have at least 3 channels!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		TexMetadata roughnessMetadata = roughnessTexture.GetMetadata();
		TexMetadata metalnessMetadata = metalnessTexture.GetMetadata();
		TexMetadata ambientMetadata = ambientTexture.GetMetadata();

		VerifySizes( roughnessMetadata, metalnessMetadata, ambientMetadata );

		DXGI_FORMAT roughnessFormat = roughnessMetadata.format;
		DXGI_FORMAT metalnessFormat = metalnessMetadata.format;
		DXGI_FORMAT ambientFormat = ambientMetadata.format;

		FORMAT_TYPE formatType = VerifyDataTypes( roughnessFormat, metalnessFormat, ambientFormat );

		DXGI_FORMAT widestFormat = GetWidestFormat( roughnessFormat, metalnessFormat, ambientFormat );

		roughnessTexture.Recast( widestFormat );
		metalnessTexture.Recast( widestFormat );
		ambientTexture.Recast( widestFormat );

		DXGI_FORMAT packFormat = CreateOutputFormat( widestFormat, 4 );

		auto bitDepth = BitsPerColor( packFormat );

		std::unique_ptr<DirectX::ScratchImage> timage = std::make_unique<DirectX::ScratchImage>();
		timage->Initialize2D( packFormat, roughnessMetadata.width, roughnessMetadata.height, 1, 1 );

		switch ( formatType ) {
			case FORMAT_TYPE_UNORM:
				if ( bitDepth == 8 ) {
					PackTextures<uint8_t>( roughnessTexture.GetImages(), metalnessTexture.GetImages(), ambientTexture.GetImages(), timage->GetImages() );
					break;
				}

				if ( bitDepth == 16 ) {
					PackTextures<uint16_t>( roughnessTexture.GetImages(), metalnessTexture.GetImages(), ambientTexture.GetImages(), timage->GetImages() );
					break;
				}

				std::cerr << "Unsupported bitdepth!" << std::endl;
				throw std::exception( "DXGI_Format error" );

			default:
				std::cerr << "Unsupported format!" << std::endl;
				throw std::exception( "DXGI_Format error" );
		}

		PostProcess( timage->GetImages(), timage->GetImageCount(), timage->GetMetadata(), pDevice );
	}

	std::map<std::string, std::string> GetMaterialKeyValues() {
		return {
			{ "FileName", m_sFilename },
			{ "SRGB", IsSRGB() ? "True" : "False" },
			{ "Premultiplied", GetMetadata().IsPMAlpha() ? "True" : "False" },
		};
	}

protected:
	void VerifySizes( DirectX::TexMetadata roughnessMetadata, DirectX::TexMetadata metalnessMetadata, DirectX::TexMetadata ambientMetadata ) {
		if ( ( roughnessMetadata.width != metalnessMetadata.width ) || ( metalnessMetadata.width != ambientMetadata.width ) ) {
			std::cerr << "Ambient, Roughness and Metalness must have same width!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( ( roughnessMetadata.height != metalnessMetadata.height ) || ( metalnessMetadata.height != ambientMetadata.height ) ) {
			std::cerr << "Ambient, Roughness and Metalness must have same height!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( ( roughnessMetadata.depth != metalnessMetadata.depth ) || ( metalnessMetadata.depth != ambientMetadata.depth ) ) {
			std::cerr << "Ambient, Roughness and Metalness must have same depth!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( roughnessMetadata.depth > 1 || roughnessMetadata.arraySize > 1 ) {
			std::cerr << "We only support flat 2D texture conversion currently!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}
	}

	DirectX::FORMAT_TYPE VerifyDataTypes( DXGI_FORMAT roughnessFormat, DXGI_FORMAT metalnessFormat, DXGI_FORMAT ambientFormat ) {
		using namespace DirectX;

		auto roughnessType = FormatDataType( roughnessFormat );
		auto metalnessType = FormatDataType( metalnessFormat );
		auto ambientType = FormatDataType( ambientFormat );

		if ( ( roughnessType != metalnessType ) || ( metalnessType != ambientType ) ) {
			std::cerr << "Ambient, Roughness and Metalness must be same type!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( InputChannels( roughnessFormat ) != 1 ) {
			std::cerr << "Roughness intermediate texture must have 1 channel!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( InputChannels( metalnessFormat ) != 1 ) {
			std::cerr << "Metalness intermediate texture must have 1 channel!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		if ( InputChannels( ambientFormat ) != 1 ) {
			std::cerr << "Ambient intermediate texture must have 1 channel!" << std::endl;
			throw std::exception( "DXGI_FORMAT error" );
		}

		return roughnessType;
	}

	DXGI_FORMAT GetWidestFormat( DXGI_FORMAT roughnessFormat, DXGI_FORMAT metalnessFormat, DXGI_FORMAT ambientFormat ) {
		using namespace DirectX;

		auto bitDepth = BitsPerColor( roughnessFormat );
		auto widestFormat = roughnessFormat;

		if ( BitsPerColor( metalnessFormat ) > bitDepth ) {
			bitDepth = BitsPerColor( metalnessFormat );
			widestFormat = metalnessFormat;
		}

		if ( BitsPerColor( ambientFormat ) > bitDepth ) {
			bitDepth = BitsPerColor( ambientFormat );
			widestFormat = ambientFormat;
		}

		return widestFormat;
	}

	template<typename T>
	void PackTextures( const DirectX::Image *roughnessImg, const DirectX::Image *metalnessImg, const DirectX::Image *ambientImg, const DirectX::Image *outImg ) {
		std::cout << PrettySection() << ": Packing...";

		for ( size_t y = 0; y < outImg->height; ++y ) {
			auto rRow = reinterpret_cast<const T *>( roughnessImg->pixels + y * roughnessImg->rowPitch );
			auto mRow = reinterpret_cast<const T *>( metalnessImg->pixels + y * metalnessImg->rowPitch );
			auto aRow = reinterpret_cast<const T *>( ambientImg->pixels + y * ambientImg->rowPitch );
			auto outRow = reinterpret_cast<T *>( outImg->pixels + y * outImg->rowPitch );

			for ( size_t x = 0; x < outImg->width; ++x ) {
				outRow[x * 4 + 0] = rRow[x];
				outRow[x * 4 + 1] = mRow[x];
				outRow[x * 4 + 2] = aRow[x];
				outRow[x * 4 + 3] = (T) -1; // TODO: fix for float/snorm/sint types 
			}
		}

		std::cout << "done!" << std::endl;
	}
};