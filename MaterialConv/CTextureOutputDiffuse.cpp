#include "CTextureOutputDiffuse.hpp"

using Microsoft::WRL::ComPtr;

CTextureOutputDiffuse::CTextureOutputDiffuse( CTextureDefinitionFile &definitionFile, CTextureInputDiffuse &diffuseTexture, ComPtr<ID3D11Device> pDevice ) :
	CTextureOutputBase( definitionFile, "OutDiffuse", "diffuse.dds" )
{
	if ( diffuseTexture.IsSRGB() != IsSRGB() ) {
		std::cerr << "Diffuse input SRGB mismatches output format!" << std::endl;
		throw std::exception( "DXGI_FORMAT error" );
	}

	if ( InputChannels( diffuseTexture.GetMetadata().format ) != InputChannels( GetOutputFormat() ) ) {
		std::cerr << "Diffuse input channels mismatches output format channels!" << std::endl;
		throw std::exception( "DXGI_FORMAT error" );
	}

	PostProcess( diffuseTexture.GetImages(), diffuseTexture.GetImageCount(), diffuseTexture.GetMetadata(), pDevice );
}