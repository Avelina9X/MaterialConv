#pragma once

#include <string>
#include <iostream>
#include <algorithm>

#include "TexUtils.hpp"

struct SSwizzle
{
public:
	SSwizzle( std::string swizzle );

	bool ValidateInputFormat( DXGI_FORMAT inputFormat );
	DXGI_FORMAT GetOutputFormat( DXGI_FORMAT inputFormat );

	const char operator[]( size_t c ) { return m_acSwizzle[c]; } // TODO: bounds check!

protected:
	char m_acSwizzle[4];
	size_t m_iInputChannels;
	size_t m_iOutputChannels;
};