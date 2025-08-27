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

#include "CTextureOutputDiffuse.hpp"
#include "CTextureOutputNormal.hpp"
#include "CTextureOutputHeight.hpp"
#include "CTextureOutputRMA.hpp"


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
    ThrowIfFailed( SaveToDDSFile(
        diffuseOutput.GetImages(),
        diffuseOutput.GetImageCount(),
        diffuseOutput.GetMetadata(),
        DDS_FLAGS_NONE,
        diffuseOutput.GetOutputPath().wstring().c_str()
    ) );
    std::cout << std::endl;


    CTextureInputNormal normal( definitionFile );
    CTextureOutputNormal normalOutput( definitionFile, normal, pDevice );
    ThrowIfFailed( SaveToDDSFile(
        normalOutput.GetImages(),
        normalOutput.GetImageCount(),
        normalOutput.GetMetadata(),
        DDS_FLAGS_NONE,
        normalOutput.GetOutputPath().wstring().c_str()
    ) );
    std::cout << std::endl;


    CTextureInputHeight height( definitionFile );
    CTextureOutputHeight heightOutput( definitionFile, height, pDevice );
    ThrowIfFailed( SaveToDDSFile(
        heightOutput.GetImages(),
        heightOutput.GetImageCount(),
        heightOutput.GetMetadata(),
        DDS_FLAGS_NONE,
        heightOutput.GetOutputPath().wstring().c_str()
    ) );
    std::cout << std::endl;

    CTextureInputRoughness roughness( definitionFile );
    CTextureInputMetalness metalness( definitionFile );
    CTextureInputAmbient ambient( definitionFile );
    CTextureOutputRMA rmaOutput( definitionFile, roughness, metalness, ambient, pDevice );
    ThrowIfFailed( SaveToDDSFile(
        rmaOutput.GetImages(),
        rmaOutput.GetImageCount(),
        rmaOutput.GetMetadata(),
        DDS_FLAGS_NONE,
        rmaOutput.GetOutputPath().wstring().c_str()
    ) );
    std::cout << std::endl;

    /*
    // Parse texture files
    auto diffuseFilePath = GetFieldWithPathPrefix( definitionFile, "InDiffuse", "File", "$INDIR", headerInDirPath );
    auto normalFilePath = GetFieldWithPathPrefix( definitionFile, "InNormal", "File", "$INDIR", headerInDirPath );
    auto ambientOcclusionFilePath = GetFieldWithPathPrefix( definitionFile, "InAmbientOcclusion", "File", "$INDIR", headerInDirPath );
    auto roughnessFilePath = GetFieldWithPathPrefix( definitionFile, "InRoughness", "File", "$INDIR", headerInDirPath );
    auto metalnessFilePath = GetFieldWithPathPrefix( definitionFile, "InMetalness", "File", "$INDIR", headerInDirPath );
    auto heightFilePath = GetFieldWithPathPrefix( definitionFile, "InHeight", "File", "$INDIR", headerInDirPath );

    std::cout << "InDiffuse:          " << diffuseFilePath.generic_string() << std::endl;
    std::cout << "InNormal:           " << normalFilePath.generic_string() << std::endl;
    std::cout << "InAmbientOcclusion: " << ambientOcclusionFilePath.generic_string() << std::endl;
    std::cout << "InRoughness:        " << roughnessFilePath.generic_string() << std::endl;
    std::cout << "InMetalness:        " << metalnessFilePath.generic_string() << std::endl;
    std::cout << "InHeight:           " << heightFilePath.generic_string() << std::endl;
    std::cout << std::endl;

    ScratchImage diffuseImage; ThrowIfFailed( LoadFromWICFile( diffuseFilePath.wstring().c_str(), WIC_FLAGS_FORCE_SRGB, nullptr, diffuseImage ) );

    ScratchImage diffuseImageConv;
    ThrowIfFailed( Convert( diffuseImage.GetImages(), diffuseImage.GetImageCount(), diffuseImage.GetMetadata(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, TEX_FILTER_SRGB_IN, TEX_THRESHOLD_DEFAULT, diffuseImageConv ) );
    diffuseImage.Release();

    ThrowIfFailed( GenerateMipMaps( diffuseImageConv.GetImages(), diffuseImageConv.GetImageCount(), diffuseImageConv.GetMetadata(), TEX_FILTER_SRGB, 0, diffuseImage ) );
    diffuseImageConv.Release();

    ThrowIfFailed( Compress( diffuseImage.GetImages(), diffuseImage.GetImageCount(), diffuseImage.GetMetadata(), DXGI_FORMAT_BC3_UNORM_SRGB, TEX_COMPRESS_SRGB_OUT, TEX_THRESHOLD_DEFAULT, diffuseImageConv ) );
    diffuseImage.Release();

    SaveToDDSFile( diffuseImageConv.GetImages(), diffuseImageConv.GetImageCount(), diffuseImageConv.GetMetadata(), DDS_FLAGS_NONE, L"test.dds" );


    ScratchImage normalImage; ThrowIfFailed( LoadFromWICFile( normalFilePath.wstring().c_str(), WIC_FLAGS_IGNORE_SRGB, nullptr, normalImage ) );
    ScratchImage ambientOcclusionImage; ThrowIfFailed( LoadFromWICFile( ambientOcclusionFilePath.wstring().c_str(), WIC_FLAGS_IGNORE_SRGB, nullptr, ambientOcclusionImage ) );
    ScratchImage roughnessImage; ThrowIfFailed( LoadFromWICFile( roughnessFilePath.wstring().c_str(), WIC_FLAGS_IGNORE_SRGB, nullptr, roughnessImage ) );
    ScratchImage metalnessImage; ThrowIfFailed( LoadFromWICFile( metalnessFilePath.wstring().c_str(), WIC_FLAGS_IGNORE_SRGB, nullptr, metalnessImage ) );
    ScratchImage heightImage; ThrowIfFailed( LoadFromWICFile( heightFilePath.wstring().c_str(), WIC_FLAGS_IGNORE_SRGB, nullptr, heightImage ) );

    DXGI_FORMAT;

    std::cout << "Format Diffuse:          " << diffuseImageConv.GetImages()->format << std::endl;
    std::cout << "Format Normal:           " << normalImage.GetImages()->format << std::endl;
    std::cout << "Format AmbientOcclusion: " << ambientOcclusionImage.GetImages()->format << std::endl;
    std::cout << "Format Roughness:        " << roughnessImage.GetImages()->format << std::endl;
    std::cout << "Format Metalness:        " << metalnessImage.GetImages()->format << std::endl;
    std::cout << "Format Height:           " << heightImage.GetImages()->format << std::endl;
    std::cout << std::endl;

    ScratchImage diffuseImage2; ThrowIfFailed( LoadFromWICFile( diffuseFilePath.wstring().c_str(), WIC_FLAGS_FORCE_SRGB, nullptr, diffuseImage2 ) );

    ScratchImage diffuseImageConv2;
    ThrowIfFailed( GenerateMipMaps( diffuseImage2.GetImages(), diffuseImage2.GetImageCount(), diffuseImage2.GetMetadata(), TEX_FILTER_SRGB, 0, diffuseImageConv2 ) );
    diffuseImage2.Release();

    ThrowIfFailed( Compress( diffuseImageConv2.GetImages(), diffuseImageConv2.GetImageCount(), diffuseImageConv2.GetMetadata(), DXGI_FORMAT_BC3_UNORM_SRGB, TEX_COMPRESS_SRGB, TEX_THRESHOLD_DEFAULT, diffuseImage2 ) );
    diffuseImageConv2.Release();

    SaveToDDSFile( diffuseImage2.GetImages(), diffuseImage2.GetImageCount(), diffuseImage2.GetMetadata(), DDS_FLAGS_NONE, L"test2.dds" );
    */
}
