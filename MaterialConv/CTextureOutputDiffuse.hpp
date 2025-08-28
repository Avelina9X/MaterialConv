#pragma once

#include "CTextureOutputBase.hpp"
#include "CTextureInputDiffuse.hpp"

class CTextureOutputDiffuse : public CTextureOutputBase
{
public:
	CTextureOutputDiffuse( CTextureDefinitionFile &definitionFile, CTextureInputDiffuse &diffuseTexture, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr );

	std::string GetMaterialSection() override { return "Diffuse"; }
	std::map<std::string, std::string> GetMaterialKeyValues() {
		return {
			{ "FileName", m_sFilename },
			{ "SRGB", IsSRGB() ? "True" : "False" },
			{ "Premultiplied", GetMetadata().IsPMAlpha() ? "True" : "False" },
		};
	}	
};