#include "CTextureInputNormal.hpp"

using namespace DirectX;

CTextureInputNormal::CTextureInputNormal( CTextureDefinitionFile &definitionFile ) :
	CTextureInputBase( definitionFile, "InNormal" )
{

	if ( InputChannels( GetMetadata().format ) != 2 ) {
		std::cerr << "Normal texture must output 2 channels! Check your swizzle!" << std::endl;
		throw std::exception( "Invalid swizzle" );
	}

	if ( IsSRGB() ) {
		std::cerr << "Normal texture cannot be SRGB!" << std::endl;
		throw std::exception( "SRGB error" );
	}

	bool flipG = definitionFile.GetField<bool>( m_sSection, "FlipG", false );

	auto bitDepth = BitsPerColor( GetMetadata().format );

	if ( flipG ) {
		std::unique_ptr<ScratchImage> timage = std::make_unique<ScratchImage>();
		timage->Initialize2D( GetMetadata().format, GetMetadata().width, GetMetadata().height, 1, 1 );

		switch ( FormatDataType( GetMetadata().format ) ) {
			case FORMAT_TYPE_UNORM:
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