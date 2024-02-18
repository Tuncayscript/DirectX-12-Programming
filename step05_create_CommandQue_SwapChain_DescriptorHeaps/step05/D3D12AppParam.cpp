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
    create_command_que(hwnd);
    create_swap_chain(hwnd);
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
    if (FAILED(CreateDXGIFactory2(dxgiFlags, IID_PPV_ARGS(&pDxgiFactory_))))
    {
        throw std::runtime_error("CreateDXGIFactory2 failed.");
    }

    // search hardware adapter
    UINT adapterIndex = 0;
    ComPtr<IDXGIAdapter1> p_useAdapter;
    ComPtr<IDXGIAdapter1> p_adapter;
    while (DXGI_ERROR_NOT_FOUND != pDxgiFactory_->EnumAdapters1(adapterIndex, &p_adapter))
    {
        DXGI_ADAPTER_DESC1 desc1{};
        p_adapter->GetDesc1(&desc1);
        ++adapterIndex;
        if (desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        // can use D3D12?
        auto hr = D3D12CreateDevice(
            p_adapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            //D3D_FEATURE_LEVEL_12_1,
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
        , D3D_FEATURE_LEVEL_11_0
        , IID_PPV_ARGS(&pDevice_));
    if (FAILED(hr))
    {
        throw new std::runtime_error("D3D12CreateDevice failed.");
    }
}

/// <summary>
/// Create command que
/// </summary>
void D3D12AppParam::create_command_que(HWND hwnd)
{
    D3D12_COMMAND_QUEUE_DESC queueDesc{
      D3D12_COMMAND_LIST_TYPE_DIRECT,       // D3D12_COMMAND_LIST_TYPE: 
      D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,  // priority: no priority
      D3D12_COMMAND_QUEUE_FLAG_NONE,        // Flags: no timeout
      0                                     // NodeMask: One adapter, so 0 is fine.
    };
    // create command que
    auto hr = pDevice_->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCommandQueue_));
    if (FAILED(hr))
    {
        throw std::runtime_error("CreateCommandQueue failed.");
    }
}

/// <summary>
/// Create swap chain
/// </summary>
void D3D12AppParam::create_swap_chain(HWND hwnd)
{
    // fix client drawing area size from HWND
    RECT rect;
    GetClientRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;


    DXGI_SWAP_CHAIN_DESC1 scDesc{};
    scDesc.BufferCount = FrameBufferCount;
    scDesc.Width = width;
    scDesc.Height = height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapchain;
    auto hr = pDxgiFactory_->CreateSwapChainForHwnd(
        pCommandQueue_.Get(),
        hwnd,
        &scDesc,
        nullptr,
        nullptr,
        &swapchain);
    if (FAILED(hr))
    {
        throw std::runtime_error("CreateSwapChainForHwnd failed.");
    }

    swapchain.As(&pSwapchain_); // IDXGISwapChain4 取得
}

/// <summary>
/// Create Descriptor Heaps
/// </summary>
void D3D12AppParam::create_descriptor_heaps(HWND hwnd)
{
    // RTV(Render Target View) Descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{
      D3D12_DESCRIPTOR_HEAP_TYPE_RTV,       // RTV coz render target view
      FrameBufferCount,
      D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
      0
    };
    auto hr = pDevice_->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&pHeapRtv_));
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed CreateDescriptorHeap(RTV)");
    }
    rtvDescriptorSize_ = pDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // DSV(Depth Stencil View) Descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{
      D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
      1,
      D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
      0
    };
    hr = pDevice_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&pHeapDsv_));
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed CreateDescriptorHeap(DSv)");
    }
}

// ===========================================
// End of file