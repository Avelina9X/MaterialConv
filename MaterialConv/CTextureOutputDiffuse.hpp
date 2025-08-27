#pragma once

#include "CTextureOutputBase.hpp"
#include "CTextureInputDIffuse.hpp"

class CTextureOutputDiffuse : public CTextureOutputBase
{
public:
	CTextureOutputDiffuse( CTextureDefinitionFile &definitionFile, CTextureInputDiffuse &diffuseTexture, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr ) :
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

	std::map<std::string, std::string> GetMaterialKeyValues() {
		return {
			{ "FileName", m_sFilename },
			{ "SRGB", IsSRGB() ? "True" : "False" },
			{ "Premultiplied", GetMetadata().IsPMAlpha() ? "True" : "False" },
		};
	}	
};