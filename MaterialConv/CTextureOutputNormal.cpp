#include "CTextureOutputNormal.hpp"

using Microsoft::WRL::ComPtr;

CTextureOutputNormal::CTextureOutputNormal( CTextureDefinitionFile &definitionFile, CTextureInputNormal &normalTexture, ComPtr<ID3D11Device> pDevice ) :
	CTextureOutputBase( definitionFile, "OutNormal", "normal.dds" )
{
	if ( normalTexture.IsSRGB() != IsSRGB() ) {
		std::cerr << "Normal input SRGB mismatches output format!" << std::endl;
		throw std::exception( "DXGI_FORMAT error" );
	}

	if ( InputChannels( normalTexture.GetMetadata().format ) != InputChannels( GetOutputFormat() ) ) {
		std::cerr << "Normal input channels mismatches output format channels!" << std::endl;
		throw std::exception( "DXGI_FORMAT error" );
	}

	PostProcess( normalTexture.GetImages(), normalTexture.GetImageCount(), normalTexture.GetMetadata(), pDevice );
}