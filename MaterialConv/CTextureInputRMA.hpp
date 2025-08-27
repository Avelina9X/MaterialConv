#pragma once

#include "CTextureInputBase.hpp"
#include "TexUtils.hpp"

class CTextureInputRMA : public CTextureInputBase
{
protected:
	CTextureInputRMA( CTextureDefinitionFile &definitionFile, std::string section ) :
		CTextureInputBase( definitionFile, section ) {

		if ( InputChannels( GetMetadata().format ) != 1 ) {
			std::cerr << "RMA textures must output 1 channel! Check your swizzle!" << std::endl;
			throw std::exception( "Invalid swizzle" );
		}

		if ( IsSRGB() ) {
			std::cerr << "RMA textures cannot be SRGB!" << std::endl;
			throw std::exception( "SRGB error" );
		}
	}

public:
	void Recast( DXGI_FORMAT destination ) {
		if ( destination != GetMetadata().format ) {
			std::unique_ptr<DirectX::ScratchImage> timage = std::make_unique<DirectX::ScratchImage>();

			HRESULT hr = DirectX::Convert( GetImages(), GetImageCount(), GetMetadata(), destination, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, *timage.get() );
			if ( FAILED( hr ) ) {
				std::cerr << "Failed to convert [" << m_sSection << "] from " << GetMetadata().format << " to " << destination << std::endl;
				throw std::exception( "DXGI_Format error" );
			}

			m_swizzleTexture.swap( timage );
		}
	}
};

class CTextureInputRoughness : public CTextureInputRMA
{
public:
	CTextureInputRoughness( CTextureDefinitionFile &definitionFile ) : CTextureInputRMA( definitionFile, "InRoughness" ) {}
};

class CTextureInputMetalness : public CTextureInputRMA
{
public:
	CTextureInputMetalness( CTextureDefinitionFile &definitionFile ) : CTextureInputRMA( definitionFile, "InMetalness" ) {}
};

class CTextureInputAmbient : public CTextureInputRMA
{
public:
	CTextureInputAmbient( CTextureDefinitionFile &definitionFile ) : CTextureInputRMA( definitionFile, "InAmbientOcclusion" ) {}
};