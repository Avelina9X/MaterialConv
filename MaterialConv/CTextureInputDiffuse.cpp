#include "CTextureInputDIffuse.hpp"

CTextureInputDiffuse::CTextureInputDiffuse( CTextureDefinitionFile &definitionFile ) :
	CTextureInputBase( definitionFile, "InDiffuse" )
{
	if ( InputChannels( GetMetadata().format ) != 4 ) {
		std::cerr << "Diffuse texture must output 4 channels! Check your swizzle!" << std::endl;
		throw std::exception( "Invalid swizzle" );
	}

	bool premultiplyAlpha = definitionFile.GetField<bool>( m_sSection, "Premultiply", false );
	ApplyPremultipliedAlpha( premultiplyAlpha );
}