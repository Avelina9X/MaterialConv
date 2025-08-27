#pragma once

#include "ITextureInput.hpp"
#include "CTextureDefinitionFile.hpp"

HRESULT LoadImageFromFile( std::filesystem::path file, bool isSRGB, DirectX::ScratchImage &image );

class CTextureInputBase : public ITextureInput
{
public:
	CTextureInputBase( CTextureDefinitionFile &definitionFile, std::string section );

	const DirectX::TexMetadata &GetMetadata() override { return m_swizzleTexture->GetMetadata();  }
	const DirectX::Image *GetImages() override { return m_swizzleTexture->GetImages(); }
	const size_t GetImageCount() override { return m_swizzleTexture->GetImageCount(); }

	bool IsSRGB() { return m_bIsSRGB; }
	const SSwizzle GetSwizzle() { return m_sSwizzle; }

	const std::string PrettySection() { return "[\033[1m" + m_sSection + "\033[0m]"; }

protected:
	const std::string m_sSection;
	SSwizzle m_sSwizzle;
	bool m_bIsSRGB;

	std::unique_ptr<DirectX::ScratchImage> m_inputTexture;
	std::unique_ptr<DirectX::ScratchImage> m_swizzleTexture;

	template<typename T>
	void ComputeSwizzle( size_t inChannels, size_t outChannels, const DirectX::Image *inImage, const DirectX::Image *outImage ) {
		std::cout << PrettySection() << ": Swizzling...";

		for ( size_t y = 0; y < inImage->height; ++y ) {
			auto inRow = reinterpret_cast<const T *>( inImage->pixels + y * inImage->rowPitch );
			auto outRow = reinterpret_cast<T *>( outImage->pixels + y * outImage->rowPitch );

			for ( size_t x = 0; x < inImage->width; ++x ) {
				for ( size_t c = 0; c < outChannels; ++c ) {
					T val;
					switch ( m_sSwizzle[c] ) {
						case 'r': val = inRow[x * inChannels + 0]; break;
						case 'g': val = inRow[x * inChannels + 1]; break;
						case 'b': val = inRow[x * inChannels + 2]; break;
						case 'a': val = inRow[x * inChannels + 3]; break;
						case '0': val = 0; break;
						case '1': val = -1; break; // TODO: fix for float/snorm/sint types
						default:
							std::cerr << "Unknown swizzle value? How did we get here???" << std::endl;
							throw std::exception( "Invalid swizzle" );
					}
					outRow[x * outChannels + c] = val;
				}
			}
		}

		std::cout << "done!" << std::endl;
	}

	void ApplyPremultipliedAlpha( bool premultiplyAlpha );
};