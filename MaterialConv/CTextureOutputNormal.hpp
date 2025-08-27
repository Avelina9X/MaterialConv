#pragma once

#include "CTextureOutputBase.hpp"
#include "CTextureInputNormal.hpp"

class CTextureOutputNormal : public CTextureOutputBase
{
public:
	CTextureOutputNormal( CTextureDefinitionFile &definitionFile, CTextureInputNormal &normalTexture, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr ) :
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

	std::map<std::string, std::string> GetMaterialKeyValues() {
		return {
			{ "FileName", m_sFilename },
			{ "SRGB", IsSRGB() ? "True" : "False" },
			{ "Premultiplied", GetMetadata().IsPMAlpha() ? "True" : "False" },
		};
	}
};