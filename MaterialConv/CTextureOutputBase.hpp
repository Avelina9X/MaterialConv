#pragma once

#include "ITextureOutput.hpp"
#include "CTextureDefinitionFile.hpp"
#include <wrl\client.h>

class CTextureOutputBase : public ITextureOutput
{
public:
	CTextureOutputBase( CTextureDefinitionFile &definitionFile, std::string section, std::string filename );

	const std::string PrettySection() { return "[\033[1m" + m_sSection + "\033[0m]"; }

	DXGI_FORMAT GetOutputFormat() { return m_fOutputFormat; }
	std::filesystem::path GetOutputPath() { return m_filePath; }

	const DirectX::TexMetadata &GetMetadata() override { return m_compressedTexture->GetMetadata(); }
	const DirectX::Image *GetImages() override { return m_compressedTexture->GetImages(); }
	const size_t GetImageCount() override { return m_compressedTexture->GetImageCount(); }

	const bool IsSRGB() override { return m_bIsSRGB; }

protected:
	const std::string m_sSection;
	const std::string m_sFilename;
	std::filesystem::path m_filePath;
	const std::string m_sOutputFormat;
	const DXGI_FORMAT m_fOutputFormat;
	const bool m_bIsSRGB;

	std::unique_ptr<DirectX::ScratchImage> m_mipmappedTexture;
	std::unique_ptr<DirectX::ScratchImage> m_compressedTexture;

	void PostProcess( const DirectX::Image *srcImages, size_t nimages, const DirectX::TexMetadata &metadata, Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr ) override;
};