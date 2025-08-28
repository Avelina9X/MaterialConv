#pragma once

#include "CTextureOutputBase.hpp"
#include "CTextureInputNormal.hpp"

class CTextureOutputNormal : public CTextureOutputBase
{
public:
	CTextureOutputNormal( CTextureDefinitionFile &definitionFile, CTextureInputNormal &normalTexture, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr );

	std::map<std::string, std::string> GetMaterialKeyValues() {
		return {
			{ "FileName", m_sFilename },
			{ "SRGB", IsSRGB() ? "True" : "False" },
			{ "Premultiplied", GetMetadata().IsPMAlpha() ? "True" : "False" },
		};
	}
};