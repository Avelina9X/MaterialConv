#pragma once

#include <string>
#include <iostream>
#include <algorithm>

#include "TexUtils.hpp"

struct SSwizzle
{
public:
	SSwizzle( std::string swizzle ) :
		m_iInputChannels( 0 ),
		m_iOutputChannels( swizzle.length() )
	{
		if ( swizzle.length() > 4 || swizzle.length() == 0 ) {
			std::cerr << "Swizzle must be between 1 and 4 characters long!" << std::endl;
			throw std::exception( "Invalid swizzle" );
		}

		for ( int i = 0; i < 4; ++i ) {
			if ( i >= swizzle.length() ) {
				m_acSwizzle[i] = (char) 0;
			}

			else {
				switch ( swizzle[i] ) {
				case 'r':
					m_iInputChannels = std::max( m_iInputChannels, (size_t) 1 );
					m_acSwizzle[i] = swizzle[i];
					break;

				case 'g':
					m_iInputChannels = std::max( m_iInputChannels, (size_t) 2 );
					m_acSwizzle[i] = swizzle[i];
					break;

				case 'b':
					m_iInputChannels = std::max( m_iInputChannels, (size_t) 3 );
					m_acSwizzle[i] = swizzle[i];
					break;

				case 'a':
					m_iInputChannels = std::max( m_iInputChannels, (size_t) 4 );
					m_acSwizzle[i] = swizzle[i];
					break;

				case '0':
					m_acSwizzle[i] = swizzle[i];
					break;

				case '1':
					m_acSwizzle[i] = swizzle[i];
					break;

				default:
					std::cerr << "Swizzle must only contain characters rgba10!" << std::endl;
					throw std::exception( "Invalid swizzle" );
				}
			}
		}
	}

	bool ValidateInputFormat( DXGI_FORMAT inputFormat ) {
		return InputChannels( inputFormat ) >= m_iInputChannels;
	}

	DXGI_FORMAT GetOutputFormat( DXGI_FORMAT inputFormat ) {
		return CreateOutputFormat( inputFormat, m_iOutputChannels );
	}

	const char operator[]( size_t c ) { return m_acSwizzle[c]; } // TODO: bounds check!


protected:
	char m_acSwizzle[4];
	size_t m_iInputChannels;
	size_t m_iOutputChannels;
};