#include "CTextureOutputPreview.hpp"

using namespace DirectX;

CTextureOutputPreview::CTextureOutputPreview( CTextureDefinitionFile &definitionFile, CTextureInputPreview &previewTexture ) :
	m_sFilename( "preview.jpg" ),
	m_filePath( definitionFile.GetOutDir() / m_sFilename ),
	m_fOutputFormat( previewTexture.GetMetadata().format ),
	m_bIsSRGB( previewTexture.IsSRGB() )
{
	m_previewTexture = std::make_unique<ScratchImage>();

	HRESULT hr = m_previewTexture->InitializeFromImage( *previewTexture.GetImages() );
	if ( FAILED( hr ) ) {
		std::cerr << "Could not initialise output preview texture!" << std::endl;
		throw std::exception( "COM exception" );
	}
}

void CTextureOutputPreview::SaveFile()
{
	std::cout << PrettySection() << ": Saving file...";
	HRESULT hr = SaveToWICFile(
		GetImages(),
		GetImageCount(),
		WIC_FLAGS_NONE,
		GetWICCodec( WIC_CODEC_JPEG ),
		GetOutputPath().wstring().c_str()
	);

	if ( FAILED( hr ) ) {
		std::cerr << "Failed to save file to " << GetOutputPath() << "with error" << hr << std::endl;
		throw std::exception( "File save error" );
	}

	std::cout << "done!" << std::endl;
}