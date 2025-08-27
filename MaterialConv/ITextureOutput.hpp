#pragma once

#define NOMINMAX
#include <DirectXTex.h>
#include <memory>
#include <filesystem>
#include <map>
#include <wrl\client.h>

#include "TexUtils.hpp"

class ITextureOutput
{
public:
	virtual DXGI_FORMAT GetOutputFormat() = 0;
	virtual std::filesystem::path GetOutputPath() = 0;
	virtual std::map<std::string, std::string> GetMaterialKeyValues() = 0;

	virtual const DirectX::TexMetadata &GetMetadata() = 0;
	virtual const DirectX::Image *GetImages() = 0;
	virtual const size_t GetImageCount() = 0;

	virtual const bool IsSRGB() = 0;

protected:
	virtual void PostProcess( const DirectX::Image *srcImages, size_t nimages, const DirectX::TexMetadata &metadata, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr ) = 0;
};