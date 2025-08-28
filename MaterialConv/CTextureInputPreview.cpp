#include "CTextureInputPreview.hpp"

CTextureInputPreview::CTextureInputPreview( CTextureDefinitionFile &definitionFile ) :
	CTextureInputBase( definitionFile, "InPreview" )
{

	if ( InputChannels( GetMetadata().format ) != 4 ) {
		std::cerr << "Preview texture must output 4 channels! Check your swizzle!" << std::endl;
		throw std::exception( "Invalid swizzle" );
	}

	bool premultiplyAlpha = definitionFile.GetField<bool>( m_sSection, "Premultiply", false );
	ApplyPremultipliedAlpha( premultiplyAlpha );
}