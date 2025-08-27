#include "EMaterialType.hpp"

#include <iostream>

const EMaterialType ParseMaterialType( std::string type )
{
    if ( type == "PBR:POM" ) return EMaterialType::PBR_POM;

    std::cerr << "Unknown material type: " << type << std::endl;
    throw std::exception( "Unknown mateiral type" );
}