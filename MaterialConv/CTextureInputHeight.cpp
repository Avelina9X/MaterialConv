#include "CTextureInputHeight.hpp"

using namespace DirectX;

CTextureInputHeight::CTextureInputHeight( CTextureDefinitionFile &definitionFile ) :
	CTextureInputBase( definitionFile, "InHeight" )
{
	if ( InputChannels( GetMetadata().format ) != 1 ) {
		std::cerr << "Normal texture must output 1 channel! Check your swizzle!" << std::endl;
		throw std::exception( "Invalid swizzle" );
	}

	if ( IsSRGB() ) {
		std::cerr << "Height texture cannot be SRGB!" << std::endl;
		throw std::exception( "SRGB error" );
	}

	auto sMinValue = definitionFile.GetField<std::string>( m_sSection, "MinValue", "0.0" );
	auto sMaxValue = definitionFile.GetField<std::string>( m_sSection, "MaxValue", "1.0" );

	MinMaxType minType = GetType( sMinValue );
	MinMaxType maxType = GetType( sMaxValue );

	if ( minType != maxType ) {
		std::cerr << "MinValue and MaxValue are different formats!" << std::endl;
		throw std::exception( "Invalid type" );
	}

	float fMinValue;
	float fMaxValue;

	auto bitDepth = BitsPerColor( GetMetadata().format );

	if ( minType == MinMaxType::Percentile ) {
		auto minPercentile = ParsePercentile( sMinValue );
		auto maxPercentile = ParsePercentile( sMaxValue );

		switch ( FormatDataType( GetMetadata().format ) ) {
			case FORMAT_TYPE_UNORM:
				if ( bitDepth == 8 ) {
					ComputePercentile<uint8_t>( GetImages(), minPercentile, maxPercentile, &fMinValue, &fMaxValue );
					break;
				}

				if ( bitDepth == 16 ) {
					ComputePercentile<uint16_t>( GetImages(), minPercentile, maxPercentile, &fMinValue, &fMaxValue );
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

	std::unique_ptr<ScratchImage> timage = std::make_unique<ScratchImage>();
	timage->Initialize2D( GetMetadata().format, GetMetadata().width, GetMetadata().height, 1, 1 );

	switch ( FormatDataType( GetMetadata().format ) ) {
		case FORMAT_TYPE_UNORM:
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

CTextureInputHeight::MinMaxType CTextureInputHeight::GetType( std::string val )
{
	if ( IsFloat( val ) )  return MinMaxType::Float;
	if ( IsPercentile( val ) ) return MinMaxType::Percentile;

	std::cerr << "Cannot parse \"" << val << "\" as percentile or float!" << std::endl;
	throw std::exception( "Invalid type" );
}

bool CTextureInputHeight::IsFloat( std::string val )
{
	std::istringstream iss( val );
	float f;
	iss >> std::noskipws >> f;
	return iss.eof() && !iss.fail() && f >= 0.0f && f <= 1.0f;
}

float CTextureInputHeight::ParseFloat( std::string val )
{
	std::istringstream iss( val );
	float f;
	iss >> std::noskipws >> f;
	return f;
}

bool CTextureInputHeight::IsPercentile( std::string val )
{
	std::istringstream iss( val );
	char p;
	size_t i;
	iss >> std::noskipws >> p >> i;
	return iss.eof() && !iss.fail() && p == 'p' && i <= 100;
}

size_t CTextureInputHeight::ParsePercentile( std::string val )
{
	std::istringstream iss( val );
	char p;
	size_t i;
	iss >> std::noskipws >> p >> i;
	return i;
}