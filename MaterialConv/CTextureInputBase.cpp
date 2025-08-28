#include "CTextureInputBase.hpp"

using namespace DirectX;

HRESULT LoadImageFromFile( std::filesystem::path file, bool isSRGB, ScratchImage &image )
{
	if ( !std::filesystem::exists( file ) ) {
		std::cerr << "File not found: " << file.generic_string() << std::endl;
		throw std::exception( "File not found" );
	}

	auto ext = file.extension().string();

	std::transform( ext.begin(), ext.end(), ext.begin(), ::tolower );

	if ( ext == ".tga" ) {
		return LoadFromTGAFile( file.wstring().c_str(), isSRGB ? TGA_FLAGS_FORCE_SRGB : TGA_FLAGS_IGNORE_SRGB, nullptr, image );
	}

	return LoadFromWICFile( file.wstring().c_str(), isSRGB ? WIC_FLAGS_FORCE_SRGB : WIC_FLAGS_IGNORE_SRGB, nullptr, image );
}


CTextureInputBase::CTextureInputBase( CTextureDefinitionFile &definitionFile, std::string section ) :
	m_sSection( section ),
	m_sSwizzle( definitionFile.GetField<std::string>( m_sSection, "Swizzle" ) ),
	m_bIsSRGB( definitionFile.GetField<bool>( m_sSection, "SRGB" ) )
{
	auto path = definitionFile.GetFieldWithPathPrefix( m_sSection, "File", "$INDIR", definitionFile.GetInDir() );

	m_inputTexture = std::make_unique<ScratchImage>();

	std::cout << PrettySection() << ": Loading image...";

	if ( FAILED( LoadImageFromFile( path, IsSRGB(), *m_inputTexture.get() ) ) ) {
		std::cerr << "Failed to load image " << path << std::endl;
		throw std::exception( "File not found" );
	}

	std::cout << "done!" << std::endl;

	auto &inputMetadata = m_inputTexture->GetMetadata();
	assert( m_sSwizzle.ValidateInputFormat( inputMetadata.format ) );

	m_swizzleTexture = std::make_unique<ScratchImage>();
	m_swizzleTexture->Initialize2D( m_sSwizzle.GetOutputFormat( inputMetadata.format ), inputMetadata.width, inputMetadata.height, 1, 1 );
	auto &swizzleMetadata = m_swizzleTexture->GetMetadata();

	auto bitDepth = BitsPerColor( inputMetadata.format );
	auto inChannels = InputChannels( inputMetadata.format );
	auto outChannels = InputChannels( swizzleMetadata.format );

	switch ( FormatDataType( inputMetadata.format ) ) {
		case FORMAT_TYPE_UNORM:
			if ( bitDepth == 8 ) {
				ComputeSwizzle<uint8_t>( inChannels, outChannels, m_inputTexture->GetImages(), m_swizzleTexture->GetImages() );
				break;
			}

			if ( bitDepth == 16 ) {
				ComputeSwizzle<uint16_t>( inChannels, outChannels, m_inputTexture->GetImages(), m_swizzleTexture->GetImages() );
				break;
			}

			std::cerr << "Unsupported bitdepth!" << std::endl;
			throw std::exception( "DXGI_Format error" );

		default:
			std::cerr << "Unsupported format!" << std::endl;
			throw std::exception( "DXGI_Format error" );
	}
}


void CTextureInputBase::ApplyPremultipliedAlpha( bool premultiplyAlpha )
{
	if ( HasAlpha( GetMetadata().format ) && premultiplyAlpha ) {
		if ( GetMetadata().IsPMAlpha() ) {
			std::cerr << "Image is already using premultiplied alpha!" << std::endl;
			throw std::exception( "PMA error" );
		}

		std::unique_ptr<ScratchImage> timage = std::make_unique<ScratchImage>();

		std::cout << PrettySection() << ": Premultiplying...";

		HRESULT hr = PremultiplyAlpha( GetImages(), GetImageCount(), GetMetadata(), IsSRGB() ? TEX_PMALPHA_SRGB : TEX_PMALPHA_IGNORE_SRGB, *timage );
		if ( FAILED( hr ) ) {
			std::cerr << "Could not premultiply alpha!" << std::endl;
			throw std::exception( "PMA error" );
		}

		std::cout << "done!" << std::endl;

		m_swizzleTexture.swap( timage );
	}
}