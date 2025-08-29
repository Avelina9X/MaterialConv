#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <iostream>
#include <DirectXTex.h>
#include <string>
#include <filesystem>

#include <Windows.h>

#include "CTextureDefinitionFile.hpp"
#include "CMaterialDefinitionFile.hpp"

#include "CTextureInputDiffuse.hpp"
#include "CTextureInputNormal.hpp"
#include "CTextureInputHeight.hpp"
#include "CTextureInputRMA.hpp"
#include "CTextureInputPreview.hpp"

#include "CTextureOutputDiffuse.hpp"
#include "CTextureOutputNormal.hpp"
#include "CTextureOutputHeight.hpp"
#include "CTextureOutputRMA.hpp"
#include "CTextureOutputPreview.hpp"


void ThrowIfFailed( HRESULT hr )
{
    if ( FAILED( hr ) ) {
        wprintf( L"COM exception (%08X)\n", static_cast<unsigned int>( hr ) );
        throw std::exception( "Com exception" );
    }
}

using namespace DirectX;

namespace fs = std::filesystem;

int main( int argc, char *argv[] )
{
    std::string resourceDir;
    std::string definitionPath;

    if ( argc != 2 && argc != 4 ) {
        std::cerr << "Incorrect commandline input. Must be in format:" << std::endl;
        std::cerr << "    MaterialConv.exe -r [RESOURCE DIR] [INPUT FILE]" << std::endl;
        std::cerr << "    MaterialConv.exe [INPUT FILE]" << std::endl;
        return -1;
    }
    if ( argc == 2 ) {
        auto szResourceDir = std::getenv( "MaterialConvResDir" );

        if ( !szResourceDir ) {
            std::cerr << "MaterialConvResDir environment variable not set. Must pass resource dir:" << std::endl;
            std::cerr << "    MaterialConv.exe -r [RESOURCE DIR] [INPUT FILE]" << std::endl;
            return -1;
        }

        resourceDir = szResourceDir;
        definitionPath = argv[1];
    }
    if ( argc == 4 ) {
        if ( !std::strcmp( argv[1], "-r" ) ) {
            std::cerr << "Incorrect commandline input. Must be in format:" << std::endl;
            std::cerr << "    MaterialConv.exe -r [RESOURCE DIR] [INPUT FILE]" << std::endl;
            std::cerr << "    MaterialConv.exe [INPUT FILE]" << std::endl;
            return -1;
        }

        resourceDir = argv[2];
        definitionPath = argv[3];
    }

    // Init COM for DX functions to work
    ThrowIfFailed( CoInitializeEx( nullptr, COINIT_MULTITHREADED ) );

    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;

    if ( !CreateDevice( 0, pDevice.GetAddressOf() ) ) {
        std::cerr << "Failed to detect GPU! BC6H/BC7 compression is disabled!" << std::endl;
    }

    CTextureDefinitionFile definitionFile( definitionPath, resourceDir );
    CMaterialDefinitionFile materialFile( definitionFile, pDevice );
}
