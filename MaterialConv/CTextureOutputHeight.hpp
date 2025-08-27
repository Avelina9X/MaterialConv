#pragma once

#include "CTextureOutputBase.hpp"
#include "CTextureInputHeight.hpp"

class CTextureOutputHeight : public CTextureOutputBase
{
public:
	CTextureOutputHeight( CTextureDefinitionFile &definitionFile, CTextureInputHeight &heightTexture, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr ) :
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

	std::map<std::string, std::string> GetMaterialKeyValues() {
		return {
			{ "FileName", m_sFilename },
			{ "HeightScale", m_sHeightScale },
			{ "SRGB", IsSRGB() ? "True" : "False" },
			{ "Premultiplied", GetMetadata().IsPMAlpha() ? "True" : "False" },
		};
	}

protected:
	std::string m_sHeightScale;
};