#pragma once
// D3D12AppParam.h
//  
//   Written by Tunjay Akbarli
// ===========================================
#define WIN32_LEAN_AND_MEAN
#include <d3d12.h>
#include <wrl/client.h>

class D3D12AppParam
{
public:
    D3D12AppParam();
    virtual  ~D3D12AppParam();

    void Initialize(HWND hWnd);

    Microsoft::WRL::ComPtr<ID3D12Device> pDevice_;

private:
    void init_search_hardware_adapter(HWND hwnd);
    void init_direct3d_device(HWND hwnd);
};
// ===========================================
// End of file