#include <vector>
#include <fstream>

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

#include "CMaterialMetadata.hpp"

using Microsoft::WRL::ComPtr;

CMaterialDefinitionFile::CMaterialDefinitionFile( CTextureDefinitionFile &definitionFile, ComPtr<ID3D11Device> pDevice ) :
	m_sFilename( "material.ini" ),
	m_filePath( definitionFile.GetOutDir() / m_sFilename )
{
	std::vector<CMaterialMetadata> sections;

	auto materialName = definitionFile.GetName();
	auto materialType = definitionFile.GetMaterialType();
	auto materialTypeName = GetMaterialTypeName( materialType );

	CMaterialMetadata header( "Header", { { "Name", materialName }, { "Type", materialTypeName } } );

	sections.push_back( header );

	if ( materialType == EMaterialType::PBR_POM ) {
		CTextureInputDiffuse diffuseInput( definitionFile );
		CTextureInputNormal normalInput( definitionFile );
		CTextureInputRoughness roughnessInput( definitionFile );
		CTextureInputMetalness metalnessInput( definitionFile );
		CTextureInputAmbient ambientInput( definitionFile );
		CTextureInputHeight heightInput( definitionFile );
		CTextureInputPreview previewInput( definitionFile );

		std::cout << std::endl;

		CTextureOutputDiffuse diffuseOutput( definitionFile, diffuseInput, pDevice );
		CTextureOutputNormal normalOutput( definitionFile, normalInput, pDevice );
		CTextureOutputRMA rmaOutput( definitionFile, roughnessInput, metalnessInput, ambientInput, pDevice );
		CTextureOutputHeight heightOutput( definitionFile, heightInput, pDevice );
		CTextureOutputPreview previewOutput( definitionFile, previewInput );

		std::cout << std::endl;

		diffuseOutput.SaveFile();
		normalOutput.SaveFile();
		rmaOutput.SaveFile();
		heightOutput.SaveFile();
		previewOutput.SaveFile();

		std::cout << std::endl;

		sections.push_back( diffuseOutput );
		sections.push_back( normalOutput );
		sections.push_back( rmaOutput );
		sections.push_back( heightOutput );
		sections.push_back( previewOutput );
	}
	else {
		std::cerr << "Unknown material type" << std::endl;
		throw std::exception( "Unknown mateiral type" );
	}

	std::ofstream outFile;

	outFile.open( m_filePath, std::ios::out );

	if ( outFile.is_open() ) {

		std::cout << "Writing " << m_sFilename << "...";

		bool first = true;
		for ( IMaterialMetadata &i : sections ) {
			if ( first ) {
				first = false;
			}
			else {
				outFile << std::endl;
			}

			outFile << "[" << i.GetMaterialSection() << "]" << std::endl;
			for ( auto const &[key, value] : i.GetMaterialKeyValues() ) {
				outFile << key << "=" << value << std::endl;
			}
		}
		outFile.close();
		std::cout << "done!" << std::endl;
	}
	else {
		std::cerr << "Could not write definition file!" << std::endl;
		throw std::exception( "File write error" );
	}


}