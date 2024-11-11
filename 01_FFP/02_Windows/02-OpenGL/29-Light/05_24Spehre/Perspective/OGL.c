// Windows Header Files
#include <windows.h>     //sdk win32api
#include <stdio.h>      //file io
#include <stdlib.h>     //exit()

//OpenGL Header File
#include <GL/gl.h>

#include <GL/glu.h>

#include "OGL.h"

//macors
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//link with opengl library
#pragma comment(lib,"opengl32.lib")

#pragma	comment(lib,"glu32.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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

//opengl related global variables
HDC ghdc = NULL;
HGLRC ghrc = NULL;

//variables for lights
BOOL bLight = FALSE;

GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f} ;

GLUquadric *Quadric = NULL;

GLfloat angleforXRotation = 0.0f;
GLfloat angleforYRotation = 0.0f;
GLfloat angleforZRotation = 0.0f;
GLuint keyPress = 0;

//camera
GLfloat x_translate = 0.0f;
GLfloat y_translate = 0.0f;
GLfloat z_translate = 0.0f;

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
	gpFILE = fopen("Log.txt", "w");
	if (gpFILE == NULL)
	{
		MessageBox(NULL, TEXT("Log File Cannot Be Opend"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFILE, "Program Started Successfully\n");
	
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
	//UpdateWindow(hwnd);

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

		case 'L':
		case 'l':
			if (bLight == FALSE)
			{
				glEnable(GL_LIGHTING);
				bLight = TRUE;
			}
			else
			{
				glDisable(GL_LIGHTING);
				bLight = FALSE;
			}	
			break;

		case 'X':
		case 'x':
			keyPress = 1;
			angleforXRotation = 0.0f;  //reset
			break;

		case 'Y':
		case 'y':
			keyPress = 2;
			angleforYRotation = 0.0f;  //reset
			break;

		case 'Z':
		case 'z':
			keyPress = 3;
			angleforZRotation = 0.0f;  //reset
			break;

		case 'W':
			y_translate += 1.0f;
			break;

		case 'w':
			y_translate -= 1.0f;
			break;

		case 'D':
			x_translate += 1.0f;
			break;

		case 'd':
			x_translate -= 1.0f;
			break;

		case 'A':
			z_translate += 1.0f;
			break;

		case 'a':
			z_translate -= 1.0f;
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY :
		if (gpFILE)
		{
			fprintf(gpFILE, "x traslate = %f\n", x_translate);
			fprintf(gpFILE, "y traslate = %f\n", y_translate);
			fprintf(gpFILE, "z traslate = %f\n", z_translate);
			fprintf(gpFILE,"Prgram Ended Successfully...\n");
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
	void resize(int, int);
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

	//here opengl starts

	// Enabling Depth
	glShadeModel(GL_SMOOTH);						   // Beautification Line (optional)
	glClearDepth(1.0f);								   // Compulsory
	glEnable(GL_DEPTH_TEST);						   // Compulsory
	glDepthFunc(GL_LEQUAL);							   // Compulsory
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Beuatification (optional)

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDefuse);

	glEnable(GL_LIGHT0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//initialise quadric
	Quadric = gluNewQuadric();

	//set the clear color of window 
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f); //dark grey

	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void resize(int width, int height)
{
	//code
	if (height <= 0)
		height = 1;

	//resize
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);	
}

void display(void)
{
	//code
	//variable declarations
	GLfloat materialAmbient[4];
	GLfloat materialDefuse[4];
	GLfloat materialSpecular[4];
	GLfloat Shininess;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	gluLookAt(09.5f , 7.5f , 5.0f - -15.0f, 
			 12.5f , 7.5f , -5.0f -20.0f,
			 0.0f, 1.0f, 0.0f);

//	gluLookAt(19.5 / 2.0f, 14.0f / 2.0f, 20.0f, 19.5 / 2.0f, 14.0f / 2.0f, 0.0f, 0.0f, 1.0, 0.0f);

// ***** 1st sphere on 1st column, emerald *****
// ambient material
materialAmbient[0] = 0.0215; // r
materialAmbient[1] = 0.1745; // g
materialAmbient[2] = 0.0215; // b
materialAmbient[3] = 1.0f;   // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.07568; // r
materialDefuse[1] = 0.61424; // g
materialDefuse[2] = 0.07568; // b
materialDefuse[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.633;    // r
materialSpecular[1] = 0.727811; // g
materialSpecular[2] = 0.633;    // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.6 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(1.5f, 14.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 2nd sphere on 1st column, jade *****
// ambient material
materialAmbient[0] = 0.135;  // r
materialAmbient[1] = 0.2225; // g
materialAmbient[2] = 0.1575; // b
materialAmbient[3] = 1.0f;   // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.54; // r
materialDefuse[1] = 0.89; // g
materialDefuse[2] = 0.63; // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.316228; // r
materialSpecular[1] = 0.316228; // g
materialSpecular[2] = 0.316228; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.1 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(1.5f, 11.5, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 3rd sphere on 1st column, obsidian *****
// ambient material
materialAmbient[0] = 0.05375; // r
materialAmbient[1] = 0.05;    // g
materialAmbient[2] = 0.06625; // b
materialAmbient[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.18275; // r
materialDefuse[1] = 0.17;    // g
materialDefuse[2] = 0.22525; // b
materialDefuse[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.332741; // r
materialSpecular[1] = 0.328634; // g
materialSpecular[2] = 0.346435; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.3 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(1.5f, 9.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 4th sphere on 1st column, pearl *****
// ambient material
materialAmbient[0] = 0.25;    // r
materialAmbient[1] = 0.20725; // g
materialAmbient[2] = 0.20725; // b
materialAmbient[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 1.0;   // r
materialDefuse[1] = 0.829; // g
materialDefuse[2] = 0.829; // b
materialDefuse[3] = 1.0f;  // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.296648; // r
materialSpecular[1] = 0.296648; // g
materialSpecular[2] = 0.296648; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.088 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(1.5f, 6.5f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 5th sphere on 1st column, ruby *****
// ambient material
materialAmbient[0] = 0.1745;  // r
materialAmbient[1] = 0.01175; // g
materialAmbient[2] = 0.01175; // b
materialAmbient[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.61424; // r
materialDefuse[1] = 0.04136; // g
materialDefuse[2] = 0.04136; // b
materialDefuse[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.727811; // r
materialSpecular[1] = 0.626959; // g
materialSpecular[2] = 0.626959; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.6 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(1.5f, 4.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 6th sphere on 1st column, turquoise *****
// ambient material
materialAmbient[0] = 0.1;     // r
materialAmbient[1] = 0.18725; // g
materialAmbient[2] = 0.1745;  // b
materialAmbient[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.396;   // r
materialAmbient[1] = 0.74151; // g
materialDefuse[2] = 0.69102; // b
materialDefuse[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.297254; // r
materialSpecular[1] = 0.30829;  // g
materialSpecular[2] = 0.306678; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.1 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(1.5f, 1.5, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************
// *******************************************************
// *******************************************************

// ***** 1st sphere on 2nd column, brass *****
// ambient material
materialAmbient[0] = 0.329412; // r
materialAmbient[1] = 0.223529; // g
materialAmbient[2] = 0.027451; // b
materialAmbient[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.780392; // r
materialDefuse[1] = 0.568627; // g
materialDefuse[2] = 0.113725; // b
materialDefuse[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.992157; // r
materialAmbient[1] = 0.941176; // g
materialAmbient[2] = 0.807843; // b
materialAmbient[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.21794872 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(7.5, 14.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 2nd sphere on 2nd column, bronze *****
// ambient material
materialAmbient[0] = 0.2125; // r
materialAmbient[1] = 0.1275; // g
materialAmbient[2] = 0.054;  // b
materialAmbient[3] = 1.0f;   // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.714;   // r
materialDefuse[1] = 0.4284;  // g
materialDefuse[2] = 0.18144; // b
materialDefuse[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.393548; // r
materialSpecular[1] = 0.271906; // g
materialSpecular[2] = 0.166721; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.2 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(7.5f, 11.5f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 3rd sphere on 2nd column, chrome *****
// ambient material
materialAmbient[0] = 0.25; // r
materialAmbient[1] = 0.25; // g
materialAmbient[2] = 0.25; // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.4;  // r
materialDefuse[1] = 0.4;  // g
materialDefuse[2] = 0.4;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.774597; // r
materialSpecular[1] = 0.774597; // g
materialSpecular[2] = 0.774597; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.6 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(7.5f, 9.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 4th sphere on 2nd column, copper *****
// ambient material
materialAmbient[0] = 0.19125; // r
materialAmbient[1] = 0.0735;  // g
materialAmbient[2] = 0.0225;  // b
materialAmbient[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.7038;  // r
materialDefuse[1] = 0.27048; // g
materialDefuse[2] = 0.0828;  // b
materialDefuse[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.256777; // r
materialSpecular[1] = 0.137622; // g
materialSpecular[2] = 0.086014; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialSpecular);

// shininess
Shininess = 0.1 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(7.5, 6.5, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 5th sphere on 2nd column, gold *****
// ambient material
materialAmbient[0] = 0.24725; // r
materialAmbient[1] = 0.1995;  // g
materialAmbient[2] = 0.0745;  // b
materialAmbient[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.75164; // r
materialDefuse[1] = 0.60648; // g
materialDefuse[2] = 0.22648; // b
materialDefuse[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.628281; // r
materialSpecular[1] = 0.555802; // g
materialSpecular[2] = 0.366065; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.4 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(7.5f, 4.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 6th sphere on 2nd column, silver *****
// ambient material
materialAmbient[0] = 0.19225; // r
materialAmbient[1] = 0.19225; // g
materialAmbient[2] = 0.19225; // b
materialAmbient[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.50754; // r
materialDefuse[1] = 0.50754; // g
materialDefuse[2] = 0.50754; // b
materialDefuse[3] = 1.0f;    // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.508273; // r
materialSpecular[1] = 0.508273; // g
materialSpecular[2] = 0.508273; // b
materialSpecular[3] = 1.0f;     // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.4 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(7.5f, 1.5f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************
// *******************************************************
// *******************************************************

// ***** 1st sphere on 3rd column, black *****
// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.01; // r
materialDefuse[1] = 0.01; // g
materialDefuse[2] = 0.01; // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.50; // r
materialSpecular[1] = 0.50; // g
materialSpecular[2] = 0.50; // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.25 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(13.5f, 14.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 2nd sphere on 3rd column, cyan *****
// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.1;  // g
materialAmbient[2] = 0.06; // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.0;        // r
materialDefuse[1] = 0.50980392; // g
materialDefuse[2] = 0.50980392; // b
materialDefuse[3] = 1.0f;       // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.50196078; // r
materialSpecular[1] = 0.50196078; // g
materialSpecular[2] = 0.50196078; // b
materialSpecular[3] = 1.0f;       // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.25 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(13.5f, 11.5f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 3rd sphere on 2nd column, green *****
// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.1;  // r
materialDefuse[1] = 0.35; // g
materialDefuse[2] = 0.1;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.45; // r
materialSpecular[1] = 0.55; // g
materialSpecular[2] = 0.45; // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.25 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(13.5f, 9.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 4th sphere on 3rd column, red *****
// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.5;  // r
materialDefuse[1] = 0.0;  // g
materialDefuse[2] = 0.0;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.7;  // r
materialSpecular[1] = 0.6;  // g
materialSpecular[2] = 0.6;  // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.25 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(13.5f, 6.5f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 5th sphere on 3rd column, white *****
// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.55; // r
materialDefuse[1] = 0.55; // g
materialDefuse[2] = 0.55; // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.70; // r
materialSpecular[1] = 0.70; // g
materialSpecular[2] = 0.70; // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.25 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(13.5f, 4.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 6th sphere on 3rd column, yellow plastic *****
// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.5;  // r
materialDefuse[1] = 0.5;  // g
materialDefuse[2] = 0.0;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.60; // r
materialSpecular[1] = 0.60; // g
materialSpecular[2] = 0.50; // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.25 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(13.5f, 1.5f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************
// *******************************************************
// *******************************************************

// ***** 1st sphere on 4th column, black *****
// ambient material
materialAmbient[0] = 0.02; // r
materialAmbient[1] = 0.02; // g
materialAmbient[2] = 0.02; // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.01; // r
materialDefuse[1] = 0.01; // g
materialDefuse[2] = 0.01; // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.4;  // r
materialSpecular[1] = 0.4;  // g
materialSpecular[2] = 0.4;  // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.078125 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(19.5, 14.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 2nd sphere on 4th column, cyan *****
// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.05; // g
materialAmbient[2] = 0.05; // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.4;  // r
materialDefuse[1] = 0.5;  // g
materialDefuse[2] = 0.5;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.04; // r
materialSpecular[1] = 0.7;  // g
materialSpecular[2] = 0.7;  // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.078125 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(19.5f, 11.5f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 3rd sphere on 4th column, green *****
// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.05; // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.4;  // r
materialDefuse[1] = 0.5;  // g
materialDefuse[2] = 0.4;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.04; // r
materialSpecular[1] = 0.7;  // g
materialSpecular[2] = 0.04; // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.078125 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(19.5f, 9.0, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 4th sphere on 4th column, red *****
// ambient material
materialAmbient[0] = 0.05; // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.5;  // r
materialDefuse[1] = 0.4;  // g
materialDefuse[2] = 0.4;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.7;  // r
materialSpecular[1] = 0.04; // g
materialSpecular[2] = 0.04; // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.078125 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(19.5f, 6.5f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 5th sphere on 4th column, white *****
// ambient material
materialAmbient[0] = 0.05; // r
materialAmbient[1] = 0.05; // g
materialAmbient[2] = 0.05; // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.5;  // r
materialDefuse[1] = 0.5;  // g
materialDefuse[2] = 0.5;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.7;  // r
materialSpecular[1] = 0.7;  // g
materialSpecular[2] = 0.7;  // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.078125 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(19.5, 4.0f, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************

// ***** 6th sphere on 4th column, yellow rubber *****
// ambient material
materialAmbient[0] = 0.05; // r
materialAmbient[1] = 0.05; // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

// diffuse material
materialDefuse[0] = 0.5;  // r
materialDefuse[1] = 0.5;  // g
materialDefuse[2] = 0.4;  // b
materialDefuse[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDefuse);

// specular material
materialSpecular[0] = 0.7;  // r
materialSpecular[1] = 0.7;  // g
materialSpecular[2] = 0.04; // b
materialSpecular[3] = 1.0f; // a
glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

// shininess
Shininess = 0.078125 * 128;
glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

// geometry
glPushMatrix();
glTranslatef(19.5f, 1.5, 0.0f);
gluSphere(Quadric, 1.0f, 30, 30);
glPopMatrix();
// *******************************************************
// *******************************************************
// *******************************************************



//creates all internally normals 
//	gluSphere(Quadric, 0.2f, 50, 50);
//animation
if (keyPress == 1)
{
	glRotatef(angleforXRotation, 1.0f, 0.0f, 0.0f);
	lightPosition[2] = angleforXRotation; 
}

if (keyPress == 2)
{
	glRotatef(angleforYRotation, 0.0f, 1.0f, 0.0f);
	lightPosition[0] = angleforYRotation;
}

if (keyPress == 3)
{
	glRotatef(angleforZRotation, 0.0f, 0.0f, 1.0f);
	lightPosition[1] = angleforZRotation;
}

	SwapBuffers(ghdc);
}

void update(void)
{
	//code
if (keyPress == 1)
    {
        angleforXRotation = angleforXRotation + 0.5f;
        if (angleforXRotation >= 360.0f)
        {
            angleforXRotation = angleforXRotation - 360.0f;
        }
        
    }	

if (keyPress == 2)
    {
        angleforYRotation = angleforYRotation + 0.5f;
        if (angleforYRotation >= 360.0f)
        {
            angleforYRotation = angleforYRotation - 360.0f;
        }
        
    }	

if (keyPress == 3)
    {
        angleforZRotation = angleforZRotation + 0.5f;
        if (angleforZRotation >= 360.0f)
        {
            angleforZRotation = angleforZRotation - 360.0f;
        }
        
    }	

}

void uninitialize(void)
{
	//fucntion declarations
	void ToggleFullScreen(void);

	//code
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

	//delete quadric
	if (Quadric)
	{
		gluDeleteQuadric(Quadric);
		Quadric = NULL;
	}
}

