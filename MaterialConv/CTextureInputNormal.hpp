#pragma once

#include "CTextureInputBase.hpp"
#include "TexUtils.hpp"

class CTextureInputNormal : public CTextureInputBase
{
public:
	CTextureInputNormal( CTextureDefinitionFile &definitionFile );

protected:
	template<typename T>
	void FlipGreen( const DirectX::Image *inImage, const DirectX::Image *outImage ) {
		std::cout << PrettySection() << ": Flipping green...";

		for ( size_t y = 0; y < inImage->height; ++y ) {
			auto inRow = reinterpret_cast<const T *>( inImage->pixels + y * inImage->rowPitch );
			auto outRow = reinterpret_cast<T *>( outImage->pixels + y * outImage->rowPitch );

			for ( size_t x = 0; x < inImage->width; ++x ) {
				outRow[x * 2 + 0] = inRow[x * 2 + 0];
				outRow[x * 2 + 1] = ( (T) -1 ) - inRow[x * 2 + 1]; // TODO: fix for float/snorm/sint types
			}
		}

		std::cout << "done!" << std::endl;
	}
};