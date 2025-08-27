#include <iostream>
#include <wrl\client.h>

#include "TexUtils.hpp"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

size_t InputChannels( DXGI_FORMAT inputFormat )
{
	switch ( DirectX::MakeTypeless( inputFormat ) )
	{
		// 1 channel
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_BC4_TYPELESS:
			return 1;

		// 2 channels
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_BC5_TYPELESS:
			return 2;

		// 3 channels
		case DXGI_FORMAT_R32G32B32_TYPELESS:
			return 3;

		// 4 channels
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC7_TYPELESS:
			return 4;

		// Compressed / depth-stencil / typeless / etc.
		default:
			std::cerr << "Unknown input format! " << inputFormat << std::endl;
			throw std::exception( "DXGI_Format error" );
	}
}

DXGI_FORMAT CreateOutputFormat( DXGI_FORMAT inputFormat, size_t outChannels ) {
	auto outFormat = DirectX::FormatDataType( inputFormat );
	auto outBitdepth = DirectX::BitsPerColor( inputFormat );

	using namespace DirectX;

	switch ( outFormat ) {
		case FORMAT_TYPE_FLOAT:
			//if ( outChannels == 1 && outBitdepth == 8 ) return DXGI_FORMAT_R8_FLOAT;
			if ( outChannels == 1 && outBitdepth == 16 ) return DXGI_FORMAT_R16_FLOAT;
			if ( outChannels == 1 && outBitdepth == 32 ) return DXGI_FORMAT_R32_FLOAT;

			//if ( outChannels == 2 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8_FLOAT;
			if ( outChannels == 2 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16_FLOAT;
			if ( outChannels == 2 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32_FLOAT;

			//if ( outChannels == 3 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8_FLOAT;
			//if ( outChannels == 3 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16_FLOAT;
			if ( outChannels == 3 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32_FLOAT;

			//if ( outChannels == 4 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8A8_FLOAT;
			if ( outChannels == 4 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16A16_FLOAT;
			if ( outChannels == 4 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32A32_FLOAT;

			break;

		case FORMAT_TYPE_UNORM:
			if ( outChannels == 1 && outBitdepth == 8 ) return DXGI_FORMAT_R8_UNORM;
			if ( outChannels == 1 && outBitdepth == 16 ) return DXGI_FORMAT_R16_UNORM;
			//if ( outChannels == 1 && outBitdepth == 32 ) return DXGI_FORMAT_R32_UNORM;

			if ( outChannels == 2 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8_UNORM;
			if ( outChannels == 2 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16_UNORM;
			//if ( outChannels == 2 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32_UNORM;

			//if ( outChannels == 3 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8_UNORM;
			//if ( outChannels == 3 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16_UNORM;
			//if ( outChannels == 3 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32_UNORM;

			if ( outChannels == 4 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8A8_UNORM;
			if ( outChannels == 4 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16A16_UNORM;
			//if ( outChannels == 4 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32A32_UNORM;

			break;

		case FORMAT_TYPE_SNORM:
			if ( outChannels == 1 && outBitdepth == 8 ) return DXGI_FORMAT_R8_SNORM;
			if ( outChannels == 1 && outBitdepth == 16 ) return DXGI_FORMAT_R16_SNORM;
			//if ( outChannels == 1 && outBitdepth == 32 ) return DXGI_FORMAT_R32_SNORM;

			if ( outChannels == 2 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8_SNORM;
			if ( outChannels == 2 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16_SNORM;
			//if ( outChannels == 2 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32_SNORM;

			//if ( outChannels == 3 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8_SNORM;
			//if ( outChannels == 3 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16_SNORM;
			//if ( outChannels == 3 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32_SNORM;

			if ( outChannels == 4 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8A8_SNORM;
			if ( outChannels == 4 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16A16_SNORM;
			//if ( outChannels == 4 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32A32_SNORM;

			break;

		case FORMAT_TYPE_UINT:
			if ( outChannels == 1 && outBitdepth == 8 ) return DXGI_FORMAT_R8_UINT;
			if ( outChannels == 1 && outBitdepth == 16 ) return DXGI_FORMAT_R16_UINT;
			if ( outChannels == 1 && outBitdepth == 32 ) return DXGI_FORMAT_R32_UINT;

			if ( outChannels == 2 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8_UINT;
			if ( outChannels == 2 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16_UINT;
			if ( outChannels == 2 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32_UINT;

			//if ( outChannels == 3 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8_UINT;
			//if ( outChannels == 3 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16_UINT;
			if ( outChannels == 3 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32_UINT;

			if ( outChannels == 4 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8A8_UINT;
			if ( outChannels == 4 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16A16_UINT;
			if ( outChannels == 4 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32A32_UINT;

			break;

		case FORMAT_TYPE_SINT:
			if ( outChannels == 1 && outBitdepth == 8 ) return DXGI_FORMAT_R8_SINT;
			if ( outChannels == 1 && outBitdepth == 16 ) return DXGI_FORMAT_R16_SINT;
			if ( outChannels == 1 && outBitdepth == 32 ) return DXGI_FORMAT_R32_SINT;

			if ( outChannels == 2 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8_SINT;
			if ( outChannels == 2 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16_SINT;
			if ( outChannels == 2 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32_SINT;

			//if ( outChannels == 3 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8_SINT;
			//if ( outChannels == 3 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16_SINT;
			if ( outChannels == 3 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32_SINT;

			if ( outChannels == 4 && outBitdepth == 8 ) return DXGI_FORMAT_R8G8B8A8_SINT;
			if ( outChannels == 4 && outBitdepth == 16 ) return DXGI_FORMAT_R16G16B16A16_SINT;
			if ( outChannels == 4 && outBitdepth == 32 ) return DXGI_FORMAT_R32G32B32A32_SINT;

			break;

		default:
			break;
	}

	std::cerr << "Unknown input format!" << std::endl;
	throw std::exception( "DXGI_Format error" );
}

bool GetDXGIFactory( IDXGIFactory1 **pFactory )
{
	if ( !pFactory )
		return false;

	*pFactory = nullptr;

	typedef HRESULT( WINAPI *pfn_CreateDXGIFactory1 )( REFIID riid, _Out_ void **ppFactory );

	static pfn_CreateDXGIFactory1 s_CreateDXGIFactory1 = nullptr;

	if ( !s_CreateDXGIFactory1 )
	{
		HMODULE hModDXGI = LoadLibraryW( L"dxgi.dll" );
		if ( !hModDXGI )
			return false;

		s_CreateDXGIFactory1 = reinterpret_cast<pfn_CreateDXGIFactory1>( reinterpret_cast<void *>( GetProcAddress( hModDXGI, "CreateDXGIFactory1" ) ) );
		if ( !s_CreateDXGIFactory1 )
			return false;
	}

	return SUCCEEDED( s_CreateDXGIFactory1( IID_PPV_ARGS( pFactory ) ) );
}

bool CreateDevice( int adapter, ID3D11Device * *pDevice )
{
	if ( !pDevice )
		return false;

	*pDevice = nullptr;

	static PFN_D3D11_CREATE_DEVICE s_DynamicD3D11CreateDevice = nullptr;

	if ( !s_DynamicD3D11CreateDevice )
	{
		HMODULE hModD3D11 = LoadLibraryW( L"d3d11.dll" );
		if ( !hModD3D11 )
			return false;

		s_DynamicD3D11CreateDevice = reinterpret_cast<PFN_D3D11_CREATE_DEVICE>( reinterpret_cast<void *>( GetProcAddress( hModD3D11, "D3D11CreateDevice" ) ) );
		if ( !s_DynamicD3D11CreateDevice )
			return false;
	}

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ComPtr<IDXGIAdapter> pAdapter;
	if ( adapter >= 0 )
	{
		ComPtr<IDXGIFactory1> dxgiFactory;
		if ( GetDXGIFactory( dxgiFactory.GetAddressOf() ) )
		{
			if ( FAILED( dxgiFactory->EnumAdapters( static_cast<UINT>( adapter ), pAdapter.GetAddressOf() ) ) )
			{
				wprintf( L"\nERROR: Invalid GPU adapter index (%d)!\n", adapter );
				return false;
			}
		}
	}

	D3D_FEATURE_LEVEL fl;
	HRESULT hr = s_DynamicD3D11CreateDevice( pAdapter.Get(),
		( pAdapter ) ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,
		nullptr, createDeviceFlags, featureLevels, static_cast<UINT>( std::size( featureLevels ) ),
		D3D11_SDK_VERSION, pDevice, &fl, nullptr );
	if ( SUCCEEDED( hr ) )
	{
		if ( fl < D3D_FEATURE_LEVEL_11_0 )
		{
			D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
			hr = ( *pDevice )->CheckFeatureSupport( D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof( hwopts ) );
			if ( FAILED( hr ) )
				memset( &hwopts, 0, sizeof( hwopts ) );

			if ( !hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
			{
				if ( *pDevice )
				{
					( *pDevice )->Release();
					*pDevice = nullptr;
				}
				hr = HRESULT_FROM_WIN32( ERROR_NOT_SUPPORTED );
			}
		}
	}

	if ( SUCCEEDED( hr ) )
	{
		ComPtr<IDXGIDevice> dxgiDevice;
		hr = ( *pDevice )->QueryInterface( IID_PPV_ARGS( dxgiDevice.GetAddressOf() ) );
		if ( SUCCEEDED( hr ) )
		{
			hr = dxgiDevice->GetAdapter( pAdapter.ReleaseAndGetAddressOf() );
			if ( SUCCEEDED( hr ) )
			{
				DXGI_ADAPTER_DESC desc;
				hr = pAdapter->GetDesc( &desc );
				if ( SUCCEEDED( hr ) )
				{
					wprintf( L"[Using DirectCompute %ls on \"%ls\"]\n",
						( fl >= D3D_FEATURE_LEVEL_11_0 ) ? L"5.0" : L"4.0", desc.Description );
				}
			}
		}

		return true;
	}
	else
		return false;
}