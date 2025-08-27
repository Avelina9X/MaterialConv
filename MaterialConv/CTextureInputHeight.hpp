#pragma once

#include <sstream>
#include <algorithm>
#include <vector>

#include "CTextureInputBase.hpp"
#include "TexUtils.hpp"


class CTextureInputHeight : public CTextureInputBase
{
public:
	CTextureInputHeight( CTextureDefinitionFile &definitionFile ) :
		CTextureInputBase( definitionFile, "InHeight" ) {

		if ( InputChannels( GetMetadata().format ) != 1 ) {
			std::cerr << "Normal texture must output 1 channel! Check your swizzle!" << std::endl;
			throw std::exception( "Invalid swizzle" );
		}

		if ( IsSRGB() ) {
			std::cerr << "Height texture cannot be SRGB!" << std::endl;
			throw std::exception( "SRGB error" );
		}

		auto sMinValue = definitionFile.GetField<std::string>( m_sSection, "MinValue", "1.0" );
		auto sMaxValue = definitionFile.GetField<std::string>( m_sSection, "MaxValue", "0.0" );

		MinMaxType minType;
		MinMaxType maxType;

		if ( IsFloat( sMinValue ) ) {
			minType = MinMaxType::Float;
		}
		else if ( IsPercentile( sMinValue ) ) {
			minType = MinMaxType::Percentile;
		}
		else {
			std::cerr << "MinValue has incorrect format!" << std::endl;
			throw std::exception( "Invalid type" );
		}

		if ( IsFloat( sMaxValue ) ) {
			maxType = MinMaxType::Float;
		}
		else if ( IsPercentile( sMaxValue ) ) {
			maxType = MinMaxType::Percentile;
		}
		else {
			std::cerr << "MaxValue has incorrect format!" << std::endl;
			throw std::exception( "Invalid type" );
		}

		if ( minType != maxType ) {
			std::cerr << "MinValue and MaxValue are different formats!" << std::endl;
			throw std::exception( "Invalid type" );
		}

		float fMinValue;
		float fMaxValue;

		auto bitDepth = DirectX::BitsPerColor( GetMetadata().format );

		if ( minType == MinMaxType::Percentile ) {
			switch ( DirectX::FormatDataType( GetMetadata().format ) ) {
				case DirectX::FORMAT_TYPE_UNORM:
					if ( bitDepth == 8 ) {
						ComputePercentile<uint8_t>( GetImages(), ParsePercentile( sMinValue ), ParsePercentile( sMaxValue ), &fMinValue, &fMaxValue );
						break;
					}

					if ( bitDepth == 16 ) {
						ComputePercentile<uint16_t>( GetImages(), ParsePercentile( sMinValue ), ParsePercentile( sMaxValue ), &fMinValue, &fMaxValue );
						break;
					}

					std::cerr << "Unsupported bitdepth!" << std::endl;
					throw std::exception( "DXGI_Format error" );

				default:
					std::cerr << "Unsupported format!" << std::endl;
					throw std::exception( "DXGI_Format error" );
			}
		}
		else {
			fMinValue = ParseFloat( sMinValue );
			fMaxValue = ParseFloat( sMaxValue );
		}

		m_fHeightScale = std::abs( fMaxValue - fMinValue );

		std::unique_ptr<DirectX::ScratchImage> timage = std::make_unique<DirectX::ScratchImage>();
		timage->Initialize2D( GetMetadata().format, GetMetadata().width, GetMetadata().height, 1, 1 );

		switch ( DirectX::FormatDataType( GetMetadata().format ) ) {
			case DirectX::FORMAT_TYPE_UNORM:
				if ( bitDepth == 8 ) {
					RescaleFloat<uint8_t>( GetImages(), timage->GetImages(), fMinValue, fMaxValue );
					break;
				}

				if ( bitDepth == 16 ) {
					RescaleFloat<uint16_t>( GetImages(), timage->GetImages(), fMinValue, fMaxValue );
					break;
				}

				std::cerr << "Unsupported bitdepth!" << std::endl;
				throw std::exception( "DXGI_Format error" );

			default:
				std::cerr << "Unsupported format!" << std::endl;
				throw std::exception( "DXGI_Format error" );
		}

		m_swizzleTexture.swap( timage );
	}

	const float GetHeightScale() const { return m_fHeightScale; }

protected:
	float m_fHeightScale;

	bool IsFloat( std::string val ) {
		std::istringstream iss( val );
		float f;
		iss >> std::noskipws >> f;
		return iss.eof() && !iss.fail() && f >= 0.0f && f <= 1.0f;
	}

	float ParseFloat( std::string val ) {
		std::istringstream iss( val );
		float f;
		iss >> std::noskipws >> f;
		return f;
	}

	bool IsPercentile( std::string val ) {
		std::istringstream iss( val );
		char p;
		size_t i;
		iss >> std::noskipws >> p >> i;
		return iss.eof() && !iss.fail() && p == 'p' && i <= 100;
	}

	size_t ParsePercentile( std::string val ) {
		std::istringstream iss( val );
		char p;
		size_t i;
		iss >> std::noskipws >> p >> i;
		return i;
	}

	enum MinMaxType { Percentile, Float };

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