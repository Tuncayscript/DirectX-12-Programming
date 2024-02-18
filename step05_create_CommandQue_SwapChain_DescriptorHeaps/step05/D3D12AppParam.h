#pragma once
// D3D12AppParam.h
//  
//   Written by Tunjay Akbarli
// ===========================================
#define WIN32_LEAN_AND_MEAN
#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_6.h>

class D3D12AppParam
{
public:
    D3D12AppParam();
    virtual  ~D3D12AppParam();

    void Initialize(HWND hWnd);

private:
    const UINT FrameBufferCount = 2;    // both sides

    Microsoft::WRL::ComPtr<IDXGIFactory3> pDxgiFactory_;

    Microsoft::WRL::ComPtr<ID3D12Device> pDevice_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> pCommandQueue_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> pSwapchain_;

    // Render Target View
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeapRtv_;
    UINT rtvDescriptorSize_;
    // Depth Stencil View
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeapDsv_;


    void init_search_hardware_adapter(HWND hwnd);
    void init_direct3d_device(HWND hwnd);
    void create_command_que(HWND hwnd);
    void create_swap_chain(HWND hwnd);
    void create_descriptor_heaps(HWND hwnd);
};
// ===========================================
// End of file