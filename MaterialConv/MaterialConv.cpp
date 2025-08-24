#include <iostream>
#include <DirectXTex.h>

int main()
{
    HRESULT hr = CoInitializeEx( nullptr, COINIT_MULTITHREADED );
    if ( FAILED( hr ) ) {
        wprintf( L"Failed to initialize COM (%08X)\n", static_cast<unsigned int>( hr ) );
        return 1;
    }
}
