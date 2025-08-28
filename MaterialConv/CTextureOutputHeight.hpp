#pragma once

#include "CTextureOutputBase.hpp"
#include "CTextureInputHeight.hpp"

class CTextureOutputHeight : public CTextureOutputBase
{
public:
	CTextureOutputHeight( CTextureDefinitionFile &definitionFile, CTextureInputHeight &heightTexture, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr );

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