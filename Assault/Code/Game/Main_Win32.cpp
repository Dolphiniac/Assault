#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/TheKeyboard.hpp"
#include "Engine/Core/EngineSystemManager.hpp"
#include "Engine/Renderer/GLRenderer.hpp"
#include "Engine/Core/Audio.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include <time.h>


//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const int WINDOW_PHYSICAL_WIDTH = 1600;
const int WINDOW_PHYSICAL_HEIGHT = 900;
const double VIEW_LEFT = 0.0;
const double VIEW_RIGHT = 1600.0;
const double VIEW_BOTTOM = 0.0;
const double VIEW_TOP = VIEW_RIGHT * static_cast<double>(WINDOW_PHYSICAL_HEIGHT) / static_cast<double>(WINDOW_PHYSICAL_WIDTH);

bool g_isQuitting = false;
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "Assault";


//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	unsigned char asKey = (unsigned char)wParam;
	switch (wmMessageCode)
	{
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		g_theGame->m_isQuitting = true;
		return 0;

	case WM_KEYDOWN:
		The.Input->SetKeyStatus(asKey, true);
		break;

	case WM_KEYUP:
		The.Input->SetKeyStatus(asKey, false);
		break;
	}

	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}


//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow(HINSTANCE applicationInstanceHandle)
{
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);

	RECT windowRect = { OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_WIDTH, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_HEIGHT };
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL);

	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);

	g_displayDeviceContext = GetDC(g_hWnd);

	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
	pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	int pixelFormatCode = ChoosePixelFormat(g_displayDeviceContext, &pixelFormatDescriptor);
	SetPixelFormat(g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor);
	g_openGLRenderingContext = wglCreateContext(g_displayDeviceContext);
	wglMakeCurrent(g_displayDeviceContext, g_openGLRenderingContext);
}


//-----------------------------------------------------------------------------------------------
void RunMessagePump()
{
	MSG queuedMessage;
	for (;;)
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage);
	}
}


//-----------------------------------------------------------------------------------------------
void Update()
{
	static double s_timeLastFrameBegan = GetCurrentTimeSeconds(); // "static" local variables are initialized once, when first encountered, thus this function call only happens once
	double timeThisFrameBegan = GetCurrentTimeSeconds();
	float deltaSeconds = static_cast<float>(timeThisFrameBegan - s_timeLastFrameBegan);
	s_timeLastFrameBegan = timeThisFrameBegan;

	static int frameNumber = 0;
	++frameNumber;

	g_theGame->Update(deltaSeconds);
	The.Input->Tick();
	The.Audio->Tick(deltaSeconds);

	//	theApp->Update();
}


// void RenderPolygon(Vector2 center, float radius, float numSides, float degreeOffset)
// {
// 	const float completeRads = 2.f * pi;
// 	float iterations = completeRads / numSides;
// 	float radOffset = degreeOffset * (pi / 180.f);
// 	glBegin(GL_LINE_LOOP);
// 	{
// 		for (float currentRad = 0.f; currentRad < completeRads; currentRad += iterations)
// 		{
// 			Vector2 vertex;
// 			vertex.SetXY(center.x + (radius * cos(currentRad + radOffset)), center.y + (radius * sin(currentRad + radOffset)));
// 			glVertex2f(vertex.x, vertex.y);
// 		}
// 	}
// 	glEnd();
// }

/*float g_counter = 0.f;*/
//-----------------------------------------------------------------------------------------------
void Render()
{
	g_theGame->Render();
	// Some simple example OpenGL code; remove this and move all rendering inside theApp and child classes
// 	glClearColor(0.5f, 0.f, 0.f, 1.f);
// 	glClear(GL_COLOR_BUFFER_BIT);
// 
// 	Vector2 center (500.f, 500.f);
// 
// 	g_counter += .0004f;
// 
// 	RenderPolygon(center, 100.f, 30.f * abs(sin(g_counter)), 90.f);
// 	// Draw a line from the bottom-left corner of the screen (0,0) to the center of the screen (800,450)
// // 	glBegin(GL_LINES);
// // 	glVertex2f(0.f, 0.f);
// // 	glVertex2f(800.f, 450.f);
// // 	glEnd();

	SwapBuffers(g_displayDeviceContext);
}


//-----------------------------------------------------------------------------------------------
void RunFrame()
{
	RunMessagePump();
	Update();
	Render();
}


//-----------------------------------------------------------------------------------------------
void Initialize(HINSTANCE applicationInstanceHandle)
{
	CreateOpenGLWindow(applicationInstanceHandle);
	//The.Audio = new AudioSystem();
	g_theGame = new TheGame();
	//The.Renderer = new GLRenderer();
	//The.Input = new TheInput();
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	//delete g_theGame;
	//delete The.Renderer;
	//delete The.Input;
	//delete The.Audio;
	//g_theGame = nullptr;
	//The.Renderer = nullptr;
	//The.Input = nullptr;
	//The.Audio = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	srand((unsigned int)time(NULL));
	UNUSED(commandLineString);
	Initialize(applicationInstanceHandle);

	while (!g_theGame->m_isQuitting)
	{
		RunFrame();
	}

	Shutdown();
	return 0;
}
