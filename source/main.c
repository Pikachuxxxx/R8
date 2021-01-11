#include <r8.h>

#include <Windows.h>
#include <stdio.h>

LPCWSTR g_szClassName = L"test window";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int main()
{

    const R8uint screenWidth = 1280;
    const R8uint screenHeight = 768;
    MSG Msg;

    //Step 1: Registering the Window Class
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(WNDCLASS));
    {
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpfnWndProc = (WNDPROC)WndProc;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = g_szClassName;
    }
    if (!RegisterClass(&wc))
    {
        puts("registering window class failed");
        return 1;
    }

    // Step 2: Creating the Window
     // Create window
    int desktopWidth = GetSystemMetrics(SM_CXSCREEN);
    int desktopHeight = GetSystemMetrics(SM_CYSCREEN);

    DWORD winStyle = WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;

    RECT winRect;
    {
        winRect.left = desktopWidth / 2 - screenWidth / 2;
        winRect.top = desktopHeight / 2 - screenHeight / 2;
        winRect.right = desktopWidth / 2 + screenWidth / 2;
        winRect.bottom = desktopHeight / 2 + screenHeight / 2;
    }

    AdjustWindowRect(&winRect, winStyle, FALSE);

    HWND wnd = CreateWindow(
        g_szClassName,
        L"pico_renderer test (Win32)",
        winStyle, 
        winRect.left,
        winRect.top,
        winRect.right - winRect.left,
        winRect.bottom - winRect.top,
        HWND_DESKTOP,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    ShowWindow(wnd, SW_SHOW);
    UpdateWindow(wnd);
    
    /**/
    if (!r8Ignite())
        return 1;
    
    R8ContextDesc contextDesc;
    
    contextDesc.window = (void*)(&wnd);
    
    // Create render context
    R8object context = r8CreateContext(&contextDesc, screenWidth, screenHeight);
    //r8MakeContextCurrent(context);

    // Create frame buffer
    R8object frameBuffer = r8CreateFrameBuffer(screenWidth, screenHeight);
    r8BindFrameBuffer(frameBuffer);
    
    r8Viewport(0, 0, screenWidth, screenHeight);
    
    // Setup projection matrix
    R8float projection[16];
    r8Mat4Perspective(
        projection,                             // output matrix
        (R8float)screenWidth / screenHeight,    // aspect ratio
        1.0f,                                   // near clipping plane
        100.0f,                                 // far clipping plane
        74.0f * R8_DEG2RAD                      // field of view (FOV) in radians (74 degrees to radians)
    );
    r8ProjectionMatrix(projection);
    
    // World transform
    R8float worldMatrix[16];
    R8float rotation = 0.0f;
    
    // Main loop
    R8bool isQuit = R8_FALSE;
    
    while (GetMessage(&Msg, NULL, 0, 0) > 0 && !isQuit)
    {
        // Update user input here
    
        // Setup world matrix
        r8Identity(worldMatrix);
        r8Translate(worldMatrix, 0, 0, 2);
        r8Rotate(worldMatrix, 0, 0, 1, rotation);
        r8ModelMatrix(worldMatrix);
        rotation += 0.01f;
    
        // Clear scene 
        r8ClearColor(0, 1.0f, 0);
        r8ClearFrameBuffer(
            frameBuffer,
            0.0f,
            R8_COLOR_BUFFER_BIT | R8_DEPTH_BUFFER_BIT
        );
    
        // Draw yellow triangle
        r8BindColor(255, 255, 0);
        r8Begin(R8_TRIANGLES);
        {
            r8Vertex2f(0, 1.155f);
            r8Vertex2f(1, -0.577f);
            r8Vertex2f(-1, -0.577f);
        }
        r8End();
    
        // Show frame buffer on render context
        r8RenderPass(context);

        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    
    // Release all objects
    r8DeleteFrameBuffer(frameBuffer);
    r8DeleteContext(context);
    
    r8Release();
    /**/

    UnregisterClass(g_szClassName, GetModuleHandle(NULL));


    return 0;
}