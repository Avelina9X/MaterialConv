#pragma once

#include <sstream>
#include <algorithm>
#include <vector>

#include "CTextureInputBase.hpp"
#include "TexUtils.hpp"


class CTextureInputHeight : public CTextureInputBase
{
public:
	CTextureInputHeight( CTextureDefinitionFile &definitionFile );

	const float GetHeightScale() const { return m_fHeightScale; }

protected:
	float m_fHeightScale;

	enum MinMaxType { Percentile, Float };

	MinMaxType GetType( std::string val );
	bool IsFloat( std::string val );
	float ParseFloat( std::string val );
	bool IsPercentile( std::string val );
	size_t ParsePercentile( std::string val );

	template<typename T>
	void ComputePercentile( const DirectX::Image *inImage, size_t minPercentile, size_t maxPercentile, float *minValue, float *maxValue ) {
		using namespace DirectX;
		std::cout << PrettySection() << ": Calculating percentiles...";

		double fMinPercentile = double( minPercentile ) / 100.0;
		double fMaxPercentile = double( maxPercentile ) / 100.0;

		double fBaseScale = double( (T) -1 ); // TODO: fix for float/snorm/sint types

		std::vector<T> aPixels;
		aPixels.reserve( inImage->width * inImage->height );

		for ( size_t y = 0; y < inImage->height; ++y ) {
			auto inRow = reinterpret_cast<const T *>( inImage->pixels + y * inImage->rowPitch );
			for ( size_t x = 0; x < inImage->width; ++x ) {
				aPixels.push_back( inRow[x] );
			}
		}

		std::sort( aPixels.begin(), aPixels.end() );

		*minValue = float( double( aPixels[static_cast<size_t>( ( aPixels.size() - 1 ) * fMinPercentile )] ) / fBaseScale );
		*maxValue = float( double( aPixels[static_cast<size_t>( ( aPixels.size() - 1 ) * fMaxPercentile )] ) / fBaseScale );

		std::cout << "done!" << std::endl;
	}

	template<typename T>
	void RescaleFloat( const DirectX::Image *inImage, const DirectX::Image *outImage, float minValue, float maxValue ) {
		using namespace DirectX;
		std::cout << PrettySection() << ": Rescaling...";

		double fScale = maxValue - minValue;
		double fBaseScale = double( (T) -1 ); // TODO: fix for float/snorm/sint types

		for ( size_t y = 0; y < inImage->height; ++y ) {
			auto inRow = reinterpret_cast<const T *>( inImage->pixels + y * inImage->rowPitch );
			auto outRow = reinterpret_cast<T *>( outImage->pixels + y * outImage->rowPitch );

			for ( size_t x = 0; x < inImage->width; ++x ) {
				double fVal = double( inRow[x] ) / fBaseScale;
				fVal -= minValue;
				fVal /= fScale;
				fVal = std::max( fVal, 0.0 );
				fVal = std::min( fVal, 1.0 );

				outRow[x] = static_cast<T>( fVal * fBaseScale );
			}
		}

		std::cout << "done!" << std::endl;
	}
};