/*
 * main.c (Windows)
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */
#include <Windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <r8.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <r8_image.h>
#include <r8_matrix4.h>


// --- global members --- //

R8object context        = NULL;
R8object frameBuffer    = NULL;
R8boolean isQuit        = R8_FALSE;

int mouseX = 0, mouseY = 0;
int mouseSpeedX = 0, mouseSpeedY = 0;
int mouseWheel = 0;
bool buttonDown[2] = { 0 };

bool keyDown[256] = { 0 };

R8enum polyMode = R8_POLYGON_FILL;

float pitch = 0.0f, yaw = 0.0f;
float posZ = 3.0f;

#define MOVE_CAMERA
#ifdef MOVE_CAMERA

#define MOVE_SPEED 0.02f

float camX = 0.0f, camY = 0.0f, camZ = -20.0f;

static void MoveCam(float x, float z)
{
    float matrix[16];
    r8LoadIdentity(matrix);
    r8Rotate(matrix, 0.0f, 1.0f, 0.0f, -yaw);
    r8Rotate(matrix, 1.0f, 0.0f, 0.0f, -pitch);

    float out[3];
    float in[3] = { x, 0.0f, z };
    r8_matrix_mul_float3(out, (R8Matrix4*)matrix, in);

    float speed = MOVE_SPEED;
    if (keyDown[VK_SHIFT])
        speed *= 5.0f;

    camX += out[0] * speed;
    camY += out[1] * speed;
    camZ += out[2] * speed;
}

#endif

#define PI 3.141592654f


// --- functions --- //

LRESULT CALLBACK window_callback(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
                isQuit = R8_TRUE;
            keyDown[wParam] = true;
        }
        break;

        case WM_KEYUP:
        {
            switch (wParam)
            {
                case VK_TAB:
                {
                    if (polyMode == R8_POLYGON_FILL)
                        polyMode = R8_POLYGON_LINE;
                    else
                        polyMode = R8_POLYGON_FILL;
                }
                break;

                case 'M':
                {
                    r8SetState(R8_MIP_MAPPING, !r8GetState(R8_MIP_MAPPING));
                }
                break;
            }
            keyDown[wParam] = false;
        }
        break;

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            mouseSpeedX = x - mouseX;
            mouseSpeedY = y - mouseY;

            mouseX = x;
            mouseY = y;
        }
        break;

        case WM_LBUTTONDOWN:
            buttonDown[0] = true;
            SetCapture(wnd);
            break;
        case WM_LBUTTONUP:
            buttonDown[0] = false;
            ReleaseCapture();
            break;

        case WM_RBUTTONDOWN:
            buttonDown[1] = true;
            SetCapture(wnd);
            break;
        case WM_RBUTTONUP:
            buttonDown[1] = false;
            ReleaseCapture();
            break;

        case WM_MOUSEWHEEL:
        {
            mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        }
        return 0;

        case WM_CLOSE:
        {
            isQuit = R8_TRUE;
        }
        break;
    }
    return DefWindowProc(wnd, message, wParam, lParam);
}

void ErrorCallback(R8enum errorID, const char* info)
{
    printf("R8 Error (%i): %s\n", errorID, info);
}

int main()
{
    // Register window class
    static const char* wcName = L"__r8_test_class__";

    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(WNDCLASS));
    {
        wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
        wc.hInstance        = GetModuleHandle(NULL);
        wc.lpfnWndProc      = (WNDPROC)window_callback;
        wc.hIcon            = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor          = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.cbClsExtra       = 0;
        wc.cbWndExtra       = 0;
        wc.lpszMenuName     = NULL;
        wc.lpszClassName    = wcName;
    }
    if (!RegisterClass(&wc))
    {
        puts("registering window class failed");
        return 1;
    }

    // Create window
    int desktopWidth    = GetSystemMetrics(SM_CXSCREEN);
    int desktopHeight   = GetSystemMetrics(SM_CYSCREEN);

    #define SCREEN_MODE 1
    #if SCREEN_MODE == 1
    const R8uint screenWidth  = 640;//800;
    const R8uint screenHeight = 480;//600;
    const R8boolean isFullscreen = R8_FALSE;
    #elif SCREEN_MODE == 2
    const R8uint screenWidth  = 1280;
    const R8uint screenHeight = 768;
    const R8boolean isFullscreen = R8_TRUE;
    #elif SCREEN_MODE == 3
    const R8uint screenWidth  = 1600;
    const R8uint screenHeight = 900;
    const R8boolean isFullscreen = R8_FALSE;
    #else
    const R8uint screenWidth  = 1920;
    const R8uint screenHeight = 1080;
    const R8boolean isFullscreen = R8_TRUE;
    #endif

    DWORD winStyle = WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;

    RECT winRect;
    {
        winRect.left    = desktopWidth/2 - screenWidth/2;
        winRect.top     = desktopHeight/2 - screenHeight/2;
        winRect.right   = desktopWidth/2 + screenWidth/2;
        winRect.bottom  = desktopHeight/2 + screenHeight/2;
    }

    if (isFullscreen)
    {
        winStyle        = WS_POPUP;

        winRect.left    = 0;
        winRect.top     = 0;
        winRect.right   = screenWidth;
        winRect.bottom  = screenHeight;

        DEVMODE devMode;
        {
            memset(&devMode, 0, sizeof(devMode));
            devMode.dmSize          = sizeof(devMode);
            devMode.dmPelsWidth     = screenWidth;
            devMode.dmPelsHeight    = screenHeight;
            devMode.dmBitsPerPel    = 32;
            devMode.dmFields        = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        }
        ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
    }

    AdjustWindowRect(&winRect, winStyle, FALSE);

    HWND wnd = CreateWindow(
        wcName,
        L"R8_renderer test (Win32)",
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

    // Initialize renderer
    r8Init();
    r8ErrorHandler(ErrorCallback);

    printf("%s %s\n", r8GetString(R8_STRING_RENDERER), r8GetString(R8_STRING_VERSION));

    // Create render context
    R8contextdesc contextDesc;

    contextDesc.window = (void*)(&wnd);

    context = r8CreateContext(&contextDesc, screenWidth, screenHeight);

    // Create frame buffer
    frameBuffer = r8CreateFrameBuffer(screenWidth, screenHeight);
    r8BindFrameBuffer(frameBuffer);

    // Create textures
    #ifdef R8_COLOR_BUFFER_24BIT
    const R8boolean dither = R8_FALSE;
    #else
    const R8boolean dither = R8_TRUE;
    #endif

    R8object textureA = r8CreateTexture();
    r8TexImage2DFromFile(textureA, "media/house.jpg", dither, R8_TRUE);

    R8object textureB = r8CreateTexture();
    r8TexImage2DFromFile(textureB, "media/tiles.png", dither, R8_TRUE);

    //r8TexEnvi(R8_TEXTURE_LOD_BIAS, 1);

    // Create vertex buffer
    R8object vertexBuffer = r8CreateVertexBuffer();
    R8object indexBuffer = r8CreateIndexBuffer();

    #if 1

    #define NUM_VERTICES 24

    sR8_vertex cubeVertices[NUM_VERTICES] =
    {
        //  x      y      z     u     v
        // front
        { -1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f, -1.0f, 1.0f, 0.0f },
        {  1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f, 0.0f, 1.0f },

        // back
        {  1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        { -1.0f,  1.0f,  1.0f, 1.0f, 0.0f },
        { -1.0f, -1.0f,  1.0f, 1.0f, 1.0f },
        {  1.0f, -1.0f,  1.0f, 0.0f, 1.0f },

        // left
        { -1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        { -1.0f,  1.0f, -1.0f, 1.0f, 0.0f },
        { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f,  1.0f, 0.0f, 1.0f },

        // right
        {  1.0f,  1.0f, -1.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f,  1.0f, 1.0f, 0.0f },
        {  1.0f, -1.0f,  1.0f, 1.0f, 1.0f },
        {  1.0f, -1.0f, -1.0f, 0.0f, 1.0f },

        // top
        { -1.0f,  1.0f,  1.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f,  1.0f, 1.0f, 0.0f },
        {  1.0f,  1.0f, -1.0f, 1.0f, 1.0f },
        { -1.0f,  1.0f, -1.0f, 0.0f, 1.0f },

        // bottom
        { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f },
        {  1.0f, -1.0f, -1.0f, 1.0f, 0.0f },
        {  1.0f, -1.0f,  1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f,  1.0f, 0.0f, 1.0f }
    };
    r8VertexBufferData(vertexBuffer, NUM_VERTICES, &(cubeVertices[0].x), &(cubeVertices[0].u), sizeof(sR8_vertex));

    #define NUM_INDICES 36

    R8ushort cubeIndices[NUM_INDICES] =
    {
         0, 1, 2,    0, 2, 3, // front
         4, 5, 6,    4, 6, 7, // back
         8, 9,10,    8,10,11, // left
        12,13,14,   12,14,15, // right
        16,17,18,   16,18,19, // top
        20,21,22,   20,22,23, // bottom
    };
    r8IndexBufferData(indexBuffer, cubeIndices, NUM_INDICES);

    //float size[3] = { 2.0f, 2.0f, 0.5f };
    float size[3] = { 1.0f, 1.0f, 1.0f };

    #else

    #define NUM_VERTICES numVertices
    #define NUM_INDICES numIndices

    R8sizei numVertices = 0, numIndices = 0;

    FILE* mdlFile = fopen("media/house_tris.r8", "rb");
    if (mdlFile)
    {
        r8VertexBufferDataFromFile(vertexBuffer, &numVertices, mdlFile);
        r8IndexBufferDataFromFile(indexBuffer, &numIndices, mdlFile);
        fclose(mdlFile);
    }
    else
        puts("could not open model file!");

    float size[3] = { 5, -5, 5 };//0.7f, -0.7f, 0.7f };

    #endif

    // Setup matrices
    float r8ojectionA[16], r8ojectionB[16], worldMatrix[16], viewMatrix[16];

    #if 0
    R8uint viewWidth = 200;
    R8uint viewHeight = 200;
    #else
    R8uint viewWidth = screenWidth;
    R8uint viewHeight = screenHeight;
    #endif

    float orthoSize = 0.007f;//0.02f;
    r8BuildPerspectiveProjection(r8ojectionA, (float)viewWidth/viewHeight, 0.01f, 100.0f, 74.0f * R8_DEG2RAD);
    r8BuildOrthogonalProjection(r8ojectionB, orthoSize*viewWidth, orthoSize*viewHeight, 0.1f, 100.0f);

    r8LoadIdentity(viewMatrix);
    r8ViewMatrix(viewMatrix);

    //r8CullMode(R8_CULL_BACK);
    r8CullMode(R8_CULL_FRONT);

    clock_t startTime = clock();
    int fps = 0;

    r8Enable(R8_MIP_MAPPING);

    // Main loop
    while (!isQuit)
    {
        // Reset input states
        mouseSpeedX = 0;
        mouseSpeedY = 0;
        mouseWheel = 0;

        // Window event handling
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        #if 0
        // Show FPS
        ++fps;
        if (clock()/CLOCKS_PER_SEC > startTime/CLOCKS_PER_SEC)
        {
            startTime = clock();
            printff("fps = %i\n", fps);
            fps = 0;
        }
        #endif

        // Update user input
        if (buttonDown[0])
        {
            yaw     -= PI*0.0025f*(float)mouseSpeedX;
            pitch   += PI*0.0025f*(float)mouseSpeedY;
        }

        posZ += 0.1f * mouseWheel;

        #ifdef MOVE_CAMERA

        if (pitch < -PI*0.5f)
            pitch = -PI*0.5f;
        if (pitch > PI*0.5f)
            pitch = PI*0.5f;

        r8LoadIdentity(viewMatrix);
        r8Rotate(viewMatrix, 1.0f, 0.0f, 0.0f, pitch);
        r8Rotate(viewMatrix, 0.0f, 1.0f, 0.0f, yaw);
        r8Translate(viewMatrix, -camX, -camY, -camZ);
        r8ViewMatrix(viewMatrix);

        if (keyDown['W'])
            MoveCam(0, 1);
        if (keyDown['S'])
            MoveCam(0, -1);
        if (keyDown['A'])
            MoveCam(-1, 0);
        if (keyDown['D'])
            MoveCam(1, 0);

        #endif

        // Drawing
        r8ClearColor(255, 255, 255);
        r8ClearFrameBuffer(frameBuffer, 0.0f, R8_COLOR_BUFFER_BIT | R8_DEPTH_BUFFER_BIT);
        {
            #if 0

            r8DrawScreenLine(300, 200, 500, 200, r8GetColorIndex(255, 0, 0));
            r8DrawScreenLine(500, 200, 500, 400, r8GetColorIndex(0, 255, 0));
            r8DrawScreenLine(500, 400, 300, 400, r8GetColorIndex(0, 0, 255));
            r8DrawScreenLine(300, 400, 300, 200, r8GetColorIndex(0, 255, 255));

            r8DrawScreenLine(10, 10, mouseX, mouseY, r8GetColorIndex(255, 255, 0));

            #elif 0

            for (int y = 0; y < 256; ++y)
                r8DrawScreenLine(100, 100 + y, 356, 100 + y, r8GetColorIndex(y, y, y));

            #elif 0

            r8Color(r8GetColorIndex(255, 0, 0));
            r8BindTexture(textureA);

            r8DrawScreenImage(100, 100, mouseX, mouseY);

            #elif 0

            // Bind buffers
            r8BindVertexBuffer(vertexBuffer);
            r8BindIndexBuffer(indexBuffer);

            // Setup view
            r8Viewport(0, 0, viewWidth, viewHeight);
            r8Color(r8GetColorIndex(0, 0, 255));

            // Setup transformation
            r8ProjectionMatrix(r8ojectionB);
            r8LoadIdentity(worldMatrix);
            #ifndef viewMatrix
            r8Translate(worldMatrix, 0.0f, 0.0f, posZ);
            r8Rotate(worldMatrix, 1.0f, 0.0f, 0.0f, pitch);
            r8Rotate(worldMatrix, 0.0f, 1.0f, 0.0f, -yaw);
            #endif
            r8Scale(worldMatrix, size[0], size[1], size[2]);
            r8WorldMatrix(worldMatrix);

            // Draw lines
            r8DrawIndexed(R8_LINES, NUM_INDICES, 0);

            // Setup view
            r8Viewport(viewWidth, 0, 600, 600);
            r8Color(r8GetColorIndex(255, 0, 0));

            // Setup transformation
            r8ProjectionMatrix(r8ojectionA);
            r8LoadIdentity(worldMatrix);
            r8Translate(worldMatrix, 0.0f, 0.0f, posZ);
            r8Rotate(worldMatrix, 1.0f, 0.0f, 0.0f, pitch);
            r8Rotate(worldMatrix, 0.0f, 1.0f, 0.0f, yaw);
            r8Scale(worldMatrix, size[0], size[1], size[2]);
            r8WorldMatrix(worldMatrix);

            // Draw lines
            r8DrawIndexed(R8_LINES, NUM_INDICES, 0);

            #elif 1

            r8Color(0, 0, 255);

            // Bind buffers
            r8BindVertexBuffer(vertexBuffer);
            r8BindIndexBuffer(indexBuffer);
            r8BindTexture(textureA);

            // Setup view
            r8Viewport(0, 0, viewWidth, viewHeight);

            //r8Enable(R8_SCISSOR);
            //r8Scissor(20, 20, viewWidth - 100, viewHeight - 100);

            r8PolygonMode(polyMode);

            // Setup transformation
            r8ProjectionMatrix(r8ojectionA);
            r8LoadIdentity(worldMatrix);
            #ifndef MOVE_CAMERA
            r8Translate(worldMatrix, 0.0f, 0.0f, posZ);
            r8Rotate(worldMatrix, 1.0f, 0.0f, 0.0f, pitch);
            r8Rotate(worldMatrix, 0.0f, 1.0f, 0.0f, yaw);
            #endif
            r8Scale(worldMatrix, size[0], size[1], size[2]);
            r8WorldMatrix(worldMatrix);

            // Draw triangles
            //r8DepthRange(0.0f, 0.5f);
            r8DrawIndexed(R8_TRIANGLES, NUM_INDICES, 0);

            #if 1
            for (int i = 1; i < 9; ++i)
            {
                worldMatrix[12] = ((float)(i % 3))*5*size[0];
                worldMatrix[14] = ((float)(i / 3))*5*size[2];
                r8WorldMatrix(worldMatrix);
                r8DrawIndexed(R8_TRIANGLES, NUM_INDICES, 0);
            }
            #endif

            // Draw floor
            float floorSize = 100.0f;
            int numQuads = 50;

            r8LoadIdentity(worldMatrix);
            r8Translate(worldMatrix, 0, 4, 0);
            r8Scale(worldMatrix, floorSize, floorSize, floorSize);
            r8WorldMatrix(worldMatrix);

            r8BindTexture(textureB);

            r8Begin(R8_TRIANGLES);
            {
                #if defined(R8_PERSPECTIVE_CORRECTED) && 0

                // Only use a single quad when textures are perspective corrected
                r8TexCoord2i(0, 0); r8Vertex3f(-1, 0, 1);
                r8TexCoord2i(numQuads, 0); r8Vertex3f(1, 0, 1);
                r8TexCoord2i(numQuads, numQuads); r8Vertex3f(1, 0, -1);

                r8TexCoord2i(0, 0); r8Vertex3f(-1, 0, 1);
                r8TexCoord2i(numQuads, numQuads); r8Vertex3f(1, 0, -1);
                r8TexCoord2i(0, numQuads); r8Vertex3f(-1, 0, -1);

                #else

                // Use many quads to emulate perspective texture correction
                float step = 2.0f / (float)numQuads;

                for (float x = -1.0f; x < 1.0f; x += step)
                {
                    for (float z = -1.0f; z < 1.0f; z += step)
                    {
                        r8TexCoord2i(0, 0); r8Vertex3f(x, 0, z + step);
                        r8TexCoord2i(1, 0); r8Vertex3f(x + step, 0, z + step);
                        r8TexCoord2i(1, 1); r8Vertex3f(x + step, 0, z);

                        r8TexCoord2i(0, 0); r8Vertex3f(x, 0, z + step);
                        r8TexCoord2i(1, 1); r8Vertex3f(x + step, 0, z);
                        r8TexCoord2i(0, 1); r8Vertex3f(x, 0, z);
                    }
                }

                #endif
            }
            r8End();

            #   if 0

            // Draw with immediate mode
            r8BindTexture(0);
            r8Color(r8GetColorIndex(255, 0, 0));

            static float angle;

            //angle += 0.02f;
            if (buttonDown[1])
                angle += 0.02f * mouseSpeedX;

            r8LoadIdentity(worldMatrix);
            r8ProjectionMatrix(worldMatrix);
            #ifndef MOVE_CAMERA
            r8Translate(worldMatrix, 0, 0, 1.5f);
            r8Rotate(worldMatrix, 0, 0, 1, angle);
            #endif
            r8Scale(worldMatrix, 0.7f, 0.7f, 0.7f);
            r8WorldMatrix(worldMatrix);

            r8CullMode(R8_CULL_NONE);
            r8PolygonMode(R8_POLYGON_FILL);
            r8Disable(R8_SCISSOR);

            r8Viewport(0, 0, 100, 100);

            r8Begin(R8_TRIANGLES);
            {
                r8Vertex2f(0, 1.155f);
                r8Vertex2f(1, -0.577f);
                r8Vertex2f(-1, -0.577f);
            }
            r8End();

            #   endif

            #   if 0
            // Setup transformation
            r8LoadIdentity(worldMatrix);
            #ifndef MOVE_CAMERA
            r8Translate(worldMatrix, 1.5f, 0.0f, posZ);
            r8Rotate(worldMatrix, 1.0f, 1.0f, 1.0f, pitch);
            #endif
            r8WorldMatrix(worldMatrix);

            r8BindTexture(textureB);

            // Draw triangles
            //r8DepthRange(0.5f, 1.0f);
            r8DrawIndexed(R8_TRIANGLES, NUM_INDICES, 0);
            #   endif

            #endif
        }
        r8Present(context);
    }

    // Clean up
    r8DeleteTexture(textureA);
    r8DeleteTexture(textureB);
    r8DeleteVertexBuffer(vertexBuffer);
    r8DeleteIndexBuffer(indexBuffer);
    r8DeleteFrameBuffer(frameBuffer);
    r8DeleteContext(context);

    r8Release();

    DestroyWindow(wnd);

    UnregisterClass(wcName, GetModuleHandle(NULL));

    return 0;
}
