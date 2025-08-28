#pragma once

#include "ITextureOutput.hpp"
#include "CTextureDefinitionFile.hpp"
#include "CTextureInputPreview.hpp"

class CTextureOutputPreview : public ITextureOutput
{
public:
	CTextureOutputPreview( CTextureDefinitionFile &definitionFile, CTextureInputPreview &previewTexture );

	std::string GetMaterialSection() override { return "Preview"; }
	std::map<std::string, std::string> GetMaterialKeyValues() {
		return {
			{ "FileName", m_sFilename }
		};
	}

	const std::string PrettySection() { return "[\033[1mOutPreview\033[0m]"; }

	DXGI_FORMAT GetOutputFormat() override { return m_fOutputFormat; }
	std::filesystem::path GetOutputPath() override { return m_filePath; }
	void SaveFile() override;

	const DirectX::TexMetadata &GetMetadata() override { return m_previewTexture->GetMetadata(); }
	const DirectX::Image *GetImages() override { return m_previewTexture->GetImages(); }
	const size_t GetImageCount() override { return m_previewTexture->GetImageCount(); }

	const bool IsSRGB() override { return m_bIsSRGB; }

protected:
	const std::string m_sFilename;
	std::filesystem::path m_filePath;
	const DXGI_FORMAT m_fOutputFormat;
	const bool m_bIsSRGB;

	std::unique_ptr<DirectX::ScratchImage> m_previewTexture;
};