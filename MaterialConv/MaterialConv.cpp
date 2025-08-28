#define NOMINMAX
#include <iostream>
#include <DirectXTex.h>
#include <string>
#include <filesystem>

#include <Windows.h>

#include "CTextureDefinitionFile.hpp"

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

const std::string DEFINITION_PATH = "D:/Projects/Resorces/Materials/Poliigon/Poliigon_BrickWallReclaimed_8320/definition.ini";
const std::string RESOURCES_DIR = "D:/Projects/Resorces/";

int main()
{
    // Init COM for DX functions to work
    ThrowIfFailed( CoInitializeEx( nullptr, COINIT_MULTITHREADED ) );

    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;

    if ( !CreateDevice( 0, pDevice.GetAddressOf() ) ) {
        std::cerr << "Failed to detect GPU! BC6H/BC7 compression is disabled!" << std::endl;
    }

    CTextureDefinitionFile definitionFile( DEFINITION_PATH, RESOURCES_DIR );

    CTextureInputDiffuse diffuse( definitionFile );
    CTextureOutputDiffuse diffuseOutput( definitionFile, diffuse, pDevice );
    diffuseOutput.SaveFile();
    std::cout << std::endl;

    CTextureInputNormal normal( definitionFile );
    CTextureOutputNormal normalOutput( definitionFile, normal, pDevice );
    normalOutput.SaveFile();
    std::cout << std::endl;

    CTextureInputRoughness roughness( definitionFile );
    CTextureInputMetalness metalness( definitionFile );
    CTextureInputAmbient ambient( definitionFile );
    CTextureOutputRMA rmaOutput( definitionFile, roughness, metalness, ambient, pDevice );
    rmaOutput.SaveFile();
    std::cout << std::endl;

    CTextureInputHeight height( definitionFile );
    CTextureOutputHeight heightOutput( definitionFile, height, pDevice );
    heightOutput.SaveFile();
    std::cout << std::endl;

    CTextureInputPreview preview( definitionFile );
    CTextureOutputPreview previewOutput( definitionFile, preview );
    previewOutput.SaveFile();
    std::cout << std::endl;


    IMaterialMetadata *materialArray[] = {
        &diffuseOutput,
        &normalOutput,
        &rmaOutput,
        &heightOutput,
        &previewOutput,
    };

    for ( IMaterialMetadata *i : materialArray ) {
        std::cout << "[" << i->GetMaterialSection() << "]" << std::endl;
        for ( auto const &[key, value] : i->GetMaterialKeyValues() ) {
            std::cout << key << "=" << value << std::endl;
        }
        std::cout << std::endl;
    }
}
