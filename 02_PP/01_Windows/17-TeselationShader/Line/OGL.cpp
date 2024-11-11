// Windows Header Files
#include <windows.h>     //sdk win32api
#include <stdio.h>      //file io
#include <stdlib.h>     //exit()

//OpenGL Header File
#include <GL/glew.h> //this must be before <GL/gl.h>
#include <GL/gl.h>

#include "vmath.h"
using namespace vmath;

#include "OGL.h"

//macors
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//link with opengl library
#pragma comment(lib, "glew32.lib")
#pragma comment(lib,"opengl32.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT,  WPARAM, LPARAM);

// Global Variable Declarations

// for fileIO
FILE *gpFILE = NULL;   

//for fullscreen
HWND ghwnd = NULL;  
DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen = FALSE;

//for active/inactive
BOOL gbActiveWindow = FALSE;

//OpenGL related global variables
HDC ghdc = NULL;
HGLRC ghrc = NULL;

GLuint shaderProgramObject = 0;

enum 
{
	AMC_ATTRIBUTE_POSITION = 0 
};

GLuint vao = 0;
GLuint vbo_position = 0;

GLuint mvpMatrixUniform = 0;

GLuint numberofSegmentsUniform;
GLuint numberofStripsUniform;
GLuint lineColorUniform;

unsigned int uiNumberOfLineSegments;

mat4 perspectiveProjetionMatrix;

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//fucntion declaration
	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	//Local Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;                                                                                                                              
	TCHAR szAppName[] = TEXT(" ABCWindow ");

	int iResult = 0;

	BOOL bDone = FALSE;   //game loop

    //for centering
	int ScreenHeight;      
    int ScreenWidth;
    int WindowHeight = 600;
    int WindowWidth = 800;
    
    ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    int X = ScreenWidth / 2 - WindowWidth / 2;
    int Y = ScreenHeight / 2 - WindowHeight / 2;


	//code
//	gpFILE = fopen("Log.txt", "w");
//	fopen_s 
	if (fopen_s(&gpFILE, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Cannot Be Opend"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	
	fprintf(gpFILE, "Program Started Successfully\n");
	fprintf(gpFILE, "-------------------------------------------\n");
	
	//WNDCEX initialization
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

	//Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	//Create Window
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

	//Initialization
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
	//game-loop
	while (bDone == FALSE)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
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
				//RENDER
				display();

				//UPDATE
				update();
			}
			
		}
	}
	uninitialize();

	return((int)msg.wParam);
}


//CallBack Function
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
		return(0);
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;

		case VK_UP:
			uiNumberOfLineSegments++;
			if(uiNumberOfLineSegments >= 30)
				uiNumberOfLineSegments = 30;
			break;

		case VK_DOWN:
			uiNumberOfLineSegments--;
			if(uiNumberOfLineSegments <= 1)
				uiNumberOfLineSegments = 1;
			break;
		}
		break; 
	
	case WM_CHAR:
		switch(LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if(gbFullscreen == FALSE)
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

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY :
		if (gpFILE)
		{
			fprintf(gpFILE,"Program Ended Successfully...\n");
			fclose(gpFILE);
			gpFILE = NULL;
		}
		
		PostQuitMessage(0);
		break;
	default :
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullScreen(void)
{
	// Local Variable Declarations
	MONITORINFO mi = { sizeof(MONITORINFO) };

	// Code
	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE); 

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) 
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);  // ~Remove Contents
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);

		SetWindowLong(ghwnd, GWL_STYLE, dwStyle |  WS_OVERLAPPEDWINDOW);

		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

int initialize(void)
{
	//function declarations
	void printgGLInfo(void);
	void resize(int, int);
	void uninitialize(void);
	
	//code
	//variable declaration
	PIXELFORMATDESCRIPTOR PFD;
	int iPixelFormatIndex = 0;

	//initializing PFD struct with 0
	ZeroMemory(&PFD, sizeof(PIXELFORMATDESCRIPTOR));

	//initialization of PixelFormatDescriptor
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

	
	//get dc
	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFILE, "GetDC Failed\n");
		return(-1);
	}

	//choosing pixel format which closlely matches to our initialised PFD
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &PFD);

	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFILE, "Choose PixelFormat function() failed");
		return(-2);
	}

	//set obtain PixelFormat
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &PFD) == FALSE)
	{
		fprintf(gpFILE, "SetPixelFormat\n");
		return(-3);
	}
	
	//create opengl context from device context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFILE, "wglCreateContext() failed");
		return(-4);
	}
	
	//make rendering context current
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFILE, "wglMakeCurrent() Fialed\n");
		return(-5);
	}

	//initialise GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFILE, "glewInit() failed\n");
		return(-6);
	}

	//printglinfo
	printgGLInfo();

	//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
									"#version 460 core"\
									"\n"\
									"in vec2 aPosition;"\
									"void main(void)"\
									"{"\
										"gl_Position = vec4(aPosition, 0.0, 1.0);"\
									"}"; 
    
	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	glCompileShader(vertexShaderObject);

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szinfolog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog =(GLchar*)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, GL_INFO_LOG_LENGTH, NULL, szinfolog);
				fprintf(gpFILE, "VertexShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}		
		uninitialize();
	}

	//tesselation control shader 
	const GLchar* tesselationControlShaderSourceCode = 
											"#version 460 core" \
											"\n" \
											"layout(vertices=4) out;" \
											"uniform int uNumberOfSegments;"\
											"uniform int uNumberOfStrips;"\
											"void main(void)" \
											"{" \
											"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;"\
											"gl_TessLevelOuter[0] = float(uNumberOfStrips);"\
											"gl_TessLevelOuter[1] = float(uNumberOfSegments);"										
											"}";

	GLuint tesselationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

	glShaderSource(tesselationControlShaderObject, 1, (const GLchar**)&tesselationControlShaderSourceCode, NULL);

	glCompileShader(tesselationControlShaderObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetShaderiv(tesselationControlShaderObject ,GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(tesselationControlShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog =(GLchar*)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetShaderInfoLog(tesselationControlShaderObject, GL_INFO_LOG_LENGTH, NULL, szinfolog);
				fprintf(gpFILE, "TesselationCotrol Shader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}		
		uninitialize();
	}	

	//tesselation evaluation shader 
	const GLchar* tesselationEvaluationShaderSourceCode = 
											"#version 460 core"\
											"\n"\
											"layout(isolines) in;"\
											"uniform mat4 uMVPMatrix;"\
											"void main(void)"\
											"{" \
												"vec3 P0 = gl_in[0].gl_Position.xyz;"\
												"vec3 P1 = gl_in[1].gl_Position.xyz;"\
												"vec3 P2 = gl_in[2].gl_Position.xyz;"\
												"vec3 P3 = gl_in[3].gl_Position.xyz;"\
												"vec3 P = P0 * (1 - gl_TessCoord.x) * (1 - gl_TessCoord.x) * (1 - gl_TessCoord.x) + P1 * 3.0 * gl_TessCoord.x * (1 -  gl_TessCoord.x) * (1 -  gl_TessCoord.x) + P2 * 3.0 * gl_TessCoord.x * gl_TessCoord.x * (1 -  gl_TessCoord.x) + P3 * gl_TessCoord.x * gl_TessCoord.x * gl_TessCoord.x ;"\
												"gl_Position = uMVPMatrix * vec4(P, 1.0);"\
											"}";

	GLuint tesselationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

	glShaderSource(tesselationEvaluationShaderObject, 1, (const GLchar**)&tesselationEvaluationShaderSourceCode, NULL);

	glCompileShader(tesselationEvaluationShaderObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetShaderiv(tesselationEvaluationShaderObject ,GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(tesselationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog =(GLchar*)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetShaderInfoLog(tesselationEvaluationShaderObject, GL_INFO_LOG_LENGTH, NULL, szinfolog);
				fprintf(gpFILE, "TesselationEvaluation Shader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}		
		uninitialize();
	}	

	//fragment shader 
	const GLchar* fragmentShaderSourceCode = 
											"#version 460 core"\
											"\n"\
											"uniform vec4 uLineColor;"\
											"out vec4 FragColor;"\
											"void main(void)"\
											"{" \
												"FragColor= uLineColor;" \
											"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetShaderiv(fragmentShaderObject ,GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog =(GLchar*)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, GL_INFO_LOG_LENGTH, NULL, szinfolog);
				fprintf(gpFILE, "FragmentShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}		
		uninitialize();
	}	

	//shader program
	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, tesselationControlShaderObject);
	glAttachShader(shaderProgramObject, tesselationEvaluationShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");

	glLinkProgram(shaderProgramObject);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog =(GLchar*)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szinfolog);
				fprintf(gpFILE, "Shader Program linking error log: %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}			
		}	
		uninitialize();
	}
	
	//get shader uniform locations
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
	numberofSegmentsUniform = glGetUniformLocation(shaderProgramObject, "uNumberOfSegments");
	numberofStripsUniform = glGetUniformLocation(shaderProgramObject, "uNumberOfStrips");
	lineColorUniform = glGetUniformLocation(shaderProgramObject, "uLineColor");

	const GLfloat point_position[] = 
									  { 
										-1.0f, -1.0f,	//3rd quadrant
										-0.5f, 1.0f,	//2nd 
										0.5f, -1.0f,	//4th
										1.0f, 1.0f		//1st
									   };

	//VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VBO for position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point_position), point_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glBindVertexArray(0);


	// Enabling Depth
	glClearDepth(1.0f);								   // Compulsory
	glEnable(GL_DEPTH_TEST);						   // Compulsory
	glDepthFunc(GL_LEQUAL);							   // Compulsory
	
	//set the clear color of window to blue
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	uiNumberOfLineSegments = 1;  //for one line
	//initialise
	perspectiveProjetionMatrix = vmath::mat4::identity();
	
	resize(WIN_WIDTH, WIN_HEIGHT);
	//here opengl starts

	return(0);
}

void printgGLInfo(void)
{
	//variable declaratios
	GLint numExtensions; 
	GLint i;

	//code
	fprintf(gpFILE, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFILE, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFILE, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFILE, "OpenGL GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFILE, "-------------------------------------------\n");
	//listing of supported extensions
	glGetIntegerv( GL_NUM_EXTENSIONS, &numExtensions);

	for ( i = 0; i < numExtensions; i++)
	{
		fprintf(gpFILE, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}	
	fprintf(gpFILE, "-------------------------------------------\n");

}

void resize(int width, int height)
{
	//code
	if (height <= 0)
		height = 1;

	//resize
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//set perspective projection matrix
	perspectiveProjetionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);;
}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);

	//transformation
	mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	mat4 modelViewProjectionMatrix = perspectiveProjetionMatrix * modelViewMatrix; 	//order is very important

	//push above mvp into vertex shaders mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glUniform1i(numberofSegmentsUniform, uiNumberOfLineSegments);
	
	glUniform1i(numberofStripsUniform, 1);

	glUniform4fv(lineColorUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f)); //yellow color

	//display changing numberpof segments in windows caption title bar
	TCHAR str[255];
	wsprintf(str, TEXT("Abhay B. Chilka [Number of line segments = %d]"), uiNumberOfLineSegments);
	SetWindowText(ghwnd, str);

	glBindVertexArray(vao);

	//tell opengl by how many vertices your one patch is created
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, 4);

	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	//code
}

void uninitialize(void)
{
	//fucntion declarations
	void ToggleFullScreen(void);

	//code
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint*)malloc(numShaders*sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachObjectARB(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
					free(pShaders);
					pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}
	
	//delete vbo of positoion
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}
	
	//delete vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	
	//if application is exiting in fullscreen
	if (gbFullscreen == TRUE)
	{
		ToggleFullScreen();
		gbFullscreen = FALSE;
	}	

	//make the hdc as current context
	if (wglGetCurrentContext() == ghrc )
	{
		wglMakeCurrent(NULL, NULL);
	}

	//Delete rendering context
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	//release the hdc
	if (ghdc)
	{
		ReleaseDC(ghwnd,ghdc);
		ghdc = NULL;
	}
	
	//DestroyWindow
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	//close the log file
		if (gpFILE)
		{
			fprintf(gpFILE,"Program Ended Successfully...\n");
			fclose(gpFILE);
			gpFILE = NULL;
		}
}
