#pragma once

#include "CTextureOutputBase.hpp"
#include "CTextureInputRMA.hpp"

class CTextureOutputRMA : public CTextureOutputBase
{
public:
	CTextureOutputRMA(
		CTextureDefinitionFile &definitionFile,
		CTextureInputRoughness &roughnessTexture,
		CTextureInputMetalness &metalnessTexture,
		CTextureInputAmbient &ambientTexture,
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice = nullptr
	);

	std::map<std::string, std::string> GetMaterialKeyValues() {
		return {
			{ "FileName", m_sFilename },
			{ "SRGB", IsSRGB() ? "True" : "False" },
			{ "Premultiplied", GetMetadata().IsPMAlpha() ? "True" : "False" },
		};
	}

protected:
	void VerifySizes( DirectX::TexMetadata roughnessMetadata, DirectX::TexMetadata metalnessMetadata, DirectX::TexMetadata ambientMetadata );
	
	DirectX::FORMAT_TYPE VerifyDataTypes( DXGI_FORMAT roughnessFormat, DXGI_FORMAT metalnessFormat, DXGI_FORMAT ambientFormat );
	
	DXGI_FORMAT GetWidestFormat( DXGI_FORMAT roughnessFormat, DXGI_FORMAT metalnessFormat, DXGI_FORMAT ambientFormat );

	template<typename T>
	void PackTextures( const DirectX::Image *roughnessImg, const DirectX::Image *metalnessImg, const DirectX::Image *ambientImg, const DirectX::Image *outImg ) {
		std::cout << PrettySection() << ": Packing...";

		for ( size_t y = 0; y < outImg->height; ++y ) {
			auto rRow = reinterpret_cast<const T *>( roughnessImg->pixels + y * roughnessImg->rowPitch );
			auto mRow = reinterpret_cast<const T *>( metalnessImg->pixels + y * metalnessImg->rowPitch );
			auto aRow = reinterpret_cast<const T *>( ambientImg->pixels + y * ambientImg->rowPitch );
			auto outRow = reinterpret_cast<T *>( outImg->pixels + y * outImg->rowPitch );

			for ( size_t x = 0; x < outImg->width; ++x ) {
				outRow[x * 4 + 0] = rRow[x];
				outRow[x * 4 + 1] = mRow[x];
				outRow[x * 4 + 2] = aRow[x];
				outRow[x * 4 + 3] = (T) -1; // TODO: fix for float/snorm/sint types 
			}
		}

		std::cout << "done!" << std::endl;
	}
};