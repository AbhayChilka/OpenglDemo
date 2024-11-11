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

GLuint pervertex_shaderProgramObject_sphere = 0;
GLuint perfragment_shaderProgramObject_sphere = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCOORD,
	AMC_ATTRIBUTE_NORMAL
};

struct Light
{
	vec3 ambient;
	vec3 defuse;
	vec3 specular;
	vec4 position;
};

struct Light light[3];

GLuint vao_sphere = 0;
GLuint vbo_sphere_position = 0;
GLuint vbo_sphere_normal = 0;
GLuint vbo_sphere_element = 0;
GLuint vbo_sphere_texcoord = 0;
// GLuint vbo_color_sphere = 0;

// GLuint mvpMatrixUniform = 0;
GLuint modelViewMatrixUniform_sphere = 0;
GLuint projectionMatrixUniform_sphere = 0;
GLuint modelMatrixUniform_sphere, uMatrixUniform_sphere;
// material
GLuint lightAmbientUniform_sphere[3];
GLuint lightDefuseUniform_sphere[3];
GLuint lightSpecularUniform_sphere[3];
GLuint lightPositionUniform_sphere[3];
GLuint materialAmbientUniform_sphere = 0;
GLuint materialDefuseUniform_sphere = 0;
GLuint materialSpecularUniform_sphere = 0;
GLuint materialShininessUniform_sphere = 0;
GLuint keyPressUniform_sphere = 0;

BOOL bLightingEnable = FALSE;

/*GLfloat lightDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialDefuse[] = {0.5f, 0.5f, 0.5f, 0.5f};
GLfloat lightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};*/

GLuint gNumElements;
GLuint gNumVertices;

GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShininess = 128.0f;

mat4 perspectiveProjetionMatrix_sphere;

char choosenShader = 'v';

float angle_sphere = 0.0f;

// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// fucntion declaration
	int initialize_sphere(void);
	void uninitialize_sphere(void);
	void display_sphere(void);
	void update_sphere(void);

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
	iResult = initialize_sphere();
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
				display_sphere();

				// UPDATE
				update_sphere();
			}
		}
	}
	uninitialize_sphere();

	return ((int)msg.wParam);
}

// CallBack Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Function Declarations
	void ToggleFullScreen(void);
	void resize_sphere(int, int);

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
		resize_sphere(LOWORD(lParam), HIWORD(lParam));
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
		case 'V':
		case 'v':
			if (bLightingEnable == TRUE)
			{
				choosenShader = 'v';

				modelMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uModelMatrix");
				modelViewMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uViewMatrix");
				projectionMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uProjectionMatrix");

				lightAmbientUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[0]");
				lightDefuseUniform_sphere[0]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[0]");
				lightSpecularUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[0]");
				lightPositionUniform_sphere[0]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[0]");

				lightAmbientUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[1]");
				lightDefuseUniform_sphere [1]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[1]");
				lightSpecularUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[1]");
				lightPositionUniform_sphere[1]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[1]");

				lightAmbientUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[2]");
				lightDefuseUniform_sphere [2]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[2]");
				lightSpecularUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[2]");
				lightPositionUniform_sphere[2]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[2]");

				materialAmbientUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialAmbient");
				materialDefuseUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialDefuse");
				materialSpecularUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialSpecular");
				materialShininessUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialShininess");

				keyPressUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uKeyPress");
			}						
			break;

		case 'F':
		case 'f':
				if (bLightingEnable == TRUE)
				{
				choosenShader = 'f';

				modelMatrixUniform_sphere = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uModelMatrix");
				modelViewMatrixUniform_sphere = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uViewMatrix");
				projectionMatrixUniform_sphere = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uProjectionMatrix");

				lightAmbientUniform_sphere[0] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightAmbient[0]");
				lightDefuseUniform_sphere[0] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightDefuse[0]");
				lightSpecularUniform_sphere[0] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightSpecular[0]");
				lightPositionUniform_sphere[0] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightPosition[0]");

				lightAmbientUniform_sphere[1] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightAmbient[1]");
				lightDefuseUniform_sphere[1] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightDefuse[1]");
				lightSpecularUniform_sphere[1] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightSpecular[1]");
				lightPositionUniform_sphere[1] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightPosition[1]");
				
				lightAmbientUniform_sphere[2] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightAmbient[2]");
				lightDefuseUniform_sphere[2] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightDefuse[2]");
				lightSpecularUniform_sphere[2] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightSpecular[2]");
				lightPositionUniform_sphere[2] = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uLightPosition[2]");

				materialAmbientUniform_sphere = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uMaterialAmbient");
				materialDefuseUniform_sphere = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uMaterialDefuse");
				materialSpecularUniform_sphere = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uMaterialSpecular");
				materialShininessUniform_sphere = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uMaterialShininess");

				keyPressUniform_sphere = glGetUniformLocation(perfragment_shaderProgramObject_sphere, "uKeyPress");
				}
				
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

int initialize_sphere(void)
{
	// function declarations
	void printgGLInfo(void);
	void resize_sphere(int, int);
	void uninitialize_sphere(void);

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
										"uniform vec3 uLightAmbient[3];"
										"uniform vec3 uLightDefuse[3];"
										"uniform vec3 uLightSpecular[3];"
										"uniform vec4 uLightPosition[3];"
										"uniform vec3 uMaterialDefuse;"
										"uniform vec3 uMaterialAmbient;"
										"uniform vec3 uMaterialSpecular;"
										"uniform float uMaterialShininess;"
										"uniform int uKeyPress;"
										"out vec3 oPhong_ADS_Light;"
										"void main(void)"
										"{"
										"if(uKeyPress == 1)"
										"{"
										"vec3 lightDirection[3];"
										"vec3 defuseLight[3];"
										"vec3 ambientLight[3];"
										"vec3 specularLight[3];"
										"vec3 reflectionVector[3];"
										"vec4 iCoordinates = uViewMatrix * uModelMatrix * aPosition;"
										"vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);"
										"for(int i=0; i<3 ; i++)"
										"{"
										"lightDirection[i] = normalize(vec3(uLightPosition[i] - iCoordinates));"
										"reflectionVector[i] = reflect(-lightDirection[i],transformedNormals);"
										"vec3 viewerVector = normalize(-iCoordinates.xyz);"
										"ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;"
										"defuseLight[i] = uLightDefuse[i] * uMaterialDefuse * max(dot(lightDirection[i],transformedNormals),0.0);"
										"specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i],viewerVector),0.0),uMaterialShininess);"
										"oPhong_ADS_Light = oPhong_ADS_Light + ambientLight[i] + defuseLight[i] + specularLight[i];"
										"}"
										"}"
										"else"
										"{"
										"oPhong_ADS_Light = vec3(0.0f,0.0f,0.0f);"
										"}"
										"gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"
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
		uninitialize_sphere();
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
											" if(uKeyPress == 1)"
											"FragColor = vec4(oPhong_ADS_Light,1.0f);"
											"else"
											"{"
											"FragColor = vec4(1.0, 1.0, 1.0, 1.0);"
											"}"
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
		uninitialize_sphere();
	}

	// shader program
	pervertex_shaderProgramObject_sphere = glCreateProgram();

	glAttachShader(pervertex_shaderProgramObject_sphere, pervertex_vertexShaderObject);
	glAttachShader(pervertex_shaderProgramObject_sphere, pervertex_fragmentShaderObject);

	// prelinking
	glBindAttribLocation(pervertex_shaderProgramObject_sphere, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(pervertex_shaderProgramObject_sphere, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(pervertex_shaderProgramObject_sphere);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetProgramiv(pervertex_shaderProgramObject_sphere, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(pervertex_shaderProgramObject_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog = (GLchar *)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetProgramInfoLog(pervertex_shaderProgramObject_sphere, infoLogLength, NULL, szinfolog);
				fprintf(gpFILE, "Shader Program linking error log: %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}
		uninitialize_sphere();
	}

				modelMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uModelMatrix");
				modelViewMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uViewMatrix");
				projectionMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uProjectionMatrix");

				lightAmbientUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[0]");
				lightDefuseUniform_sphere[0]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[0]");
				lightSpecularUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[0]");
				lightPositionUniform_sphere[0]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[0]");

				lightAmbientUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[1]");
				lightDefuseUniform_sphere [1]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[1]");
				lightSpecularUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[1]");
				lightPositionUniform_sphere[1]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[1]");

				lightAmbientUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[2]");
				lightDefuseUniform_sphere [2]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[2]");
				lightSpecularUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[2]");
				lightPositionUniform_sphere[2]= glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[2]");

				materialAmbientUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialAmbient");
				materialDefuseUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialDefuse");
				materialSpecularUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialSpecular");
				materialShininessUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialShininess");

				keyPressUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uKeyPress");

//////////---------------------------------------------------------------------------------------------------------------//////////


/////////-----------------------------------------------perfragment------------------------------------------------------//////////

	// Vertex Shader
	const GLchar *perfragment_vertexShaderSourceCode =
										"#version 460 core"
										"\n"
										"in vec4 aPosition;"
										"in vec3 aNormal;"
										"uniform mat4 uModelMatrix;"
										"uniform mat4 uViewMatrix;"
										"uniform mat4 uProjectionMatrix;"
										"uniform vec4 uLightPosition[3];"
										"uniform int uKeyPress;"
										"out vec3 oTransformedNormals;"
										"out vec3 oViewerVector;"
										"out vec3 oLightDirection[3];"
										"void main(void)"
										"{"
										"if(uKeyPress == 1)"
										"{"
										"vec4 iCoordinates = uViewMatrix * uModelMatrix * aPosition;"
										"oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;"
										"oViewerVector = -iCoordinates.xyz;"
										"for(int i=0; i<3 ; i++)"
										"{"
										"oLightDirection[i] = vec3(uLightPosition[i] - iCoordinates);"
										"}"
										"}"
										"else"
										"{"
										"oTransformedNormals = vec3(0.0f,0.0f,0.0f);"
										"oViewerVector = vec3(0.0f,0.0f,0.0f);"
										"oLightDirection[0] = vec3(0.0f,0.0f,0.0f);"
										"oLightDirection[1] = vec3(0.0f,0.0f,0.0f);"
										"oLightDirection[2]= vec3(0.0f, 0.0f, 0.0f);"
										"}"
										" gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"
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
		uninitialize_sphere();
	}

	// fragment shader
	const GLchar *perfragment_fragmentShaderSourceCode =
							"#version 460 core"
							"\n"
							"in vec3 oTransformedNormals;"
							"in vec3 oViewerVector;"
							"in vec3 oLightDirection[3];"
							"uniform vec3 uLightDefuse[3];"
							"uniform vec3 uLightAmbient[3];"
							"uniform vec3 uLightSpecular[3];"
							"uniform vec3 uMaterialDefuse;"
							"uniform vec3 uMaterialAmbient;"
							"uniform vec3 uMaterialSpecular;"
							"uniform float uMaterialShininess;"
							"uniform int uKeyPress;"
							"out vec4 FragColor;"
							"void main(void)"
							"{"
							"  vec3 phongADSLight = vec3(0.0f,0.0f,0.0f);"
							"  if(uKeyPress == 1)"
							"	{"
							"		vec3 viewerVector = normalize(oViewerVector);"
							"		vec3 transformedNormals = normalize(oTransformedNormals);"
							"		vec3 lightDirection[3];"
							"		vec3 defuseLight[3];"
							"		vec3 ambientLight[3];"
							"		vec3 specularLight[3];"
							"		vec3 reflectionVector[3];"
							"		for(int i=0; i<3 ; i++)"
							"		{"
							"		   lightDirection[i] = normalize(oLightDirection[i]);"
							"		   reflectionVector[i] = reflect(-lightDirection[i],transformedNormals);"
							"		   ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;"
							"		   defuseLight[i] = uLightDefuse[i] * uMaterialDefuse * max(dot(lightDirection[i],transformedNormals),0.0);"
							"		   specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i],viewerVector),0.0),uMaterialShininess);"
							"		   phongADSLight = phongADSLight + ambientLight[i] + defuseLight[i] + specularLight[i];"
							"		}"
							"	}"
							"  else"
							"	phongADSLight = vec3(1.0f,1.0f,1.0f);"
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
		uninitialize_sphere();
	}

	// shader program
	perfragment_shaderProgramObject_sphere = glCreateProgram();

	glAttachShader(perfragment_shaderProgramObject_sphere, perfragment_vertexShaderObject);
	glAttachShader(perfragment_shaderProgramObject_sphere, perfragment_fragmentShaderObject);

	// prelinking
	glBindAttribLocation(perfragment_shaderProgramObject_sphere, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(perfragment_shaderProgramObject_sphere, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(perfragment_shaderProgramObject_sphere);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetProgramiv(perfragment_shaderProgramObject_sphere, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(perfragment_shaderProgramObject_sphere, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog = (GLchar *)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetProgramInfoLog(perfragment_shaderProgramObject_sphere, infoLogLength, NULL, szinfolog);
				fprintf(gpFILE, "Shader Program linking error log: %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}
			uninitialize_sphere();
	}
//////////---------------------------------------------------------------------------------------------------------------//////////

	float sphere_positons[1146];
	float sphere_normals[1146];
	float sphere_texcoords[764];
	unsigned short sphere_elements[2280];

	light[0].ambient = vec3(0.0f, 0.0f, 0.0f);
	light[1].ambient = vec3(0.0f, 0.0f, 0.0f);
	light[2].ambient = vec3(0.0f, 0.0f, 0.0f);

	light[0].defuse = vec3(1.0f, 0.0f, 0.0f); // REd Light Source
	light[1].defuse =  vec3(0.0f, 1.0f, 0.0f); // Blue Light Source
	light[2].defuse = vec3(0.0f, 0.0f, 1.0f); //Green Light 

	light[0].specular = vec3(1.0f, 1.0f, 1.0f);
	light[1].specular = vec3(1.0f, 1.0f, 1.0f);
	light[2].specular = vec3(1.0f, 1.0f, 1.0f);

	light[0].position = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[1].position = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light[2].position = vec4(0.0f, 0.0f, 0.0f, 1.0f);

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

	// texture vbo sphere note- will not use it in light
	glGenBuffers(1, &vbo_sphere_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_texcoord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_texcoords), sphere_texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	// glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
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
	perspectiveProjetionMatrix_sphere = vmath::mat4::identity();

	resize_sphere(WIN_WIDTH, WIN_HEIGHT);
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

void resize_sphere(int width, int height)
{
	// code
	if (height <= 0)
		height = 1;

	// resize
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// set perspective projection matrix
	perspectiveProjetionMatrix_sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display_sphere(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	vmath::mat4 modelViewMatrix = vmath::mat4 ::identity();

	mat4 translationmatrix = mat4::identity();

	mat4 rotationmatrix = mat4::identity();

	mat4 scalematrix = mat4::identity();

	// transformation
	// sphere
	translationmatrix = vmath::translate(0.0f, 0.0f, -3.0f);

	//view matrix
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	// Model Matrix
	vmath::mat4 modelMatrix = translationmatrix;

	if (choosenShader == 'v')
	{
		glUseProgram(pervertex_shaderProgramObject_sphere);
	}	
	else if (choosenShader == 'f')
	{
		glUseProgram(perfragment_shaderProgramObject_sphere);		
	}
	
	// push above mvp into vertex shaders mvpuniform
	glUniformMatrix4fv(modelMatrixUniform_sphere, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(modelViewMatrixUniform_sphere, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_sphere, 1, GL_FALSE, perspectiveProjetionMatrix_sphere);

	if (bLightingEnable == TRUE)
	{
		glUniform1i(keyPressUniform_sphere, 1);
		glUniform3fv(lightAmbientUniform_sphere[0], 1, light[0].ambient);
		glUniform3fv(lightDefuseUniform_sphere[0], 1, light[0].defuse);
		glUniform3fv(lightSpecularUniform_sphere[0], 1, light[0].specular);
		glUniform4fv(lightPositionUniform_sphere[0], 1, light[0].position);

		glUniform3fv(lightAmbientUniform_sphere[1], 1, light[1].ambient);
		glUniform3fv(lightDefuseUniform_sphere[1], 1, light[1].defuse);
		glUniform3fv(lightSpecularUniform_sphere[1], 1, light[1].specular);
		glUniform4fv(lightPositionUniform_sphere[1], 1, light[1].position);

		glUniform3fv(lightAmbientUniform_sphere[2], 1, light[2].ambient);
		glUniform3fv(lightDefuseUniform_sphere[2], 1, light[2].defuse);
		glUniform3fv(lightSpecularUniform_sphere[2], 1, light[2].specular);
		glUniform4fv(lightPositionUniform_sphere[2], 1, light[2].position);

		glUniform3fv(materialAmbientUniform_sphere, 1, materialAmbient);
		glUniform3fv(materialDefuseUniform_sphere, 1, materialDefuse);
		glUniform3fv(materialSpecularUniform_sphere, 1, materialSpecular);
		glUniform1f(materialShininessUniform_sphere, materialShininess);
	}
	else
	{
		glUniform1i(keyPressUniform_sphere, 0);
	}

	// *** bind vao ***
	glBindVertexArray(vao_sphere);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

		// *** unbind vao ***
	glBindVertexArray(0);

		glUseProgram(0);
	
	
		
	SwapBuffers(ghdc);
}

void update_sphere(void)
{
	// code
	angle_sphere += 0.005f;

	light[0].position[0] = 5.0f * cos(angle_sphere) + 5.0f *sin(angle_sphere);
	light[0].position[1] = 0.0f;
	light[0].position[2] = 5.0f * cos(angle_sphere) - 5.0f *sin(angle_sphere);	
	light[0].position[3] = 1.0f;

	light[1].position[0] = 0.0f;
	light[1].position[1] = 5.0f * cos(angle_sphere) + 5.0f *sin(angle_sphere);
	light[1].position[2] = 5.0f * cos(angle_sphere) - 5.0f *sin(angle_sphere);	
	light[1].position[3] = 1.0f;

	light[2].position[0] = 5.0f * cos(angle_sphere) + 5.0f *sin(angle_sphere);
	light[2].position[1] = 5.0f * cos(angle_sphere) - 5.0f *sin(angle_sphere);	
	light[2].position[2] = 0.0f;
	light[2].position[3] = 1.0f;

}

void uninitialize_sphere(void)
{
	// fucntion declarations
	void ToggleFullScreen(void);

	// code
	if (perfragment_shaderProgramObject_sphere)
	{
		glUseProgram(perfragment_shaderProgramObject_sphere);

		GLint numShaders = 0;
		glGetProgramiv(perfragment_shaderProgramObject_sphere, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(perfragment_shaderProgramObject_sphere, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachObjectARB(perfragment_shaderProgramObject_sphere, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(perfragment_shaderProgramObject_sphere);
		perfragment_shaderProgramObject_sphere = 0;
	}

	if (pervertex_shaderProgramObject_sphere)
	{
		glUseProgram(pervertex_shaderProgramObject_sphere);

		GLint numShaders = 0;
		glGetProgramiv(pervertex_shaderProgramObject_sphere, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(pervertex_shaderProgramObject_sphere, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachObjectARB(pervertex_shaderProgramObject_sphere, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(pervertex_shaderProgramObject_sphere);
		pervertex_shaderProgramObject_sphere = 0;
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
