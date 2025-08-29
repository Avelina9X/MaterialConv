#pragma once

#include <string>

enum class EMaterialType
{
    PBR_POM,
};

const EMaterialType ParseMaterialType( std::string type );
const std::string GetMaterialTypeName( EMaterialType type );