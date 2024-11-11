// Windows Header Files
#include <windows.h>     //sdk win32api
#include <stdio.h>      //file io
#include <stdlib.h>     //exit()

#define _USE_MATH_DEFINES
#include <math.h>

#include<mmsystem.h>

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

#pragma comment(lib, "winmm.lib")

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0])) 

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declarations
GLfloat lerp(GLfloat start, GLfloat end, GLfloat t)
{
	return (start + (end - start) * t);
}

//b
float b_x = -2.5f;
//float b_y = 0.0f;
//h
float h_x = -1.6f;
float h_y = 0.0f;
//a
float a_x = -0.7f;
float a_y = 0.0f;
//r
float r_x = 0.2f;
float r_y = 0.0f;
//a
float a2_x = 1.10f;
float a2_y = 0.f;
//t
float t_x = 2.3f;
//float t_y = 0.0f;
//jets
float  jetmidx = -8.0f;
float j1x = -8.0f;
float j1y = 1.0f;
float j3x = -8.0f;
float j3y = -1.0f;
//flags 
float flagb = -2.5f;
float flagh = -1.6f;
float flaga = -0.7f;
float flagr = 0.2f;
float flaga1 = 1.10f;
float flagt = 2.2f;


GLfloat b = 0.0f;
GLfloat h = 0.0f;
GLfloat a = 0.0f;
GLfloat r = 0.0f;
GLfloat a_1 = 0.0f;
GLfloat t = 0.0f;
GLfloat j = 0.0f;
GLfloat j_1 = 0.0f;
GLfloat j_3 = 0.0f;
GLfloat j_all = 0.0f;
GLfloat j_out = 0.0f;
GLfloat angle = -90.0f;
GLfloat tangle = 0.0f;

//for colors

// for fileIO
FILE *gpFILE = NULL;   

//for fullscreen
HWND ghwnd = NULL;  
DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen = FALSE;
BOOL   bStartAnimation;

//for active/inactive
BOOL gbActiveWindow = FALSE;

//opengl related global variables
HDC ghdc = NULL;
HGLRC ghrc = NULL;

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//fucntion declaration
	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);
	void B();
	void H();
	void A();
	void R();
	void T();
	void Jet();
	void B_to_A();
	void flag_T();
	void Fog1();
	void Fog2();
	void Fog3();

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
	    case 's':
        case 'S':
            if (bStartAnimation == FALSE)
            {
                bStartAnimation = TRUE;
                PlaySound("Anamikas Anamveera 1963.wav", NULL, SND_ASYNC| SND_FILENAME);
            }
            break;

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

	//set the clear color of window to blue
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
//	glTranslatef(-2.5f, 0.0f, -5.0f);
	glTranslatef(b_x, 0.0f, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	B();
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
//	glTranslatef(-1.6f, 0.0f, -5.0f);
	glTranslatef(h_x, h_y, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	H();
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
//	glTranslatef(-0.7f, 0.0f, -5.0f);
	glTranslatef(a_x, a_y, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	A();
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
//	glTranslatef(0.2f, 0.0f, -5.0f);
	glTranslatef(r_x, r_y, -5.0);
	glScalef(1.3, 1.3, 0.0);
	R();
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
//	glTranslatef(1.10f, 0.0f, -5.0f);
	glTranslatef(a2_x, a2_y, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	A();
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
//	glTranslatef(2.3f, 0.0f, -5.0f);
	glTranslatef(t_x, 0.0f, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	T();
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(flagb, 0.0f, -5.0f);	
	glScalef(1.3, 1.3, 0.0);
	if (jetmidx >= -2.2f)
	{
		B_to_A();		
	}
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(flagh, 0.0f, -5.0f);	
	glScalef(1.3, 1.3, 0.0);
	if (jetmidx >= -1.25f)
	{
		B_to_A();		
	}
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(flaga, 0.0f, -5.0f);	
	glScalef(1.3, 1.3, 0.0);
	if (jetmidx >= -0.2f)
	{
		B_to_A();		
	}
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(flagr, 0.0f, -5.0f);	
	glScalef(1.3, 1.3, 0.0);
	if (jetmidx >= 0.7f)
	{
		B_to_A();		
	}
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(flaga1, 0.0f, -5.0f);	
	glScalef(1.3, 1.3, 0.0);
	if (jetmidx >= 1.6f)
	{
		B_to_A();		
	}
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(flagt, 0.0f, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	if (jetmidx >= 2.5f)
	{
		flag_T();		
	}
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(j1x, j1y, -5.0f);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glScalef(0.5, 0.5, 0.0);
	Jet();
	glRotatef(tangle, 1.0f, 0.0f, 0.0f);
	Fog1();
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(jetmidx, 0.0f, -5.0f);
	glScalef(0.5, 0.5, 0.0);
	Jet();
	glRotatef(tangle, 1.0f, 0.0f, 0.0f);
	Fog2();
//	glLoadIdentity();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(j3x, j3y, -5.0f);
	glRotatef(-angle, 0.0f, 0.0f, 1.0f);
	glScalef(0.5, 0.5, 0.0);
	Jet();
	glRotatef(tangle, 1.0f, 0.0f, 0.0f);
	Fog3();
	glPopMatrix();
//	glLoadIdentity();

	SwapBuffers(ghdc);
}

void update(void)
{
	//code
	if(b <= 1.0f)
	{
		b += 0.00015f;
	
		b_x = lerp(-4.0f, -2.5f,b);
	//	b_y = lerp(-3.5, -2.5f, b);
	}

	if (h <= 1.0f)
	{
		h += 0.00015f;
		h_x = lerp(-4.0f, -1.6f,h);
		h_y = lerp(4.0f, 0.0f,h);
	}

	if (a <= 1.0f)
	{
		a += 0.00015f;
		a_x = lerp(-4.0f,-0.7f, a);
		a_y = lerp(-4.0f, 0.0f, a);
	}
	
	if (r <= 1.0f)
	{
		r += 0.00015f;
		r_x = lerp(4.0f, 0.2f, r);
		r_y =lerp(4.0f, 0.0f, r);
	}

	if (a_1 <= 1.0f)
	{
		a_1 += 0.00015f;
		a2_x = lerp(4.0f, 1.10f, a_1);
		a2_y = lerp(-4.0f, 0.0f, a_1);
	}
	
	if (t <= 1.0f)
	{
		t += 0.00015f;
		t_x = lerp(4.0f ,2.2f, t);
	}
	else if(j  <= 1.0f )
	{
		j += 0.00010f;
		jetmidx = lerp(-4.0f ,-2.5f, j);

		j1x = lerp(-4.0f, -2.5f, j);
		j1y = lerp(2.0f, 0.02f, j);

		angle = lerp(-90.0f, 0.0f, j);
		j3x = lerp(-4.0f, -2.5f, j);
		j3y = lerp(-2.0f, -0.02f, j);			
	}		  
	else if (j_all <= 1.0f)
	{
		j_all += 0.00005f;
		jetmidx = lerp(-2.5f ,2.5f, j_all);
		j1x = lerp(-2.5f, 2.5f, j_all);
		j3x = lerp(-2.5f, 2.5f, j_all);	 		
	}
	else if (j_out <= 1.0f)
	{
		j_out += 0.00010f;
		jetmidx = lerp(2.5f ,4.5f, j_out);

		j1x = lerp(2.5f, 4.5f, j_out);
		j1y = lerp(0.0f, 4.0f, j_out);

		angle = lerp(0.0f, 90.0f, j_out);
		j3x = lerp(2.5f, 4.5f, j_out);
		j3y = lerp(0.0f, -4.0f, j_out);	
	}

	tangle = tangle + 1.0f;
	if (tangle >= 360.0f)
	{
		tangle = 0.0f;
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
}

void B()
{
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2f(-0.10,0.6f);
	glVertex2f(0.01,0.5f);
	glVertex2f(-0.1,0.5f);
	glVertex2f(-0.20,0.6f);

	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(-0.1f, -0.5f);

	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.05f, 0.55f);
	glVertex2f(0.5f,0.45f);
	glVertex2f(0.4f, 0.35f);

	glVertex2f(0.5f,0.45f);
	glVertex2f(0.4f, 0.35f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(0.5f, 0.05f);

	glVertex2f(0.5f, 0.05f);
	glVertex2f(0.5f, -0.05f);
	glVertex2f(0.01f, -0.05f);
	glVertex2f(0.01f, 0.05f);
	
	glVertex2f(0.25f, -0.05f);
	glVertex2f(0.35f, -0.05f);
	glVertex2f(0.45f, -0.2f);
	glVertex2f(0.35f, -0.2f);

	glVertex2f(0.45f, -0.2f);
	glVertex2f(0.35f, -0.2f);
	glVertex2f(0.35f, -0.4f);
	glVertex2f(0.45f, -0.4f);

	glVertex2f(0.35f, -0.35f);
	glVertex2f(0.45f, -0.4f);
	glVertex2f(0.01f,-0.55f);
	glVertex2f(-0.1f, -0.5f);
	glEnd();	
}

void H()
{
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2f(-0.10,0.6f);
	glVertex2f(0.01,0.5f);
	glVertex2f(-0.1,0.5f);
	glVertex2f(-0.20,0.6f);

	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(-0.1f, -0.5f);

	glVertex2f(-0.1f, -0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(0.10f, -0.6f);
	glVertex2f(0.01f,-0.6f);

	glVertex2f(0.4f, 0.5f);
	glVertex2f(0.5f,0.5f);
	glVertex2f(0.4f, 0.6f);
	glVertex2f(0.3f,0.6f);

	glVertex2f(0.4f, 0.5f);
	glVertex2f(0.5f, 0.5f);
	glVertex2f(0.5f,-0.5f);
	glVertex2f(0.4f, -0.5f);

	glVertex2f(0.4f, -0.5f);
	glVertex2f(0.5f,-0.5f);
	glVertex2f(0.6f, -0.6f);
	glVertex2f(0.5f,-0.6f);

	glVertex2f(0.5f, -0.05f);
	glVertex2f(0.5f, 0.05f);
	glVertex2f(0.01f, 0.05f);
	glVertex2f(0.01f, -0.05f);

	glEnd();
}

void A()
{
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2f(-0.10,0.6f);
	glVertex2f(0.01,0.5f);
	glVertex2f(-0.1,0.5f);
	glVertex2f(-0.20,0.6f);

	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(-0.1f, -0.5f);

	glVertex2f(-0.1f, -0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(0.10f, -0.6f);
	glVertex2f(0.01f,-0.6f);

	glVertex2f(0.4f, 0.45f);
	glVertex2f(0.5f, 0.45f);
	glVertex2f(0.5f,-0.5f);
	glVertex2f(0.4f, -0.5f);

	glVertex2f(0.4f, -0.5f);
	glVertex2f(0.5f,-0.5f);
	glVertex2f(0.6f, -0.6f);
	glVertex2f(0.5f,-0.6f);

	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.05f, 0.55f);
	glVertex2f(0.5f,0.45f);
	glVertex2f(0.4f, 0.35f);

	glVertex2f(0.5f, -0.05f);
	glVertex2f(0.5f, 0.05f);
	glVertex2f(0.01f, 0.05f);
	glVertex2f(0.01f, -0.05f);

	glEnd();
}

void R()
{
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2f(-0.10,0.6f);
	glVertex2f(0.01,0.5f);
	glVertex2f(-0.1,0.5f);
	glVertex2f(-0.20,0.6f);

	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(-0.1f, -0.5f);

	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.05f, 0.55f);
	glVertex2f(0.5f,0.45f);
	glVertex2f(0.4f, 0.35f);

	glVertex2f(0.5f,0.45f);
	glVertex2f(0.4f, 0.35f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(0.5f, 0.05f);

	glVertex2f(0.5f, 0.05f);
	glVertex2f(0.5f, -0.05f);
	glVertex2f(0.01f, -0.05f);
	glVertex2f(0.01f, 0.05f);
	
	glVertex2f(0.15f, -0.05f);
	glVertex2f(0.25f, -0.05f);
	glVertex2f(0.45f, -0.2f);
	glVertex2f(0.35f, -0.2f);

	glVertex2f(0.45f, -0.2f);
	glVertex2f(0.35f, -0.2f);
	glVertex2f(0.35f, -0.5f);
	glVertex2f(0.45f, -0.5f);

	glVertex2f(-0.1f, -0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(0.10f, -0.6f);
	glVertex2f(0.01f,-0.6f);

	glVertex2f(0.35f, -0.5f);
	glVertex2f(0.45f,-0.5f);
	glVertex2f(0.55f, -0.6f);
	glVertex2f(0.45f,-0.6f);

	glEnd();
}

void T()
{
	glBegin(GL_QUADS);	
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2f(-0.05f, 0.5f);
	glVertex2f(0.1f, 0.45f);
	glVertex2f(0.3f,0.5f);
	glVertex2f(0.1f, 0.55f);

	glVertex2f(-0.05f, 0.5f);
	glVertex2f(-0.15f, 0.45f);
	glVertex2f(-0.35f,0.5f);
	glVertex2f(-0.15f, 0.55f);

	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(-0.1f, -0.5f);

	glVertex2f(-0.1f, -0.5f);
	glVertex2f(0.01f,-0.5f);
	glVertex2f(0.10f, -0.6f);
	glVertex2f(0.01f,-0.6f);
	glEnd();
}

void Jet()
{
	glBegin(GL_POLYGON);
	glColor3f(0.608f, 0.608f, 0.722f);
	float x = 0.8f;
	float y = 0.1f;
	for (int i = 0; i < 100; i++)
	{
		float angle = 2.0f * M_PI * i / 100;
		glVertex2f(x * cos(angle), y * sin(angle));
	}
	glEnd(); 

	glBegin(GL_POLYGON);
	glColor3f(1.5, 1.5, 1.5);
	float x1 = 0.03f;
	float y1 = 0.05f;
	float r = 0.6f;
	for (int i = 0; i < 100; i++)
	{
		float angle = 2.0f * M_PI * i / 100;
		glVertex2f(x1 * cos(angle)+ r, y1 * sin(angle));
	}
	glEnd(); 

	glBegin(GL_QUADS);
	glColor3f(0.373, 0.373, 0.6);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(-0.5f, 0.5f);
	glVertex2f(-0.5f, 0.5f);
	glVertex2f(-0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.373, 0.373, 0.6);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(-0.5f, -0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.58);
	glVertex2f(0.5f, 0.0f);
	glVertex2f(0.0f, 0.25f);
	glVertex2f(-0.5f, 0.25f);
	glVertex2f(-0.5f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2f(0.5f, 0.0f);
	glVertex2f(0.0f, -0.25f);
	glVertex2f(-0.5f, -0.25f);
	glVertex2f(-0.5f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glLineWidth(5.0f);
	glColor3f(1, 0, 0);
	glVertex2f(0.5f, 0.0f);
	glVertex2f(0.0f, 0.25f);
	glVertex2f(-0.5f, 0.25f);
	glVertex2f(-0.5f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex2f(0.5f, 0.0f);
	glVertex2f(0.0f, -0.25f);
	glVertex2f(-0.5f, -0.25f);
	glVertex2f(-0.5f, 0.0f);
	glEnd();
}

void B_to_A()
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(-0.1f, 0.05f);
	glVertex2f(0.5f, 0.05f);
	glVertex2f(0.5f, 0.01f);
	glVertex2f(-0.1f, 0.01f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.1f, 0.01f);
	glVertex2f(0.5f, 0.01f);
	glVertex2f(0.5f, -0.02f);
	glVertex2f(-0.1f, -0.02f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.1f, -0.02f);
	glVertex2f(0.5f, -0.02f);
	glVertex2f(0.5f, -0.05f);
	glVertex2f(-0.1f, -0.05f);
	glEnd();
}

void flag_T()
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(-0.1f, 0.05f);
	glVertex2f(0.01f, 0.05f);
	glVertex2f(0.01f, 0.01f);
	glVertex2f(-0.1f, 0.01f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.1f, 0.01f);
	glVertex2f(0.01f, 0.01f);
	glVertex2f(0.01f, -0.02f);
	glVertex2f(-0.1f, -0.02f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.1f, -0.02f);
	glVertex2f(0.01f, -0.02f);
	glVertex2f(0.01f, -0.05f);
	glVertex2f(-0.1f, -0.05f);
	glEnd();
}

void Fog1()
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(-0.5f,-0.05f);
	glVertex2f(-0.5f,0.05f);
	glVertex2f(-1.5f, 0.1f);
	glVertex2f(-1.5f, -0.1f);	
	glEnd();
}

void Fog2()
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.5f,-0.05f);
	glVertex2f(-0.5f,0.05f);
	glVertex2f(-1.5f, 0.1f);
	glVertex2f(-1.5f, -0.1f);
	glEnd();
}

void Fog3()
{
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.5f,-0.05f);
	glVertex2f(-0.5f,0.05f);
	glVertex2f(-1.5f, 0.1f);
	glVertex2f(-1.5f, -0.1f);
	glEnd();
}