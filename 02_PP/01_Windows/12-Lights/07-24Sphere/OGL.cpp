// Windows Header Files
#include <windows.h> //sdk win32api
#include <stdio.h>	 //file io
#include <stdlib.h>	 //exit()
#include <iostream>

// OpenGL Header File
#include <GL/glew.h> //this must be before <GL/gl.h>
#include <GL/gl.h>

#include "vmath.h"
using namespace vmath;

#include "OGL.h"
#include "Sphere.h"

// macors
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// link with opengl library
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Sphere.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations

// for fileIO
FILE *gpFILE = NULL;

// for fullscreen
HWND ghwnd = NULL;
DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
BOOL gbFullscreen = FALSE;

// for active/inactive
BOOL gbActiveWindow = FALSE;

// OpenGL related global variables
HDC ghdc = NULL;
HGLRC ghrc = NULL;

GLuint pervertex_shaderProgramObject = 0;
GLuint perfragment_shaderProgramObject = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCOORD,
	AMC_ATTRIBUTE_NORMAL
};

GLuint vao_sphere = 0;
GLuint vbo_sphere_position = 0;
GLuint vbo_sphere_normal = 0;
GLuint vbo_sphere_element = 0;
GLuint vbo_sphere_texcoord = 0;
// GLuint vbo_color_sphere = 0;

// GLuint mvpMatrixUniform = 0;
GLuint modelViewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;
GLuint modelMatrixUniform, uMatrixUniform;

//light
GLuint lightAmbientUniform = 0;
GLuint lightDefuseUniform = 0;
GLuint lightSpecularUniform = 0;
GLuint lightPositionUniform = 0;

// unifrom location for Material Attributes
GLuint materialAmbientUniform = 0;
GLuint materialDefuseUniform = 0;
GLuint materialSpecularUniform = 0;
GLuint materialShininessUniform = 0;
GLuint keyPressUniform = 0;

BOOL bLightingEnable = FALSE;

/*GLfloat lightDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialDefuse[] = {0.5f, 0.5f, 0.5f, 0.5f};
GLfloat lightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};*/

GLuint gNumElements;
GLuint gNumVertices;

GLfloat lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f}; // grey ambient light
GLfloat lightDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White diffuse light (main Color of light)
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f}; // Positional light

mat4 perspectiveProjetionMatrix;

char choosenShader = 'v';

GLuint keyPress;

GLfloat XRotation = 0.0f;
GLfloat YRotation = 0.0f;
GLfloat ZRotation = 0.0f;

struct MaterialProperties
{
	GLfloat materialAmbient[4], materialDefuse[4], materialSpecular[4];
	GLfloat materialShininess;
};

MaterialProperties material[24];

// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// fucntion declaration
	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	// Local Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT(" ABCWindow ");

	int iResult = 0;

	BOOL bDone = FALSE; // game loop

	// for centering
	int ScreenHeight;
	int ScreenWidth;
	int WindowHeight = 600;
	int WindowWidth = 800;

	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	int X = ScreenWidth / 2 - WindowWidth / 2;
	int Y = ScreenHeight / 2 - WindowHeight / 2;

	// code
	//	gpFILE = fopen("Log.txt", "w");
	//	fopen_s
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
	{
		AllocConsole();
	}
	freopen("CONOUT$", "w", stdout);
	std::cout << "Log On Console" << std::endl;

	if (fopen_s(&gpFILE, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Cannot Be Opend"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}

	fprintf(gpFILE, "Program Started Successfully\n");
	fprintf(gpFILE, "-------------------------------------------\n");

	// WNDCEX initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	// Create Window
	hwnd = CreateWindow(szAppName,
						TEXT("Abhay B. Chilka"),
						WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
						X,
						Y,
						WIN_WIDTH,
						WIN_HEIGHT,
						NULL,
						NULL,
						hInstance,
						NULL);

	ghwnd = hwnd;

	// Initialization
	iResult = initialize();
	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialize() failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);
	}

	// Show The Window
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Paint/Re-Draw The Window
	UpdateWindow(hwnd);

	// Message-loop
	// game-loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				// RENDER
				display();

				// UPDATE
				update();
			}
		}
	}
	uninitialize();

	return ((int)msg.wParam);
}

// CallBack Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Declarations
	void ToggleFullScreen(void);
	void resize(int, int);

	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ERASEBKGND:
		return (0);
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			if (gbFullscreen == FALSE)
			{
				ToggleFullScreen();
				gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullScreen();
				gbFullscreen = FALSE;
			}

			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'f':
		case 'F':
			choosenShader = 'f';
			// Get Shaders Uniform Locations
			modelMatrixUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uModelMatrix");
			modelViewMatrixUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uViewMatrix");
			projectionMatrixUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uProjectionMatrix");

			// Light uniform locations
			lightAmbientUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uLightAmbient");
			lightDefuseUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uLightDefuse");
			lightSpecularUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uLightSpecular");
			lightPositionUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uLightPosition");

			// material uniform locations
			materialAmbientUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uMaterialAmbient");
			materialDefuseUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uMaterialDefuse");
			materialSpecularUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uMaterialSpecular");
			materialShininessUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uMaterialShininess");

			keyPressUniform = glGetUniformLocation(perfragment_shaderProgramObject, "uKeyPress");
			break;

			// Enable Per Vertex Lighitng
		case 'v':
		case 'V':
			// Get Shaders Uniform Locations
			modelMatrixUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uModelMatrix");
			modelViewMatrixUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uViewMatrix");
			projectionMatrixUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uProjectionMatrix");

			// Light uniform locations
			lightAmbientUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uLightAmbient");
			lightDefuseUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uLightDefuse");
			lightSpecularUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uLightSpecular");
			lightPositionUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uLightPosition");

			// material uniform locations
			materialAmbientUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uMaterialAmbient");
			materialDefuseUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uMaterialDefuse");
			materialSpecularUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uMaterialSpecular");
			materialShininessUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uMaterialShininess");

			keyPressUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uKeyPress");

			choosenShader = 'v';
			break;

		case 'L':
		case 'l':
			if (bLightingEnable == FALSE)
			{
				bLightingEnable = TRUE;
			}
			else
			{
				bLightingEnable = FALSE;
			}
			break;

		case 'Q':
		case 'q':
			DestroyWindow(hwnd);
			break;

		case 'x':
		case 'X':
			keyPress = 1;
			XRotation = 0.0f;
			break;

		case 'y':
		case 'Y':
			keyPress = 2;
			YRotation = 0.0f;
			break;

		case 'z':
		case 'Z':
			keyPress = 3;
			ZRotation = 0.0f;
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		if (gpFILE)
		{
			fprintf(gpFILE, "Program Ended Successfully...\n");
			fclose(gpFILE);
			gpFILE = NULL;
		}

		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	// Local Variable Declarations
	MONITORINFO mi = {sizeof(MONITORINFO)};

	// Code
	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW); // ~Remove Contents
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);

		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

int initialize(void)
{
	// function declarations
	void printgGLInfo(void);
	void resize(int, int);
	void uninitialize(void);
	void initMaterialValues();

	// code
	// variable declaration
	PIXELFORMATDESCRIPTOR PFD;
	int iPixelFormatIndex = 0;

	// initializing PFD struct with 0
	ZeroMemory(&PFD, sizeof(PIXELFORMATDESCRIPTOR));

	// initialization of PixelFormatDescriptor
	PFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	PFD.nVersion = 1;
	PFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	PFD.iPixelType = PFD_TYPE_RGBA;
	PFD.cColorBits = 32;
	PFD.cRedBits = 8;
	PFD.cGreenBits = 8;
	PFD.cBlueBits = 8;
	PFD.cAlphaBits = 8;
	PFD.cDepthBits = 32;

	// get dc
	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFILE, "GetDC Failed\n");
		return (-1);
	}

	// choosing pixel format which closlely matches to our initialised PFD
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &PFD);

	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFILE, "Choose PixelFormat function() failed");
		return (-2);
	}

	// set obtain PixelFormat
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &PFD) == FALSE)
	{
		fprintf(gpFILE, "SetPixelFormat\n");
		return (-3);
	}

	// create opengl context from device context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFILE, "wglCreateContext() failed");
		return (-4);
	}

	// make rendering context current
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFILE, "wglMakeCurrent() Fialed\n");
		return (-5);
	}

	// initialise GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFILE, "glewInit() failed\n");
		return (-6);
	}

	// printglinfo
	printgGLInfo();

/////////---------------------------------------------pervertex-----------------------------------------------//////////

	// Vertex Shader
	const GLchar *pervertex_vertexShaderSourceCode =
		"#version 460 core"
		"\n"
		"in vec4 aPosition;"
		"in vec3 aNormal;"
		"uniform mat4 uModelMatrix;"
		"uniform mat4 uViewMatrix;"
		"uniform mat4 uProjectionMatrix;"
		"uniform vec3 uLightDefuse;"
		"uniform vec3 uLightAmbient;"
		"uniform vec3 uLightSpecular;"
		"uniform vec4 uLightPosition;"
		"uniform vec3 uMaterialDefuse;"
		"uniform vec3 uMaterialAmbient;"
		"uniform vec3 uMaterialSpecular;"
		"uniform float uMaterialShininess;"
		"uniform int uKeyPress;"
		"out vec3 oPhong_ADS_Light;"
		"void main(void)"
		"{"
		"	if(uKeyPress == 1)"
		"	{"
		"		vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;"
		"		vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);"
		"		vec3 lightDirection = normalize(vec3(uLightPosition - eyeCoordinates));"
		"		vec3 reflectionVector = reflect(-lightDirection,transformedNormals);"
		"		vec3 viewerVector = normalize(-eyeCoordinates.xyz);"
		"		vec3 ambientLight = uLightAmbient * uMaterialAmbient;"
		"		vec3 defuseLight = uLightDefuse * uMaterialDefuse * max(dot(lightDirection,transformedNormals),0.0);"
		"		vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector,viewerVector),0.0),uMaterialShininess);"
		"		oPhong_ADS_Light = ambientLight + defuseLight + specularLight;"
		"	}"
		"	else"
		"	{"
		"		oPhong_ADS_Light = vec3(0.0f,0.0f,0.0f);"
		"	}"
		"   gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"
		"}";

	GLuint pervertex_vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(pervertex_vertexShaderObject, 1, (const GLchar **)&pervertex_vertexShaderSourceCode, NULL);

	glCompileShader(pervertex_vertexShaderObject);

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar *szinfolog = NULL;

	glGetShaderiv(pervertex_vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(pervertex_vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog = (GLchar *)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetShaderInfoLog(pervertex_vertexShaderObject, GL_INFO_LOG_LENGTH, NULL, szinfolog);
				fprintf(gpFILE, "VertexShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}
		uninitialize();
	}

	// fragment shader
	const GLchar *pervertex_fragmentShaderSourceCode =
										"#version 460 core"
										"\n"
										"in vec3 oPhong_ADS_Light;"
										"uniform int uKeyPress;"
										"out vec4 FragColor;"
										"void main(void)"
										"{"
										"  if(uKeyPress == 1)"
										"	FragColor = vec4(oPhong_ADS_Light,1.0f);"
										"  else"
										"	FragColor = vec4(1.0f,1.0f,1.0f,1.0f);"
										"}";

	GLuint pervertex_fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(pervertex_fragmentShaderObject, 1, (const GLchar **)&pervertex_fragmentShaderSourceCode, NULL);

	glCompileShader(pervertex_fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetShaderiv(pervertex_fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(pervertex_fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog = (GLchar *)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetShaderInfoLog(pervertex_fragmentShaderObject, GL_INFO_LOG_LENGTH, NULL, szinfolog);
				fprintf(gpFILE, "FragmentShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}
		uninitialize();
	}

	// shader program
	pervertex_shaderProgramObject = glCreateProgram();

	glAttachShader(pervertex_shaderProgramObject, pervertex_vertexShaderObject);
	glAttachShader(pervertex_shaderProgramObject, pervertex_fragmentShaderObject);

	// prelinking
	glBindAttribLocation(pervertex_shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(pervertex_shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(pervertex_shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetProgramiv(pervertex_shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(pervertex_shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog = (GLchar *)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetProgramInfoLog(pervertex_shaderProgramObject, infoLogLength, NULL, szinfolog);
				fprintf(gpFILE, "Shader Program linking error log: %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}
		uninitialize();
	}

	modelMatrixUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uModelMatrix");
	modelViewMatrixUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uViewMatrix");
	projectionMatrixUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uProjectionMatrix");

	// Light uniform locations
	lightAmbientUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uLightAmbient");
	lightDefuseUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uLightDefuse");
	lightSpecularUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uLightSpecular");
	lightPositionUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uLightPosition");

	// material uniform locations
	materialAmbientUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uMaterialAmbient");
	materialDefuseUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uMaterialDefuse");
	materialSpecularUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uMaterialSpecular");
	materialShininessUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uMaterialShininess");

	keyPressUniform = glGetUniformLocation(pervertex_shaderProgramObject, "uKeyPress");

//////////---------------------------------------------------------------------------------------------------------------//////////


/////////-----------------------------------------------perfragment------------------------------------------------------//////////

	// Vertex Shader
	const GLchar *perfragment_vertexShaderSourceCode =
		"#version 460 core"
		"\n"
		"in vec4 aPosition;"
		"in vec3 aNormal;"
		"uniform vec4 uLightPosition;"
		"uniform mat4 uModelMatrix;"
		"uniform mat4 uViewMatrix;"
		"uniform mat4 uProjectionMatrix;"
		"uniform int uKeyPress;"
		"out vec3 oTransformedNormals;"
		"out vec3 oLightDirection;"
		"out vec3 oViewerVector;"
		"void main(void)"
		"{"
		"	if(uKeyPress == 1)"
		"	{"
		"		vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;"
		"		oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;"
		"		oLightDirection = vec3(uLightPosition - eyeCoordinates);"
		"		oViewerVector = -eyeCoordinates.xyz;"
		"	}"
		"	else"
		"	{"
		"		oTransformedNormals = vec3(0.0f,0.0f,0.0f);"
		"		oLightDirection = vec3(0.0f,0.0f,0.0f);"
		"		oViewerVector = vec3(0.0f,0.0f,0.0f);"
		"	}"
		"   gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"
		"}";

	GLuint perfragment_vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(perfragment_vertexShaderObject, 1, (const GLchar **)&perfragment_vertexShaderSourceCode, NULL);

	glCompileShader(perfragment_vertexShaderObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetShaderiv(perfragment_vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(perfragment_vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog = (GLchar *)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetShaderInfoLog(perfragment_vertexShaderObject, GL_INFO_LOG_LENGTH, NULL, szinfolog);
				fprintf(gpFILE, "VertexShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}
		uninitialize();
	}

	// fragment shader
	const GLchar *perfragment_fragmentShaderSourceCode =
		"#version 460 core"
		"\n"
		"in vec3 oTransformedNormals;"
		"in vec3 oLightDirection;"
		"in vec3 oViewerVector;"
		"uniform vec3 uLightDefuse;"
		"uniform vec3 uLightAmbient;"
		"uniform vec3 uLightSpecular;"
		"uniform vec3 uMaterialDefuse;"
		"uniform vec3 uMaterialAmbient;"
		"uniform vec3 uMaterialSpecular;"
		"uniform float uMaterialShininess;"
		"uniform int uKeyPress;"
		"out vec4 FragColor;"
		"void main(void)"
		"{"
		"	vec3 phongADSLight;"
		"  if(uKeyPress == 1)"
		"	{"
		"		vec3 normalisedTransformedNormals = normalize(oTransformedNormals);"
		"		vec3 normalisedLightDirection = normalize(oLightDirection);"
		"		vec3 normalisedViewerVector = normalize(oViewerVector);"
		"		vec3 reflectionVector = reflect(-normalisedLightDirection,normalisedTransformedNormals);"
		"		vec3 ambientLight = uLightAmbient * uMaterialAmbient;"
		"		vec3 defuseLight = uLightDefuse * uMaterialDefuse * max(dot(normalisedLightDirection,normalisedTransformedNormals),0.0);"
		"		vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector,normalisedViewerVector),0.0),uMaterialShininess);"
		"		phongADSLight = ambientLight + defuseLight + specularLight;"
		"	}"
		"  else"
		"	{"
		"		phongADSLight = vec3(1.0f,1.0f,1.0f);"
		"	}"
		"  FragColor = vec4(phongADSLight,1.0f);"
		"}";

	GLuint perfragment_fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(perfragment_fragmentShaderObject, 1, (const GLchar **)&perfragment_fragmentShaderSourceCode, NULL);

	glCompileShader(perfragment_fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetShaderiv(perfragment_fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(perfragment_fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog = (GLchar *)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetShaderInfoLog(perfragment_fragmentShaderObject, GL_INFO_LOG_LENGTH, NULL, szinfolog);
				fprintf(gpFILE, "FragmentShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}
		uninitialize();
	}

	// shader program
	perfragment_shaderProgramObject = glCreateProgram();

	glAttachShader(perfragment_shaderProgramObject, perfragment_vertexShaderObject);
	glAttachShader(perfragment_shaderProgramObject, perfragment_fragmentShaderObject);

	// prelinking
	glBindAttribLocation(perfragment_shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(perfragment_shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(perfragment_shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetProgramiv(perfragment_shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(perfragment_shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog = (GLchar *)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetProgramInfoLog(perfragment_shaderProgramObject, infoLogLength, NULL, szinfolog);
				fprintf(gpFILE, "Shader Program linking error log: %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}
			uninitialize();
	}
//////////---------------------------------------------------------------------------------------------------------------//////////

	float sphere_positons[1146];
	float sphere_normals[1146];
	float sphere_texcoords[764];
	unsigned short sphere_elements[2280];

	getSphereVertexData(sphere_positons, sphere_normals, sphere_texcoords, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// vao sphere
	glGenVertexArrays(1, &vao_sphere);
	glBindVertexArray(vao_sphere);

	// position vbo sphere
	glGenBuffers(1, &vbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_positons), sphere_positons, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo sphere
	glGenBuffers(1, &vbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo sphere
	glGenBuffers(1, &vbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao sphere
	glBindVertexArray(0);

	// Enabling Depth
	glClearDepth(1.0f);		 // Compulsory
	glEnable(GL_DEPTH_TEST); // Compulsory
	glDepthFunc(GL_LEQUAL);	 // Compulsory

	// set the clear color of window to blue
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// initialise
	perspectiveProjetionMatrix = vmath::mat4::identity();

	initMaterialValues();

	resize(WIN_WIDTH, WIN_HEIGHT);
	// here opengl starts
	return (0);
}

void printgGLInfo(void)
{
	// variable declaratios
	GLint numExtensions;
	GLint i;

	// code
	fprintf(gpFILE, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFILE, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFILE, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFILE, "OpenGL GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFILE, "-------------------------------------------\n");
	// listing of supported extensions
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

	for (i = 0; i < numExtensions; i++)
	{
		fprintf(gpFILE, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	fprintf(gpFILE, "-------------------------------------------\n");
}

void resize(int width, int height)
{
	// code
	if (height <= 0)
		height = 1;

	// resize
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// set perspective projection matrix
	perspectiveProjetionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (choosenShader == 'v')
		glUseProgram(pervertex_shaderProgramObject);
	else if (choosenShader == 'f')
		glUseProgram(perfragment_shaderProgramObject);

	// Cube
	vmath::mat4 translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

	// View matrix
	vmath::mat4 viewMatrix = vmath::lookat(vmath::vec3(19.5f / 2.0f, 16.0f / 2.0f, 17.0f), vmath::vec3(19.5f / 2.0f, 16.0f / 2.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

	// Model Matrix
	vmath::mat4 modelMatrix = translationMatrix;

	// Push All Uniform values into vertex shader
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjetionMatrix);

	if (bLightingEnable == TRUE)
	{
		if (keyPress == 1)
		{
			lightPosition[0] = 0.0f;
			lightPosition[1] = sin(XRotation) * 45.0f;
			lightPosition[2] = cos(XRotation) * 45.0f;
			lightPosition[3] = 1.0f;
		}
		else if (keyPress == 2)
		{
			lightPosition[0] = cos(YRotation) * 45.0f;
			lightPosition[1] = 0.0f;
			lightPosition[2] = sin(YRotation) * 45.0f;
			lightPosition[3] = 1.0f;
		}
		else if (keyPress == 3)
		{
			lightPosition[0] = cos(ZRotation) * 45.0f;
			lightPosition[1] = sin(ZRotation) * 45.0f;
			lightPosition[2] = 0.0f;
			lightPosition[3] = 1.0f;
		}

		glUniform1i(keyPressUniform, 1); // Light uniforms
		glUniform3fv(lightAmbientUniform, 1, lightAmbient);
		glUniform3fv(lightDefuseUniform, 1, lightDefuse);
		glUniform3fv(lightSpecularUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
	}
	else
	{
		glUniform1i(keyPressUniform, 0);
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			// model Matrix uniform
			modelMatrix = vmath::translate(1.5f + 6.0f * i, 14.0f - 2.5f * j, 0.0f);
			glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

			if (bLightingEnable == TRUE)
			{
				// Material Uniforms
				glUniform3fv(materialDefuseUniform, 1, material[i * 6 + j].materialDefuse);
				glUniform3fv(materialAmbientUniform, 1, material[i * 6 + j].materialAmbient);
				glUniform3fv(materialSpecularUniform, 1, material[i * 6 + j].materialSpecular);
				glUniform1f(materialShininessUniform, material[i * 6 + j].materialShininess);
			}
			glBindVertexArray(vao_sphere);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
			glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
			glBindVertexArray(0);
		}
	}
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	// code
	if (bLightingEnable == TRUE)
	{
		if (keyPress == 1)
		{
			XRotation += 0.02f;

			if (XRotation >= 2 * M_PI)
				XRotation -= 2 * M_PI;
		}
		else if (keyPress == 2)
		{
			YRotation += 0.02f;

			if (YRotation >= 2 * M_PI)
				YRotation -= 2 * M_PI;
		}
		else if (keyPress == 3)
		{
			ZRotation += 0.02f;

			if (ZRotation >= 2 * M_PI)
				ZRotation -= 2 * M_PI;
		}
	}
}

void uninitialize(void)
{
	// fucntion declarations
	void ToggleFullScreen(void);

	// code
	if (perfragment_shaderProgramObject)
	{
		glUseProgram(perfragment_shaderProgramObject);

		GLint numShaders = 0;
		glGetProgramiv(perfragment_shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(perfragment_shaderProgramObject, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachObjectARB(perfragment_shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(perfragment_shaderProgramObject);
		perfragment_shaderProgramObject = 0;
	}

	if (pervertex_shaderProgramObject)
	{
		glUseProgram(pervertex_shaderProgramObject);

		GLint numShaders = 0;
		glGetProgramiv(pervertex_shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(pervertex_shaderProgramObject, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachObjectARB(pervertex_shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(pervertex_shaderProgramObject);
		pervertex_shaderProgramObject = 0;
	}

	// sphere
	if (vbo_sphere_element)
	{
		glDeleteBuffers(1, &vbo_sphere_element);
		vbo_sphere_element = 0;
	}

	if (vbo_sphere_texcoord)
	{
		glDeleteBuffers(1, &vbo_sphere_texcoord);
		vbo_sphere_texcoord = 0;
	}

	if (vbo_sphere_normal)
	{
		glDeleteBuffers(1, &vbo_sphere_normal);
		vbo_sphere_normal = 0;
	}

	// delete vbo of positoion
	if (vbo_sphere_position)
	{
		glDeleteBuffers(1, &vbo_sphere_position);
		vbo_sphere_position = 0;
	}

	// delete vao
	if (vao_sphere)
	{
		glDeleteVertexArrays(1, &vao_sphere);
		vao_sphere = 0;
	}

	// if application is exiting in fullscreen
	if (gbFullscreen == TRUE)
	{
		ToggleFullScreen();
		gbFullscreen = FALSE;
	}

	// make the hdc as current context
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	// Delete rendering context
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	// release the hdc
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// DestroyWindow
	if (ghwnd)
	{
		fclose(stdout);
		FreeConsole();

		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// close the log file
	if (gpFILE)
	{
		fprintf(gpFILE, "Program Ended Successfully...\n");
		fclose(gpFILE);
		gpFILE = NULL;
	}
}

void initMaterialValues()
{
	// ***** 1st sphere on 1st column, emerald *****
	// ambient material
	material[0].materialAmbient[0] = 0.0215; // r
	material[0].materialAmbient[1] = 0.1745; // g
	material[0].materialAmbient[2] = 0.0215; // b
	material[0].materialAmbient[3] = 1.0f;	 // a

	// diffuse material
	material[0].materialDefuse[0] = 0.07568; // r
	material[0].materialDefuse[1] = 0.61424; // g
	material[0].materialDefuse[2] = 0.07568; // b
	material[0].materialDefuse[3] = 1.0f;	  // a

	// specular material
	material[0].materialSpecular[0] = 0.633;	// r
	material[0].materialSpecular[1] = 0.727811; // g
	material[0].materialSpecular[2] = 0.633;	// b
	material[0].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[0].materialShininess = 0.6 * 128;

	// geometry

	// *******************************************************

	// ***** 2nd sphere on 1st column, jade *****
	// ambient material
	material[1].materialAmbient[0] = 0.135;	 // r
	material[1].materialAmbient[1] = 0.2225; // g
	material[1].materialAmbient[2] = 0.1575; // b
	material[1].materialAmbient[3] = 1.0f;	 // a

	// diffuse material
	material[1].materialDefuse[0] = 0.54; // r
	material[1].materialDefuse[1] = 0.89; // g
	material[1].materialDefuse[2] = 0.63; // b
	material[1].materialDefuse[3] = 1.0f; // a

	// specular material
	material[1].materialSpecular[0] = 0.316228; // r
	material[1].materialSpecular[1] = 0.316228; // g
	material[1].materialSpecular[2] = 0.316228; // b
	material[1].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[1].materialShininess = 0.1 * 128;

	// geometry

	// *******************************************************
	// ***** 3rd sphere on 1st column, obsidian *****
	// ambient material
	material[2].materialAmbient[0] = 0.05375; // r
	material[2].materialAmbient[1] = 0.05;	  // g
	material[2].materialAmbient[2] = 0.06625; // b
	material[2].materialAmbient[3] = 1.0f;	  // a

	// diffuse material
	material[2].materialDefuse[0] = 0.18275; // r
	material[2].materialDefuse[1] = 0.17;	  // g
	material[2].materialDefuse[2] = 0.22525; // b
	material[2].materialDefuse[3] = 1.0f;	  // a

	// specular material
	material[2].materialSpecular[0] = 0.332741; // r
	material[2].materialSpecular[1] = 0.328634; // g
	material[2].materialSpecular[2] = 0.346435; // b
	material[2].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[2].materialShininess = 0.3 * 128;

	// geometry

	// *******************************************************

	// ***** 4th sphere on 1st column, pearl *****
	// ambient material
	material[3].materialAmbient[0] = 0.25;	  // r
	material[3].materialAmbient[1] = 0.20725; // g
	material[3].materialAmbient[2] = 0.20725; // b
	material[3].materialAmbient[3] = 1.0f;	  // a

	// diffuse material
	material[3].materialDefuse[0] = 1.0;	// r
	material[3].materialDefuse[1] = 0.829; // g
	material[3].materialDefuse[2] = 0.829; // b
	material[3].materialDefuse[3] = 1.0f;	// a

	// specular material
	material[3].materialSpecular[0] = 0.296648; // r
	material[3].materialSpecular[1] = 0.296648; // g
	material[3].materialSpecular[2] = 0.296648; // b
	material[3].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[3].materialShininess = 0.088 * 128;

	// geometry

	// *******************************************************

	// ***** 5th sphere on 1st column, ruby *****
	// ambient material
	material[4].materialAmbient[0] = 0.1745;  // r
	material[4].materialAmbient[1] = 0.01175; // g
	material[4].materialAmbient[2] = 0.01175; // b
	material[4].materialAmbient[3] = 1.0f;	  // a

	// diffuse material
	material[4].materialDefuse[0] = 0.61424; // r
	material[4].materialDefuse[1] = 0.04136; // g
	material[4].materialDefuse[2] = 0.04136; // b
	material[4].materialDefuse[3] = 1.0f;	  // a

	// specular material
	material[4].materialSpecular[0] = 0.727811; // r
	material[4].materialSpecular[1] = 0.626959; // g
	material[4].materialSpecular[2] = 0.626959; // b
	material[4].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[4].materialShininess = 0.6 * 128;

	// geometry

	// *******************************************************

	// ***** 6th sphere on 1st column, turquoise *****
	// ambient material
	material[5].materialAmbient[0] = 0.1;	  // r
	material[5].materialAmbient[1] = 0.18725; // g
	material[5].materialAmbient[2] = 0.1745;  // b
	material[5].materialAmbient[3] = 1.0f;	  // a

	// diffuse material
	material[5].materialDefuse[0] = 0.396;	  // r
	material[5].materialDefuse[1] = 0.74151; // g
	material[5].materialDefuse[2] = 0.69102; // b
	material[5].materialDefuse[3] = 1.0f;	  // a

	// specular material
	material[5].materialSpecular[0] = 0.297254; // r
	material[5].materialSpecular[1] = 0.30829;	// g
	material[5].materialSpecular[2] = 0.306678; // b
	material[5].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[5].materialShininess = 0.1 * 128;

	// geometry

	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 2nd column, brass *****
	// ambient material
	material[6].materialAmbient[0] = 0.329412; // r
	material[6].materialAmbient[1] = 0.223529; // g
	material[6].materialAmbient[2] = 0.027451; // b
	material[6].materialAmbient[3] = 1.0f;	   // a

	// diffuse material
	material[6].materialDefuse[0] = 0.780392; // r
	material[6].materialDefuse[1] = 0.568627; // g
	material[6].materialDefuse[2] = 0.113725; // b
	material[6].materialDefuse[3] = 1.0f;	   // a

	// specular material
	material[6].materialSpecular[0] = 0.992157; // r
	material[6].materialSpecular[1] = 0.941176; // g
	material[6].materialSpecular[2] = 0.807843; // b
	material[6].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[6].materialShininess = 0.21794872 * 128;

	// geometry

	// *******************************************************

	// ***** 2nd sphere on 2nd column, bronze *****
	// ambient material
	material[7].materialAmbient[0] = 0.2125; // r
	material[7].materialAmbient[1] = 0.1275; // g
	material[7].materialAmbient[2] = 0.054;	 // b
	material[7].materialAmbient[3] = 1.0f;	 // a

	// diffuse material
	material[7].materialDefuse[0] = 0.714;	  // r
	material[7].materialDefuse[1] = 0.4284;  // g
	material[7].materialDefuse[2] = 0.18144; // b
	material[7].materialDefuse[3] = 1.0f;	  // a

	// specular material
	material[7].materialSpecular[0] = 0.393548; // r
	material[7].materialSpecular[1] = 0.271906; // g
	material[7].materialSpecular[2] = 0.166721; // b
	material[7].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[7].materialShininess = 0.2 * 128;

	// geometry

	// *******************************************************

	// ***** 3rd sphere on 2nd column, chrome *****
	// ambient material
	material[8].materialAmbient[0] = 0.25; // r
	material[8].materialAmbient[1] = 0.25; // g
	material[8].materialAmbient[2] = 0.25; // b
	material[8].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[8].materialDefuse[0] = 0.4;  // r
	material[8].materialDefuse[1] = 0.4;  // g
	material[8].materialDefuse[2] = 0.4;  // b
	material[8].materialDefuse[3] = 1.0f; // a

	// specular material
	material[8].materialSpecular[0] = 0.774597; // r
	material[8].materialSpecular[1] = 0.774597; // g
	material[8].materialSpecular[2] = 0.774597; // b
	material[8].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[8].materialShininess = 0.6 * 128;

	// geometry

	// *******************************************************

	// ***** 4th sphere on 2nd column, copper *****
	// ambient material
	material[9].materialAmbient[0] = 0.19125; // r
	material[9].materialAmbient[1] = 0.0735;  // g
	material[9].materialAmbient[2] = 0.0225;  // b
	material[9].materialAmbient[3] = 1.0f;	  // a

	// diffuse material
	material[9].materialDefuse[0] = 0.7038;  // r
	material[9].materialDefuse[1] = 0.27048; // g
	material[9].materialDefuse[2] = 0.0828;  // b
	material[9].materialDefuse[3] = 1.0f;	  // a

	// specular material
	material[9].materialSpecular[0] = 0.256777; // r
	material[9].materialSpecular[1] = 0.137622; // g
	material[9].materialSpecular[2] = 0.086014; // b
	material[9].materialSpecular[3] = 1.0f;		// a

	// shininess
	material[9].materialShininess = 0.1 * 128;

	// geometry

	// *******************************************************

	// ***** 5th sphere on 2nd column, gold *****

	// ambient material
	material[10].materialAmbient[0] = 0.24725; // r
	material[10].materialAmbient[1] = 0.1995;  // g
	material[10].materialAmbient[2] = 0.0745;  // b
	material[10].materialAmbient[3] = 1.0f;	   // a

	// diffuse material
	material[10].materialDefuse[0] = 0.75164; // r
	material[10].materialDefuse[1] = 0.60648; // g
	material[10].materialDefuse[2] = 0.22648; // b
	material[10].materialDefuse[3] = 1.0f;	   // a

	// specular material
	material[10].materialSpecular[0] = 0.628281; // r
	material[10].materialSpecular[1] = 0.555802; // g
	material[10].materialSpecular[2] = 0.366065; // b
	material[10].materialSpecular[3] = 1.0f;	 // a

	// shininess
	material[10].materialShininess = 0.4 * 128;

	// geometry

	// *******************************************************

	// ***** 6th sphere on 2nd column, silver *****
	// ambient material
	material[11].materialAmbient[0] = 0.19225; // r
	material[11].materialAmbient[1] = 0.19225; // g
	material[11].materialAmbient[2] = 0.19225; // b
	material[11].materialAmbient[3] = 1.0f;	   // a

	// diffuse material
	material[11].materialDefuse[0] = 0.50754; // r
	material[11].materialDefuse[1] = 0.50754; // g
	material[11].materialDefuse[2] = 0.50754; // b
	material[11].materialDefuse[3] = 1.0f;	   // a

	// specular material
	material[11].materialSpecular[0] = 0.508273; // r
	material[11].materialSpecular[1] = 0.508273; // g
	material[11].materialSpecular[2] = 0.508273; // b
	material[11].materialSpecular[3] = 1.0f;	 // a

	// shininess
	material[11].materialShininess = 0.4 * 128;

	// geometry

	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 3rd column, black *****
	// ambient material
	material[12].materialAmbient[0] = 0.0;	// r
	material[12].materialAmbient[1] = 0.0;	// g
	material[12].materialAmbient[2] = 0.0;	// b
	material[12].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[12].materialDefuse[0] = 0.01; // r
	material[12].materialDefuse[1] = 0.01; // g
	material[12].materialDefuse[2] = 0.01; // b
	material[12].materialDefuse[3] = 1.0f; // a

	// specular material
	material[12].materialSpecular[0] = 0.50; // r
	material[12].materialSpecular[1] = 0.50; // g
	material[12].materialSpecular[2] = 0.50; // b
	material[12].materialSpecular[3] = 1.0f; // a

	// shininess
	material[12].materialShininess = 0.25 * 128;

	// geometry

	// *******************************************************

	// ***** 2nd sphere on 3rd column, cyan *****
	// ambient material
	material[13].materialAmbient[0] = 0.0;	// r
	material[13].materialAmbient[1] = 0.1;	// g
	material[13].materialAmbient[2] = 0.06; // b
	material[13].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[13].materialDefuse[0] = 0.0;		  // r
	material[13].materialDefuse[1] = 0.50980392; // g
	material[13].materialDefuse[2] = 0.50980392; // b
	material[13].materialDefuse[3] = 1.0f;		  // a

	// specular material
	material[13].materialSpecular[0] = 0.50196078; // r
	material[13].materialSpecular[1] = 0.50196078; // g
	material[13].materialSpecular[2] = 0.50196078; // b
	material[13].materialSpecular[3] = 1.0f;	   // a

	// shininess
	material[13].materialShininess = 0.25 * 128;

	// geometry

	// *******************************************************

	// ***** 3rd sphere on 2nd column, green *****
	// ambient material
	material[14].materialAmbient[0] = 0.0;	// r
	material[14].materialAmbient[1] = 0.0;	// g
	material[14].materialAmbient[2] = 0.0;	// b
	material[14].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[14].materialDefuse[0] = 0.1;	// r
	material[14].materialDefuse[1] = 0.35; // g
	material[14].materialDefuse[2] = 0.1;	// b
	material[14].materialDefuse[3] = 1.0f; // a

	// specular material
	material[14].materialSpecular[0] = 0.45; // r
	material[14].materialSpecular[1] = 0.55; // g
	material[14].materialSpecular[2] = 0.45; // b
	material[14].materialSpecular[3] = 1.0f; // a

	// shininess
	material[14].materialShininess = 0.25 * 128;

	// geometry

	// *******************************************************

	// ***** 4th sphere on 3rd column, red *****
	// ambient material
	material[15].materialAmbient[0] = 0.0;	// r
	material[15].materialAmbient[1] = 0.0;	// g
	material[15].materialAmbient[2] = 0.0;	// b
	material[15].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[15].materialDefuse[0] = 0.5;	// r
	material[15].materialDefuse[1] = 0.0;	// g
	material[15].materialDefuse[2] = 0.0;	// b
	material[15].materialDefuse[3] = 1.0f; // a

	// specular material
	material[15].materialSpecular[0] = 0.7;	 // r
	material[15].materialSpecular[1] = 0.6;	 // g
	material[15].materialSpecular[2] = 0.6;	 // b
	material[15].materialSpecular[3] = 1.0f; // a

	// shininess
	material[15].materialShininess = 0.25 * 128;

	// geometry

	// *******************************************************

	// ***** 5th sphere on 3rd column, white *****
	// ambient material
	material[16].materialAmbient[0] = 0.0;	// r
	material[16].materialAmbient[1] = 0.0;	// g
	material[16].materialAmbient[2] = 0.0;	// b
	material[16].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[16].materialDefuse[0] = 0.55; // r
	material[16].materialDefuse[1] = 0.55; // g
	material[16].materialDefuse[2] = 0.55; // b
	material[16].materialDefuse[3] = 1.0f; // a

	// specular material
	material[16].materialSpecular[0] = 0.70; // r
	material[16].materialSpecular[1] = 0.70; // g
	material[16].materialSpecular[2] = 0.70; // b
	material[16].materialSpecular[3] = 1.0f; // a

	// shininess
	material[16].materialShininess = 0.25 * 128;

	// geometry

	// *******************************************************

	// ***** 6th sphere on 3rd column, yellow plastic *****
	// ambient material
	material[17].materialAmbient[0] = 0.0;	// r
	material[17].materialAmbient[1] = 0.0;	// g
	material[17].materialAmbient[2] = 0.0;	// b
	material[17].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[17].materialDefuse[0] = 0.5;	// r
	material[17].materialDefuse[1] = 0.5;	// g
	material[17].materialDefuse[2] = 0.0;	// b
	material[17].materialDefuse[3] = 1.0f; // a

	// specular material
	material[17].materialSpecular[0] = 0.60; // r
	material[17].materialSpecular[1] = 0.60; // g
	material[17].materialSpecular[2] = 0.50; // b
	material[17].materialSpecular[3] = 1.0f; // a

	// shininess
	material[17].materialShininess = 0.25 * 128;

	// geometry

	// *******************************************************
	// *******************************************************
	// *******************************************************

	// ***** 1st sphere on 4th column, black *****
	// ambient material
	material[18].materialAmbient[0] = 0.02; // r
	material[18].materialAmbient[1] = 0.02; // g
	material[18].materialAmbient[2] = 0.02; // b
	material[18].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[18].materialDefuse[0] = 0.01; // r
	material[18].materialDefuse[1] = 0.01; // g
	material[18].materialDefuse[2] = 0.01; // b
	material[18].materialDefuse[3] = 1.0f; // a

	// specular material
	material[18].materialSpecular[0] = 0.4;	 // r
	material[18].materialSpecular[1] = 0.4;	 // g
	material[18].materialSpecular[2] = 0.4;	 // b
	material[18].materialSpecular[3] = 1.0f; // a

	// shininess
	material[18].materialShininess = 0.078125 * 128;

	// geometry

	// *******************************************************

	// ***** 2nd sphere on 4th column, cyan *****
	// ambient material
	material[19].materialAmbient[0] = 0.0;	// r
	material[19].materialAmbient[1] = 0.05; // g
	material[19].materialAmbient[2] = 0.05; // b
	material[19].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[19].materialDefuse[0] = 0.4;	// r
	material[19].materialDefuse[1] = 0.5;	// g
	material[19].materialDefuse[2] = 0.5;	// b
	material[19].materialDefuse[3] = 1.0f; // a

	// specular material
	material[19].materialSpecular[0] = 0.04; // r
	material[19].materialSpecular[1] = 0.7;	 // g
	material[19].materialSpecular[2] = 0.7;	 // b
	material[19].materialSpecular[3] = 1.0f; // a

	// shininess
	material[19].materialShininess = 0.078125 * 128;

	// geometry

	// *******************************************************

	// ***** 3rd sphere on 4th column, green *****
	// ambient material
	material[20].materialAmbient[0] = 0.0;	// r
	material[20].materialAmbient[1] = 0.05; // g
	material[20].materialAmbient[2] = 0.0;	// b
	material[20].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[20].materialDefuse[0] = 0.4;	// r
	material[20].materialDefuse[1] = 0.5;	// g
	material[20].materialDefuse[2] = 0.4;	// b
	material[20].materialDefuse[3] = 1.0f; // a

	// specular material
	material[20].materialSpecular[0] = 0.04; // r
	material[20].materialSpecular[1] = 0.7;	 // g
	material[20].materialSpecular[2] = 0.04; // b
	material[20].materialSpecular[3] = 1.0f; // a

	// shininess
	material[20].materialShininess = 0.078125 * 128;

	// geometry

	// *******************************************************

	// ***** 4th sphere on 4th column, red *****
	// ambient material
	material[21].materialAmbient[0] = 0.05; // r
	material[21].materialAmbient[1] = 0.0;	// g
	material[21].materialAmbient[2] = 0.0;	// b
	material[21].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[21].materialDefuse[0] = 0.5;	// r
	material[21].materialDefuse[1] = 0.4;	// g
	material[21].materialDefuse[2] = 0.4;	// b
	material[21].materialDefuse[3] = 1.0f; // a

	// specular material
	material[21].materialSpecular[0] = 0.7;	 // r
	material[21].materialSpecular[1] = 0.04; // g
	material[21].materialSpecular[2] = 0.04; // b
	material[21].materialSpecular[3] = 1.0f; // a

	// shininess
	material[21].materialShininess = 0.078125 * 128;

	// geometry

	// *******************************************************

	// ***** 5th sphere on 4th column, white *****
	// ambient material
	material[22].materialAmbient[0] = 0.05; // r
	material[22].materialAmbient[1] = 0.05; // g
	material[22].materialAmbient[2] = 0.05; // b
	material[22].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[22].materialDefuse[0] = 0.5;	// r
	material[22].materialDefuse[1] = 0.5;	// g
	material[22].materialDefuse[2] = 0.5;	// b
	material[22].materialDefuse[3] = 1.0f; // a

	// specular material
	material[22].materialSpecular[0] = 0.7;	 // r
	material[22].materialSpecular[1] = 0.7;	 // g
	material[22].materialSpecular[2] = 0.7;	 // b
	material[22].materialSpecular[3] = 1.0f; // a

	// shininess
	material[22].materialShininess = 0.078125 * 128;

	// geometry

	// *******************************************************

	// ***** 6th sphere on 4th column, yellow rubber *****
	// ambient material
	material[23].materialAmbient[0] = 0.05; // r
	material[23].materialAmbient[1] = 0.05; // g
	material[23].materialAmbient[2] = 0.0;	// b
	material[23].materialAmbient[3] = 1.0f; // a

	// diffuse material
	material[23].materialDefuse[0] = 0.5;	// r
	material[23].materialDefuse[1] = 0.5;	// g
	material[23].materialDefuse[2] = 0.4;	// b
	material[23].materialDefuse[3] = 1.0f; // a

	// specular material
	material[23].materialSpecular[0] = 0.7;	 // r
	material[23].materialSpecular[1] = 0.7;	 // g
	material[23].materialSpecular[2] = 0.04; // b
	material[23].materialSpecular[3] = 1.0f; // a
	material[23].materialShininess = 0.078125 * 128;
}
