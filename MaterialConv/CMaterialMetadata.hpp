#pragma once

#include "IMaterialMetadata.hpp"

class CMaterialMetadata : public IMaterialMetadata
{
public:
	CMaterialMetadata( IMaterialMetadata &data ) :
		m_materialSection( data.GetMaterialSection() ),
		m_materialKeyValues( data.GetMaterialKeyValues() ) {
	}

	CMaterialMetadata( std::string section, std::map<std::string, std::string> keyValues ) :
		m_materialSection( section ),
		m_materialKeyValues( keyValues ) {
	}

	std::string GetMaterialSection() override { return m_materialSection; }
	std::map<std::string, std::string> GetMaterialKeyValues() { return m_materialKeyValues; }

protected:
	const std::string m_materialSection;
	const std::map<std::string, std::string> m_materialKeyValues;
};