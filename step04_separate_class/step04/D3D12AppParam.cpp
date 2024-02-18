// D3D12AppParam.cpp
//  
//   Written by Tunjay Akbarli
// ===========================================
// include

#include <stdexcept>
#include <wrl.h>            // Microsoft::WRL::ComPtr

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")

#include "D3D12AppParam.h"

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;


D3D12AppParam::D3D12AppParam()
{
}

D3D12AppParam::~D3D12AppParam()
{
}

void D3D12AppParam::Initialize(HWND hwnd)
{
    init_search_hardware_adapter(hwnd);
    init_direct3d_device(hwnd);
}

/// <summary>
/// init search hardware adapter
/// </summary>
void D3D12AppParam::init_search_hardware_adapter(HWND hwnd)
{
    // create IDXGIFactory3
    UINT dxgiFlags = 0;
    ComPtr<ID3D12Debug> debug;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
    {
        debug->EnableDebugLayer();
        dxgiFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
    ComPtr<IDXGIFactory3> factory;
    if (FAILED(CreateDXGIFactory2(dxgiFlags, IID_PPV_ARGS(&factory))))
    {
        throw std::runtime_error("CreateDXGIFactory2 failed.");
    }

    // search hardware adapter
    UINT adapterIndex = 0;
    ComPtr<IDXGIAdapter1> p_useAdapter;
    ComPtr<IDXGIAdapter1> p_adapter;
    while (DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &p_adapter))
    {
        DXGI_ADAPTER_DESC1 desc1{};
        p_adapter->GetDesc1(&desc1);
        ++adapterIndex;
        if (desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        // can use D3D12?
        auto hr = D3D12CreateDevice(
            p_adapter.Get(),
            //            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_12_1,
            __uuidof(ID3D12Device), nullptr);
        if (SUCCEEDED(hr))
        {
            break;
        }
    }
    p_adapter.As(&p_useAdapter);   // use adapter!
}

/// <summary>
/// init Direct3D device
/// </summary>
void D3D12AppParam::init_direct3d_device(HWND hwnd)
{
    // init Direct3D device
    pDevice_ = nullptr;
    auto hr = D3D12CreateDevice(
        nullptr
        , D3D_FEATURE_LEVEL_12_1
        , IID_PPV_ARGS(&pDevice_));
    if (FAILED(hr))
    {
        throw new std::runtime_error("D3D12CreateDevice failed.");
    }
}

// ===========================================
// End of file
