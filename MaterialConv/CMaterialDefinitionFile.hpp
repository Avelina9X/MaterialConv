#pragma once

#define NOMINMAX

#include "CTextureDefinitionFile.hpp"
#include <wrl/client.h>
#include "DirectXTex.h"

class CMaterialDefinitionFile
{
public:
    CMaterialDefinitionFile( CTextureDefinitionFile &definitionFile, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr );

protected:
	const std::string m_sFilename;
	std::filesystem::path m_filePath;
};