#include "CTextureOutputHeight.hpp"

using Microsoft::WRL::ComPtr;

CTextureOutputHeight::CTextureOutputHeight( CTextureDefinitionFile &definitionFile, CTextureInputHeight &heightTexture, ComPtr<ID3D11Device> pDevice ) :
	CTextureOutputBase( definitionFile, "OutHeight", "height.dds" )
{
	if ( heightTexture.IsSRGB() != IsSRGB() ) {
		std::cerr << "Height input SRGB mismatches output format!" << std::endl;
		throw std::exception( "DXGI_FORMAT error" );
	}

	if ( InputChannels( heightTexture.GetMetadata().format ) != InputChannels( GetOutputFormat() ) ) {
		std::cerr << "Height input channels mismatches output format channels!" << std::endl;
		throw std::exception( "DXGI_FORMAT error" );
	}

	PostProcess( heightTexture.GetImages(), heightTexture.GetImageCount(), heightTexture.GetMetadata(), pDevice );

	m_sHeightScale = std::to_string( heightTexture.GetHeightScale() );
}