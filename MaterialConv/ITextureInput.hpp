#pragma once

#define NOMINMAX
#include <DirectXTex.h>
#include <memory>
#include <filesystem>

#include "SSwizzle.hpp"

class ITextureInput
{
public:
	virtual bool IsSRGB() = 0;
	virtual const SSwizzle GetSwizzle() = 0;

	virtual const DirectX::TexMetadata &GetMetadata() = 0;

	virtual const DirectX::Image *GetImages() = 0;
	virtual const size_t GetImageCount() = 0;
};