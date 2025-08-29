#include "CTextureDefinitionFile.hpp"

namespace fs = std::filesystem;

fs::path ReplacePathPrefix( std::string &path, std::string &prefix, fs::path &replacement )
{
    auto uPrefixPos = path.find( prefix );

    // No prefix in path
    if ( uPrefixPos == std::string::npos ) {
        return fs::path( path );
    }

    // Prefix at start
    if ( uPrefixPos == 0 ) {
        // Remove prefix from path
        std::string sStrippedPath = path.substr( prefix.length() );

        // Try stripping forward slash or backslash from front
        while ( sStrippedPath[0] == '\\' || sStrippedPath[0] == '/' ) {
            sStrippedPath = sStrippedPath.substr( 1 );
        }

        // Combine prefix path with stripped path
        return replacement / fs::path( sStrippedPath );
    }

    // Otherwise throw error because prefix not at start
    std::cerr << "Substring \"" << prefix << "\" found, but must be at start of string!";
    throw std::exception( "Incorrect prefix location" );
}

CTextureDefinitionFile::CTextureDefinitionFile( std::string definitionFilePath, std::string resourceDirectoryPath ) :
    m_definitionFilePath( definitionFilePath ),
    m_definitionFileDirectory( m_definitionFilePath.parent_path() ),
    m_resourceDirectoryPath( resourceDirectoryPath )
{
    // Check definition file actually exists
    if ( !fs::exists( m_definitionFilePath ) ) {
        std::cerr << "Definition file not fount: " << m_definitionFilePath << std::endl;
        throw std::exception( "File not found" );
    }

    // Check resource directory actually exists
    if ( !fs::exists( m_resourceDirectoryPath ) ) {
        std::cerr << "Resource directory not found: " << m_resourceDirectoryPath << std::endl;
        throw std::exception( "Directory not found" );
    }

    // Load definition file
    m_pDefinitionFile = std::make_unique<ini::IniFile>( m_definitionFilePath.string() );

    // Set $INDIR and $OUTDIR -- #TODO better container for definition constants?
    m_headerInDirPath = GetFieldWithPathPrefix( "Header", "InDir", "$RESDIR", m_resourceDirectoryPath );
    m_headerOutDirPath = GetFieldWithPathPrefix( "Header", "OutDir", "$DEFDIR", m_definitionFileDirectory );

    // Set $NAME
    m_namePrefix = GetField<std::string>( "Header", "Name" );

    // Parse material type
    m_eMaterialType = ParseMaterialType( GetField<std::string>( "Header", "Type" ) );
};

const fs::path CTextureDefinitionFile::GetFieldWithPathPrefix( std::string section, std::string key, std::string prefix, fs::path replacement, std::optional<std::string> defaultValue )
{
    auto field = GetField<std::string>( section, key, defaultValue );
    auto path = ReplacePathPrefix( field, prefix, replacement );

    return path.generic_string();
}