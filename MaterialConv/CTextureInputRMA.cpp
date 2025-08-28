#include "CTextureInputRMA.hpp"

using namespace DirectX;

CTextureInputRMA::CTextureInputRMA( CTextureDefinitionFile &definitionFile, std::string section ) :
	CTextureInputBase( definitionFile, section )
{

	if ( InputChannels( GetMetadata().format ) != 1 ) {
		std::cerr << "RMA textures must output 1 channel! Check your swizzle!" << std::endl;
		throw std::exception( "Invalid swizzle" );
	}

	if ( IsSRGB() ) {
		std::cerr << "RMA textures cannot be SRGB!" << std::endl;
		throw std::exception( "SRGB error" );
	}
}

void CTextureInputRMA::Recast( DXGI_FORMAT destination )
{
	if ( destination != GetMetadata().format ) {
		std::unique_ptr<ScratchImage> timage = std::make_unique<ScratchImage>();

		HRESULT hr = Convert( GetImages(), GetImageCount(), GetMetadata(), destination, TEX_FILTER_DEFAULT, TEX_THRESHOLD_DEFAULT, *timage.get() );
		if ( FAILED( hr ) ) {
			std::cerr << "Failed to convert [" << m_sSection << "] from " << GetMetadata().format << " to " << destination << std::endl;
			throw std::exception( "DXGI_Format error" );
		}

		m_swizzleTexture.swap( timage );
	}
}