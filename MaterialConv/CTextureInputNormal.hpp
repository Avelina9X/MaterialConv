#pragma once

#include "CTextureInputBase.hpp"
#include "TexUtils.hpp"

class CTextureInputNormal : public CTextureInputBase
{
public:
	CTextureInputNormal( CTextureDefinitionFile &definitionFile ) :
		CTextureInputBase( definitionFile, "InNormal" ) {

		if ( InputChannels( GetMetadata().format ) != 2 ) {
			std::cerr << "Normal texture must output 2 channels! Check your swizzle!" << std::endl;
			throw std::exception( "Invalid swizzle" );
		}

		if ( IsSRGB() ) {
			std::cerr << "Normal texture cannot be SRGB!" << std::endl;
			throw std::exception( "SRGB error" );
		}

		bool flipG = definitionFile.GetField<bool>( m_sSection, "FlipG", false );

		auto bitDepth = DirectX::BitsPerColor( GetMetadata().format );

		if ( flipG ) {
			std::unique_ptr<DirectX::ScratchImage> timage = std::make_unique<DirectX::ScratchImage>();
			timage->Initialize2D( GetMetadata().format, GetMetadata().width, GetMetadata().height, 1, 1 );

			switch ( DirectX::FormatDataType( GetMetadata().format ) ) {
				case DirectX::FORMAT_TYPE_UNORM:
					if ( bitDepth == 8 ) {
						FlipGreen<uint8_t>( GetImages(), timage->GetImages() );
						break;
					}

					if ( bitDepth == 16 ) {
						FlipGreen<uint16_t>( GetImages(), timage->GetImages() );
						break;
					}

					std::cerr << "Unsupported bitdepth!" << std::endl;
					throw std::exception( "DXGI_Format error" );

				default:
					std::cerr << "Unsupported format!" << std::endl;
					throw std::exception( "DXGI_Format error" );
			}

			m_swizzleTexture.swap( timage );
		}

	}

protected:
	template<typename T>
	void FlipGreen( const DirectX::Image *inImage, const DirectX::Image *outImage ) {
		using namespace DirectX;
		std::cout << PrettySection() << ": Flipping green...";

		for ( size_t y = 0; y < inImage->height; ++y ) {
			auto inRow = reinterpret_cast<const T *>( inImage->pixels + y * inImage->rowPitch );
			auto outRow = reinterpret_cast<T *>( outImage->pixels + y * outImage->rowPitch );

			for ( size_t x = 0; x < inImage->width; ++x ) {
				outRow[x * 2 + 0] = inRow[x * 2 + 0];
				outRow[x * 2 + 1] = ( (T) -1 ) - inRow[x * 2 + 1]; // TODO: fix for float/snorm/sint types
			}
		}

		std::cout << "done!" << std::endl;
	}
};