#include "EMaterialType.hpp"

#include <iostream>

const EMaterialType ParseMaterialType( std::string type )
{
    if ( type == "PBR:POM" ) return EMaterialType::PBR_POM;

    std::cerr << "Unknown material type: " << type << std::endl;
    throw std::exception( "Unknown mateiral type" );
}

const std::string GetMaterialName( EMaterialType type )
{
    if ( type == EMaterialType::PBR_POM ) return "PBR:POM";

    throw std::exception( "Unknown mateiral type" );
}