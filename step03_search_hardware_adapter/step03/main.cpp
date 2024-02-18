// step03:
//  Search hardware adapter
//   Written by Tunjay Akbarli
// ===========================================
// include
#include <Windows.h>
#include <wrl.h>            // Microsoft::WRL::ComPtr
#include <stdexcept>

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")

// definition
const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

/// <summary>
/// Make Window procedure
/// </summary>
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_DESTROY) {  // call when discard window
        PostQuitMessage(0);   // tell OS app finessing
        return 0;
    }
    return DefWindowProc(hWnd, msg, wparam, lparam);
}

/// <summary>
/// init Direct3D device
/// </summary>
void Init_direct3d_device(HWND hwnd)
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

    // ---------------------------------------------------
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

    // ---------------------------------------------------
    // init Direct3D device
    ComPtr<ID3D12Device> dev_ = nullptr;
    auto hr = D3D12CreateDevice(
        nullptr
        , D3D_FEATURE_LEVEL_12_1
        , IID_PPV_ARGS(&dev_));
    if (FAILED(hr))
    {
        throw new std::runtime_error("D3D12CreateDevice failed.");
    }
}

/// <summary>
/// WinMain
/// </summary>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // make/register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = (WNDPROC)WndProc;     // setting callback
    wc.lpszClassName = L"DirectXTest";     // application class name
    wc.hInstance = hInstance;
    RegisterClassEx(&wc);                  // register application class

    DWORD dwStyle = WS_OVERLAPPEDWINDOW;
    RECT rect = { 0,0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&rect, dwStyle, false);  // set window size

    // make windows object
    auto hwnd = CreateWindow(wc.lpszClassName, // set class name
        L"DX12 Test",                          // title bar character
        dwStyle,                               // title bar border window
        CW_USEDEFAULT,                         // display x pos
        CW_USEDEFAULT,                         // display y pos
        rect.right - rect.left,                // window w
        rect.bottom - rect.top,                // windows h
        nullptr,                  // parent window handle
        nullptr,                  // menu handle
        wc.hInstance,             // call application handle
        nullptr                   // add parameter
    );

    try
    {
        ShowWindow(hwnd, nCmdShow);    // show window

        // init Direct3D device
        Init_direct3d_device(hwnd);

        // message loop
        MSG msg{};
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        // remove
        UnregisterClass(wc.lpszClassName, wc.hInstance);

        return static_cast<int>(msg.wParam);
    }
    catch (std::runtime_error e) {
        DebugBreak();
        OutputDebugStringA(e.what());
        OutputDebugStringA("\n");
    }

    return 0;
}

// ===========================================
// End of file