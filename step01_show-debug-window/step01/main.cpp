// step01:
// Show blank window
//   Written by Tunjay Akbarli
// ===========================================
// include
#include <Windows.h>
#include <stdexcept>

// definition
const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

/// <summary>
/// Make Window procedure
/// </summary>
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_DESTROY) {          // call when discard window
        PostQuitMessage(0);   // tell OS app finessing
        return 0;
    }
    return DefWindowProc(hWnd, msg, wparam, lparam);
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

        MSG msg{};
        while (msg.message != WM_QUIT){
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