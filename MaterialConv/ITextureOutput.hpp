#pragma once

#define NOMINMAX
#include <DirectXTex.h>
#include <memory>
#include <filesystem>
#include <map>
#include <wrl\client.h>

#include "TexUtils.hpp"
#include "IMaterialMetadata.hpp"

class ITextureOutput : public IMaterialMetadata
{
public:
	virtual DXGI_FORMAT GetOutputFormat() = 0;
	virtual std::filesystem::path GetOutputPath() = 0;
	virtual void SaveFile() = 0;

	virtual const DirectX::TexMetadata &GetMetadata() = 0;
	virtual const DirectX::Image *GetImages() = 0;
	virtual const size_t GetImageCount() = 0;

	virtual const bool IsSRGB() = 0;
};