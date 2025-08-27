#pragma once

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include "EMaterialType.hpp"
#include "inicpp.h"

// Defines the root of a texture defintion file with parsed headers
class CTextureDefinitionFile
{
public:
    CTextureDefinitionFile( std::string definitionFilePath, std::string resourceDirectoryPath );

    template<typename T> T GetField( std::string section, std::string key, std::optional<T> defaultValue = std::nullopt ) {
        // Check if section does not exist and throw error
        if ( m_pDefinitionFile->find( section ) == m_pDefinitionFile->end() ) {
            std::cerr << "Section [" << section << "] not found!" << std::endl;
            throw std::exception( "Section not found" );
        }

        // Check if key does not exist
        if ( m_pDefinitionFile->operator[]( section ).find( key ) == m_pDefinitionFile->operator[]( section ).end() ) {

            // If no default value is specified throw error
            if ( !defaultValue.has_value() ) {
                std::cerr << "Key \"" << key << "\" in section [" << section << "] not found and must be specified!" << std::endl;
                throw std::exception( "Key not found" );
            }

            // Otherwise return default value
            std::cout << "Key \"" << key << "\" in section [" << section << "] set to default value: " << defaultValue.value() << std::endl;
            return defaultValue.value();
        }

        // Get value as type
        return m_pDefinitionFile->operator[]( section )[key].as<T>();
    };

    // TODO: expose all prefixes
    const std::filesystem::path GetFieldWithPathPrefix( std::string section, std::string key, std::string prefix, std::filesystem::path replacement, std::optional<std::string> defaultValue = std::nullopt );

    const std::filesystem::path GetResDir() { return m_resourceDirectoryPath; }
    const std::filesystem::path GetDefDir() { return m_definitionFileDirectory; }
    const std::filesystem::path GetInDir() { return m_headerInDirPath; }
    const std::filesystem::path GetOutDir() { return m_headerOutDirPath; }

    const EMaterialType GetMaterialType() { return m_eMaterialType; }

protected:
    std::filesystem::path m_definitionFilePath;
    std::filesystem::path m_definitionFileDirectory;
    std::filesystem::path m_resourceDirectoryPath;

    std::filesystem::path m_headerInDirPath;
    std::filesystem::path m_headerOutDirPath;

    EMaterialType m_eMaterialType;

    std::unique_ptr<ini::IniFile> m_pDefinitionFile;
};