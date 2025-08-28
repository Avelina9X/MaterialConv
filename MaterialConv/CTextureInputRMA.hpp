#pragma once

#include "CTextureInputBase.hpp"
#include "TexUtils.hpp"

class CTextureInputRMA : public CTextureInputBase
{
protected:
	CTextureInputRMA( CTextureDefinitionFile &definitionFile, std::string section );

public:
	void Recast( DXGI_FORMAT destination );
};

class CTextureInputRoughness : public CTextureInputRMA
{
public:
	CTextureInputRoughness( CTextureDefinitionFile &definitionFile ) : CTextureInputRMA( definitionFile, "InRoughness" ) {}
};

class CTextureInputMetalness : public CTextureInputRMA
{
public:
	CTextureInputMetalness( CTextureDefinitionFile &definitionFile ) : CTextureInputRMA( definitionFile, "InMetalness" ) {}
};

class CTextureInputAmbient : public CTextureInputRMA
{
public:
	CTextureInputAmbient( CTextureDefinitionFile &definitionFile ) : CTextureInputRMA( definitionFile, "InAmbientOcclusion" ) {}
};