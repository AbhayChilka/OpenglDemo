// Windows Header Files
#include <windows.h>     //sdk win32api
#include <stdio.h>      //file io
#include <stdlib.h>     //exit()
#include <time.h>

#include "sll.h"

#define _USE_MATH_DEFINES 
#include<math.h>

#include<mmsystem.h>

//OpenGL Header File
#include <GL/gl.h>

#include <GL/glu.h>

#include "OGL.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glcorearb.h"

//macors
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//link with opengl library
#pragma comment(lib,"opengl32.lib")

#pragma	comment(lib,"glu32.lib")

#pragma comment(lib, "winmm.lib")

// Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GLfloat lerp(GLfloat start, GLfloat end, GLfloat t)
{
	return (start + (end - start) * t);
}

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

//font 
GLuint font;

TCHAR str[600];

GLYPHMETRICSFLOAT gmf[256];

GLuint texture_sky;
GLuint texture_grass;
GLuint texture_pnggrass;
GLuint texture_bush;
GLuint texture_plant;
GLuint texture_swing;

GLuint texture_floorone;
GLuint texture_floortwo;
GLuint texture_chat;
GLuint texture_wall;
GLuint texture_tiles;
GLuint texture_brown;

GLUquadric *quadric = NULL;

GLuint texture_sky;
GLuint texture_sky1;
GLuint texture_sky2;
GLuint texture_feather;
GLuint texture_body;
GLuint texture_wing;

GLuint texture_crowfeather;
GLuint texture_crowbody;

GLuint texture_peacockfeather;
GLuint texture_peacockbody;
GLuint texture_featherone;

GLuint texture_joker;

GLfloat angle =  -180.0f ;

GLfloat skyx = 0.0f;
float s = 0.0f;

GLfloat peacock_entryy;
GLfloat peacock_entryx = 5.0f;
GLfloat peacock_outx;
GLfloat peacock_outy;
GLfloat peacock_inframex = 5.0f;
GLfloat peacock_inframey;
float peacocktravel = 0.0f;

GLfloat crow_entryx = 4.0f;
GLfloat crow_entryy;
GLfloat crow_outx;
GLfloat crow_outy;
GLfloat crow_inframex = 4.0f;
GLfloat crow_inframey;
float crowtravel = 0.0f;

//seens
#define MAIN_SCENE 1 
#define CROW_SCENE 2
#define PEACOCK_SCENE 3
#define CROW_MAIN_SCENE 4
#define PEACOCK_MAIN_SCENE 5
#define MAIN_SCENETWO 6 
#define MAIN_SCENETHREE 7

//for time
int Global_time = 0;
GLfloat fTimer = 0.0f;
SYSTEMTIME  timer_in_Min;
int iCurrent_Time;

int Present_Scene = MAIN_SCENE;

sll_t* function_list;

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

	PlaySound("song.wav", NULL, SND_ASYNC | SND_FILENAME);

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
	void sky(void);
	void grass(void);
	void grasspng(void);
	void house(void);
	void floor2(void);
	void step(void);
	void bush(void);
	void plant(void);
	void swing(void);

	void resize(int, int);
	GLuint createTexture2D(const char *);
	GLuint BuildFont(char *name, int fontSize, float depth);
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
	
	// Enabling Depth
	glShadeModel(GL_SMOOTH);						   // Beautification Line (optional)
	glClearDepth(1.0f);								   // Compulsory
	glEnable(GL_DEPTH_TEST);						   // Compulsory
	glDepthFunc(GL_LEQUAL);							   // Compulsory
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Beuatification (optional)

	texture_sky = createTexture2D("sky.jpg");
	texture_grass = createTexture2D("grass.png");
	texture_pnggrass = createTexture2D("tree.png");
	texture_floorone = createTexture2D("floor1.png");
	texture_floortwo = createTexture2D("floor2.png");
	texture_chat = createTexture2D("chat.png");
	texture_wall = createTexture2D("wall.png");
	texture_tiles = createTexture2D("tiles.png");
	texture_brown = createTexture2D("brown.png");
    texture_bush = createTexture2D("bush.png");
	texture_plant = createTexture2D("plant.png");
	texture_swing = createTexture2D("swing.png");

	texture_sky = createTexture2D("sky.jpg");
	texture_sky1 = createTexture2D("2.png");
	texture_sky2 = createTexture2D("sky2.png");

	texture_crowfeather = createTexture2D("crowwing.png");
	texture_crowbody = createTexture2D("c.png");

	texture_peacockfeather = createTexture2D("peac.bmp");
	texture_peacockbody = createTexture2D("p.bmp");
	texture_featherone = createTexture2D("feather.png");

	texture_joker = createTexture2D("jjjoker.png");

	//initialize quadric
	quadric = gluNewQuadric();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glEnable(GL_TEXTURE_2D);

	GetSystemTime(&timer_in_Min);

	//set the clear color of window to blue
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	fprintf(gpFILE, "texture\n");

	font = BuildFont("AMS Calligraphy 2", 50, 0.1f);

	//
	function_list = create_list(); 
	insert_end(function_list, sky);
	insert_end(function_list, grasspng);
	insert_end(function_list, grass);
	insert_end(function_list, floor2);
	insert_end(function_list, house);
	insert_end(function_list, step);
	insert_end(function_list, bush);
	insert_end(function_list, plant);
	insert_end(function_list, swing);

	resize(WIN_WIDTH, WIN_HEIGHT);
	//here opengl starts

	return(0);
}

GLuint BuildFont(char *name, int fontSize, float depth)
{
	HFONT font;
	HFONT oldFont;
	GLuint base = glGenLists(128);

	font = CreateFont(fontSize, 
						0,
						0,
						0,
						FW_BOLD,
						FALSE,
						FALSE,
						FALSE,
						ANSI_CHARSET,
						OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY,
						FF_DONTCARE |DEFAULT_PITCH,
						(TCHAR *)name);

//	oldFont = (HFONT)SelectObject(ghdc, font);

	if(! font)
	 return 0;

	SelectObject(ghdc, font);
	wglUseFontOutlines(ghdc, 0 ,255, base, 0.0, depth, WGL_FONT_POLYGONS, gmf);

	return base;
}

GLuint createTexture2D(const char *filePath)
{
	int width, height, channel;
	stbi_set_flip_vertically_on_load(TRUE);

	unsigned char *data = stbi_load(filePath, &width, &height, &channel, 0);

	if (data == NULL)
	{
		fprintf(gpFILE, "Failed To Load Txture %s\n", filePath);
		return -1;
	}

	GLenum format = GL_RGBA;

	if (channel == STBI_grey)
		format = GL_RED;
	else if (channel == STBI_rgb)
		format = GL_RGB;
	else if (channel == STBI_rgb_alpha)
		format = GL_RGBA;

	GLuint texture;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	return texture;
}

int TimeCal(SYSTEMTIME currentTime, SYSTEMTIME StartTime)
{
	int CurrentSeconds = currentTime.wMinute * 60 + currentTime.wSecond;

	int StartSeconds = StartTime.wMinute * 60 + StartTime.wSecond;

	return CurrentSeconds - StartSeconds;
}

void glPrint(GLuint base, const char* str, ...)
{
	float length = 0;
	char text[256];
	va_list ap;

	if (str == NULL)
		return;

	va_start(ap, str);
		vsprintf(text, str, ap);
	va_end(ap);

	for (GLuint i = 0; i < strlen((text)); i++)
	{
		length += gmf[text[i]].gmfCellIncX;
	}

	glTranslatef(-length / 2, 0.0f, 0.0f);	
	glPushAttrib(GL_LIST_BIT);
	glListBase(base);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();	
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
	node_t *fun_run = NULL;

	void skybg(void);
	void sky1(void);
	void sky2(void);

	void crow(void);
	void feather(void);

	void glass(void);
	void plate(void);

	void peacock(void);

	void baby(void);

	void Circle(void);
	void name(void);
	void joker(void);
	void DrawFloor(void);

	SYSTEMTIME CurrentTime;

	GetSystemTime(&CurrentTime);

	Global_time = TimeCal(CurrentTime, timer_in_Min);

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	switch (Present_Scene)
	{
	case MAIN_SCENE:
		if (Global_time <= 15)
		{   
			fun_run = function_list->next;
			glPushMatrix();
			glTranslatef(0.0f, 0.5f, -10.0f);
			glScalef(10.0f, 5.0f, 1.0f);
		//	sky();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-2.5f, 0.5f, -5.0f);
			glScalef(1.0f, 1.5f, 0.0f);
//			grasspng();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -1.8f, -6.5f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glScalef(10.0f, 5.0f, 1.0f);
//			grass();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, 0.35f, -6.5f);
			glScalef(1.0, 0.3, 1.0);
//			floor2();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -0.5f, -6.5f);
			glScalef(1.5f, 0.6, 1.0);
//			house();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -1.3f, -6.5f);
			glScalef(1.2f, 0.2, 1.0);
//			step();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(1.8f, -0.6f, -5.0f);
			glScalef(0.4f, 0.4f, 0.0f);
//			bush();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(2.7f, -0.7f, -5.0f);
			glScalef(0.6f, 0.3f, 0.0f);
//			plant();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-2.6f, -0.7f, -5.0f);
			glScalef(1.0f, 0.5f, 0.0f);
//			swing();
			fun_run->data();
			fun_run = fun_run->next;
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -1.5f, -5.0f);
			glScalef(0.5f, 0.5f, 0.0f);
			glass();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.2f, -2.0f, -1.0f);
			plate();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -0.7f, -1.0f);
			glScalef(0.5f, 0.5, 1.0);
			baby();
			glPopMatrix();

		}
		else
		{
			Present_Scene = CROW_SCENE;
		}
		break;

	case CROW_SCENE:
		if (Global_time <= 30)
		{
			glPushMatrix();
			glTranslatef(0.0f, 0.0f, -3.0f);
			glScalef(2.5f, 2.5f, 1.0f);
			skybg();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 4.5, 0.5f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 2.5, 0.5f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 4.8, -0.8f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 3.5, -0.5f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 2.0, -0.5f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 1.5, 0.8f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 1.0, 0.0f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 0.2, 0.8f, -3.0f);
			glScalef(0.5f, 0.5f, 1.0f);
			sky2();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 4.9 , 0.0f, -3.0f);
			glScalef(0.5f, 0.5f, 1.0f);
			sky2();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(crow_inframex, 0.0, -1.0f);
			crow();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(crow_inframex, 0.0, -0.5f);
			feather();
			glPopMatrix();
		}
		else
		{
			Present_Scene = PEACOCK_SCENE;
		}
		break;

	case PEACOCK_SCENE:
		if (Global_time <= 45)
		{
			glPushMatrix();
			glTranslatef(0.0f, 0.0f, -3.0f);
			glScalef(2.5f, 2.5f, 1.0f);
			skybg();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 4.5, 0.5f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 2.5, 0.5f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 4.8, -0.8f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 3.5, -0.5f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 2.0, -0.5f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 1.5, 0.8f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 1.0, 0.0f, -3.0f);
			glScalef(0.9f, 0.5f, 1.0f);
			sky1();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 0.2, 0.8f, -3.0f);
			glScalef(0.5f, 0.5f, 1.0f);
			sky2();
			glPopMatrix();

			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTranslatef(skyx + 4.9 , 0.0f, -3.0f);
			glScalef(0.5f, 0.5f, 1.0f);
			sky2();
			glPopMatrix();
		
			glPushMatrix();
			glTranslatef(0.0, 0.0f, -1.0f);
			glScalef(1.5f, 1.5f, 1.0f);
			peacock();
			glPopMatrix();
		}
		else
		{
			Present_Scene = MAIN_SCENETWO;
		}
		break;

	case MAIN_SCENETWO:
		if (Global_time <= 60)
		{
			glPushMatrix();
			glTranslatef(0.0f, 0.5f, -10.0f);
			glScalef(10.0f, 5.0f, 1.0f);
			sky();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-2.5f, 0.5f, -5.0f);
			glScalef(1.0f, 1.5f, 0.0f);
			grasspng();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -1.8f, -6.5f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glScalef(10.0f, 5.0f, 1.0f);
			grass();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, 0.35f, -6.5f);
			glScalef(1.0, 0.3, 1.0);
			floor2();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -0.5f, -6.5f);
			glScalef(1.5f, 0.6, 1.0);
			house();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -1.3f, -6.5f);
			glScalef(1.2f, 0.2, 1.0);
			step();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(1.8f, -0.6f, -5.0f);
			glScalef(0.4f, 0.4f, 0.0f);
			bush();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(2.7f, -0.7f, -5.0f);
			glScalef(0.6f, 0.3f, 0.0f);
			plant();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-2.6f, -0.7f, -5.0f);
			glScalef(1.0f, 0.5f, 0.0f);
			swing();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(crow_entryx, crow_entryy, -1.5f);
		//	glTranslatef(0.0, 0.0, -1.5);
			crow();
			glScalef(2.0, 2.0, 1.0);
			feather();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -0.7f, -1.0f);
			glScalef(0.5f, 0.5, 1.0);
			baby();
			glPopMatrix();
		}
		else
		{
			Present_Scene = MAIN_SCENETHREE;
		}
		break;	

		case MAIN_SCENETHREE:
		if (Global_time <= 75)
		{
			glPushMatrix();
			glTranslatef(0.0f, 0.5f, -10.0f);
			glScalef(10.0f, 5.0f, 1.0f);
			sky();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-2.5f, 0.5f, -5.0f);
			glScalef(1.0f, 1.5f, 0.0f);
			grasspng();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -1.8f, -6.5f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glScalef(10.0f, 5.0f, 1.0f);
			grass();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, 0.35f, -6.5f);
			glScalef(1.0, 0.3, 1.0);
			floor2();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -0.5f, -6.5f);
			glScalef(1.5f, 0.6, 1.0);
			house();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -1.3f, -6.5f);
			glScalef(1.2f, 0.2, 1.0);
			step();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(1.8f, -0.6f, -5.0f);
			glScalef(0.4f, 0.4f, 0.0f);
			bush();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(2.7f, -0.7f, -5.0f);
			glScalef(0.6f, 0.3f, 0.0f);
			plant();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-2.6f, -0.7f, -5.0f);
			glScalef(1.0f, 0.5f, 0.0f);
			swing();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(peacock_entryx, peacock_entryy, -1.0f);
		//	glTranslatef(0.0, 0.0, -1.5);
			peacock();
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.0f, -0.7f, -1.0f);
			glScalef(0.5f, 0.5, 1.0);
			baby();
			glPopMatrix();			
		}
		break;
	}


	
	SwapBuffers(ghdc);
}

void update(void)
{
	//code
	angle = angle - 0.1f;
	if (angle <= 0.0f)
	{
		angle = angle + 180.0f;
	}	

	if (s <= 1.0f)
	{
		s += 0.00005f;
		skyx = lerp(-4.0f ,4.0f, s);
	}	

	if (crowtravel <= 1.0f && Present_Scene == MAIN_SCENETWO)
	{	
		crowtravel += 0.0005f;
		crow_entryx = lerp(1.0, 0.3, crowtravel);
		crow_entryy = lerp(1.0, -0.20, crowtravel);
	}

	if (peacocktravel <= 1.0f && Present_Scene == MAIN_SCENETHREE)
	{	
		peacocktravel += 0.0005f;
		peacock_entryx = lerp(1.0, 0.3, peacocktravel);
		peacock_entryy = lerp(1.0, -0.13, peacocktravel);
	}

	if (crowtravel <= 1.0f && Present_Scene == CROW_SCENE)
	{
		crowtravel += 0.00005f;
		crow_inframex = lerp(1.0, -4.0, crowtravel);
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

void sky(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_sky);

	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void grass()
{
	glBindTexture(GL_TEXTURE_2D, texture_grass);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void grasspng(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_pnggrass);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void house(void)
{
	//1floor
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_floorone);
//	glRotatef(angle, 0.0, 1.0, 0.0);
//	glScalef(2.0, 0.8, 1.0);
	glBegin(GL_QUADS);
	glColor3f(0.651, 0.494, 0.306);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnd();
	glPopMatrix();

	// Right Face
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_wall);
//	glTranslatef(0.0f, 0.0f, -1.0f);
//	glRotatef(angle, 0.0, 1.0, 0.0);
//	glScalef(2.0, 0.8, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnd();
	glPopMatrix();
	
	// Back Face
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_wall);
//	glTranslatef(0.0f, 0.0f, -1.0f);
//	glRotatef(angle, 0.0, 1.0, 0.0);
//	glScalef(2.0, 0.8, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnd();
	glPopMatrix();

	// Left Face
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_wall);
//	glTranslatef(0.0f, 0.0f, -1.0f);
//	glRotatef(angle, 0.0, 1.0, 0.0);
//	glScalef(2.0f, 0.8, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnd();
	glPopMatrix();

}

void floor2(void)
{
	//chat1
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_chat);
//	glScalef(1.0, 0.2, 1.0);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.8f, -1.0f, 1.5f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.8f, -1.0f, 1.5f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	//2floor
	//front
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_floortwo);
//	glRotatef(angle, 0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 2.8f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 2.8f, 1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	// Right Face
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_wall);
//	glRotatef(angle, 0.0, 1.0, 0.0);
//	glScalef(1.0, 0.2, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 2.8f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 2.8f, -1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnd();
	glPopMatrix();
	
	// Back Face
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_wall);
//	glRotatef(angle, 0.0, 1.0, 0.0);
//	glScalef(1.0, 0.2, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 2.8f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 2.8f, -1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnd();
	glPopMatrix();

	// Left Face
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_wall);
//	glRotatef(angle, 0.0, 1.0, 0.0);
//	glScalef(1.0, 0.2, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 2.8f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 2.8f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnd();
	glPopMatrix();

	//chat2
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_chat);
//	glScalef(1.0, 0.2, 1.0);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.2f,2.8f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.2f, 2.8f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.1f, 2.9f, 1.5f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.1f, 2.9f, 1.5f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void step(void)
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_tiles);
//	glScalef(1.0, 0.2, 1.0);
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.25f,1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.25f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.8f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.8f, -1.0f, 1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture_brown);
//	glRotatef(angle, 0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.8f, -1.4f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.8f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.8f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.8f, -1.4f, 1.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

}

void bush(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_bush);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void plant(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_plant);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void swing(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_swing);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void skybg(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_sky);

	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void sky1(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_sky1);
	glBegin(GL_QUADS);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void sky2(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_sky2);

	glBegin(GL_QUADS);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void crow(void)
{
	//legs
	glPushMatrix();
	glTranslatef(-0.05f, -0.088f, 0.0f);
	glScalef(0.05f, 0.15f, 0.0f);
	glColor3f(0.475, 0.478, 0.475);
	gluSphere(quadric, 0.1, 9, 9);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.07f, -0.088f, 0.0f);
	glScalef(0.05f, 0.15f, 0.0f);
	glColor3f(0.475, 0.478, 0.475);
	gluSphere(quadric, 0.1, 9, 9);
	glPopMatrix();

	//tell
	glPushMatrix();
	glTranslatef(0.03f, -0.08, 0.0f);
	glScalef(0.02f, 0.04f, 0.0f);
    glRotatef(170.0, 0.0, 0.0, 1.0);
	glRotatef(sinf(angle) * 10, 1.0, 0.0, 0.01);	
//	glBindTexture(GL_TEXTURE_2D, texture_body);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,0.1f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 0.05f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.9f, -0.3f, 0.0f);
	glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.03f, -0.085, 0.0f);
	glScalef(0.02f, 0.04f, 0.0f);
    glRotatef(170.0, 0.0, 0.0, 1.0);
	glRotatef(sinf(angle) * 10, 1.0, 0.0, 0.0);	
	glBindTexture(GL_TEXTURE_2D, texture_crowbody);
//	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,0.1f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 0.05f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.9f, -0.3f, 0.0f);
	glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	//body
	glPushMatrix();
	gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    glBindTexture(GL_TEXTURE_2D, texture_crowbody);
    gluQuadricTexture(quadric, GL_TRUE);	
	glColor3f(0.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glTranslatef(-0.05f, -0.05f, 0.0f);
	glRotatef(50.0f, 0.0f, 0.0f, 1.0f);
	glScalef(0.13f, 0.3f, 0.0f);
	//glRotatef(70.0, 1.0, 0.0, 0.0);
	gluSphere(quadric, 0.3f, 50, 50);
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	//beak
	glPushMatrix();
	glTranslatef(-0.14f, 0.01f, 0.0f);
	glColor3f(0.475, 0.478, 0.475);
	glScalef(0.018f, 0.017f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(-1.5f, 0.0f);
	glEnd();	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.14f, 0.01f, 0.0f);
	glColor3f(0.475, 0.478, 0.475);
	glScalef(0.013f, 0.012f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, -1.0f);
	glVertex2f(-1.5f, 0.0f);
	glEnd();	
	glPopMatrix();

	//face
	glPushMatrix();
	gluQuadricDrawStyle(quadric, GLU_FILL);
  	gluQuadricNormals(quadric, GLU_SMOOTH);
  	glBindTexture(GL_TEXTURE_2D, texture_crowbody);
    glColor3f(0.0f, 0.0f, 0.0f);
	gluQuadricTexture(quadric, GL_TRUE);
	glTranslatef(-0.11f, 0.01f, 0.0f);
	glScalef(0.12f, 0.14f, 0.0f);
	gluSphere(quadric, 0.25, 50, 10);
	gluSphere(quadric, 0.3, 50, 50);
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	//eyes
	glPushMatrix();
	glTranslatef(-0.11f, 0.02f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glScalef(0.06f, 0.05f, 0.0f);
	gluSphere(quadric, 0.12, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.11f, 0.02f, 0.0f);
	glColor3f(0.0, 0.0, 0.0);
	glScalef(0.06f, 0.05f, 0.0f);
	gluSphere(quadric, 0.05, 50, 50);
	glPopMatrix();
}

void feather(void)
{
	glPushMatrix();
	glTranslatef(-0.02f, -0.02f, 0.0f);
	glRotatef(sinf(angle) * 10, 0.0, 0.0, 0.01);	
//	glScalef(0.05f, 0.08f, 1.0f);
	glScalef(0.03f, 0.2f, 1.0f);	
	glBindTexture(GL_TEXTURE_2D, texture_crowfeather);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,0.1f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 0.05f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.9f, -0.3f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();	
}

void peacock(void)
{
	//feather
	glPushMatrix();
	glTranslatef(-0.01f, -0.05f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, texture_peacockfeather);
	glScalef(0.1f, 0.3f, 0.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,0.1f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 0.05f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.9f, -0.3f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	//neck
	glPushMatrix();
	gluQuadricDrawStyle(quadric, GLU_FILL);
  	gluQuadricNormals(quadric, GLU_SMOOTH);
    glBindTexture(GL_TEXTURE_2D, texture_peacockbody);
    gluQuadricTexture(quadric, GL_TRUE);
	glTranslatef(-0.12f, -0.02f, 0.0f);
	glScalef(0.1f, 0.4f, 0.0f);
	gluSphere(quadric, 0.1, 10, 10);
//  gluSphere(quadric, 0.3, 50, 50);
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	//legs
	glPushMatrix();
	glTranslatef(-0.05f, -0.085f, 0.0f);
	glScalef(0.05f, 0.15f, 0.0f);
	glColor3f(0.941f, 0.639f, 0.125f);
	gluSphere(quadric, 0.1, 9, 9);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.07f, -0.085f, 0.0f);
	glScalef(0.05f, 0.15f, 0.0f);
	glColor3f(0.941f, 0.639f, 0.125f);
	gluSphere(quadric, 0.1, 9, 9);
	glPopMatrix();

	//body
	glPushMatrix();
	gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    glBindTexture(GL_TEXTURE_2D, texture_peacockbody);
    gluQuadricTexture(quadric, GL_TRUE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glTranslatef(-0.05f, -0.05f, 0.0f);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glScalef(0.1f, 0.3f, 0.0f);
	gluSphere(quadric, 0.3f, 50, 50);
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	//beak
	glPushMatrix();
	glTranslatef(-0.14f, 0.02f, 0.0f);
	glColor3f(0.941f, 0.639f, 0.125f);
	glScalef(0.016f, 0.015f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(-1.5f, 0.0f);
	glEnd();	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.14f, 0.02f, 0.0f);
	glColor3f(0.941f, 0.639f, 0.125f);
	glScalef(0.013f, 0.012f, 0.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, -1.0f);
	glVertex2f(-1.5f, 0.0f);
	glEnd();	
	glPopMatrix();

	//face
	glPushMatrix();
	gluQuadricDrawStyle(quadric, GLU_FILL);
  	gluQuadricNormals(quadric, GLU_SMOOTH);
    glBindTexture(GL_TEXTURE_2D, texture_peacockbody);
    gluQuadricTexture(quadric, GL_TRUE);
	glTranslatef(-0.13f, 0.02f, 0.0f);
	glScalef(0.2f, 0.2f, 0.0f);
	gluSphere(quadric, 0.1, 10, 10);
//  gluSphere(quadric, 0.3, 50, 50);
    glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();


	//eyes
	glPushMatrix();
	glTranslatef(-0.13f, 0.025f, 0.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glScalef(0.06f, 0.05f, 0.0f);
	gluSphere(quadric, 0.12, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.13f, 0.025f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glScalef(0.06f, 0.05f, 0.0f);
	gluSphere(quadric, 0.05, 50, 50);
	glPopMatrix();

	//featherbody
	glPushMatrix();
	glTranslatef(-0.05f, -0.05f, 0.0f);
	glRotatef(sinf(angle) * 10, 1.0, 0.0, 0.01);	
	glBindTexture(GL_TEXTURE_2D, texture_featherone);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glScalef(0.06f, 0.10f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,0.1f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 0.05f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.9f, -0.3f, 0.0f);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void Circle()
{

	float Colors[] = {//1.0f, 0.0f, 0.0f, //r
					  0.0f, 1.0f, 0.0,  //g
					  0.0f, 0.0f, 1.0f, //b
					 // 0.0f, 1.0f, 1.0f, //c
					  1.0f, 0.0f, 1.0f, //m
					  1.0f, 1.0f, 0.0f, //y
					  1.0f, 1.0f, 1.0f, //w
					  1.0f, 0.5f, 0.0f, //o
					  0.5f, 0.5f, 0.5f, //lg
					  0.5f, 0.5f, 1.0f}; //p

	float a = 0.3f;
	float b = 0.3f;
	
	for (int i = 0; i < 5; i++)
	{
		glColor3fv(Colors +i *3);

		glLineWidth(5.0f);
		glScalef(1.0f, 1.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
		float radius = 0.1f;

		for (int i = 0; i < 10000; i++)
		{
			float angle = 2.0f * M_PI * i / 10000;
			glVertex2f(a * cos(angle) + radius , b * sin(angle) + radius);
		}
		glEnd();

		a = a + 0.08f;
		b = b + 0.08f;

	}			
}

void joker(void)
{
	glBindTexture(GL_TEXTURE_2D, texture_joker);

	glBegin(GL_QUADS);

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f,1.0f, 0.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void baby(void)
{
	//hair
	glPushMatrix();
	glTranslatef(0.0f, 0.88f, -3.1f);
	glScalef(1.0f, 0.5f, 1.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	gluSphere(quadric, 0.2, 50, 50);
	glPopMatrix();
	
	//face
	glPushMatrix();	
	glTranslatef(0.0f, 0.75f, -3.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0, 0.5, 1.0);
	glColor3f(1, 0.749, 0.529);
	gluSphere(quadric, 0.2, 50, 50);
	glPopMatrix();

	//eyes
	glPushMatrix();
	glTranslatef(0.06f, 0.75f, -3.0f);
	glScalef(1.0f, 0.65f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	gluSphere(quadric, 0.1, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.06f, 0.75f, -3.0f);
	glScalef(1.0f, 0.65f, 1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	gluSphere(quadric, 0.1, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.07f, 0.75f, -3.0f);
	glScalef(0.7f, 0.6f, 1.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	gluSphere(quadric, 0.1, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.07f, 0.75f, -3.0f);
	glScalef(0.7f, 0.6f, 1.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	gluSphere(quadric, 0.1, 50, 50);
	glPopMatrix();

	//nose
	glPushMatrix();
	glTranslatef(0.0f, 0.7f, -3.0f);
	glScalef(0.7f, 0.6f, 1.0f);
	glColor3f(0.969, 0.675, 0.416);
	gluSphere(quadric, 0.1, 50, 50);
	glPopMatrix();

	//lip
	glPushMatrix();
	glTranslatef(0.0f, 0.65f, -3.0f);
	glScalef(0.6f, 0.4f, 1.0f);
	glColor3f(1.0, 0.5, 0.5);
	gluSphere(quadric, 0.1, 50, 50);
	glPopMatrix();

	//ears
	//nose
	glPushMatrix();
	glTranslatef(-0.2f, 0.75f, -3.0f);
	glScalef(0.2f, 0.2f, 1.0f);
	glColor3f(0.969, 0.675, 0.416);
	gluSphere(quadric, 0.1, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.2f, 0.75f, -3.0f);
	glScalef(0.2f, 0.2f, 1.0f);
	glColor3f(0.969, 0.675, 0.416);
	gluSphere(quadric, 0.1, 50, 50);
	glPopMatrix();

	//body
	glPushMatrix();
	glColor3f(0.878, 0.569, 0.98);
	glTranslatef(0.0f, 0.5f, -3.0f);
	glRotatef(90.0f, 1.0, 0.0, 0.0);
	glScalef(1.5, 1.5, 1.0);
	gluCylinder(quadric, 0.2, 0.22, 0.5, 30, 30);
	glPopMatrix();

	//lefthand
	glPushMatrix();
	glColor3f(0.878, 0.569, 0.98);
	glTranslatef(-0.5f, 0.4f, -3.0f);
	glRotatef(90.0f, 0.0, 1.0, 0.0);
	glScalef(0.15, 0.3, 1.0);
	gluCylinder(quadric, 0.2, 0.4, 0.3, 30, 30);
	glPopMatrix();

	//r hand
	glPushMatrix();
	glColor3f(0.878, 0.569, 0.98);
	glTranslatef(0.5f, 0.4f, -3.0f);
	glRotatef(90.0f, 0.0, 1.0, 0.0);
	glRotatef(180.0f, 1.0, 0.0, 0.0);
	glScalef(0.15, 0.3, 1.0);
	gluCylinder(quadric, 0.2, 0.4, 0.3, 30, 30);
	glPopMatrix();

// hand
	glPushMatrix();
	glTranslatef(-0.55f, 0.4f, -3.05f);
	glScalef(1.0f, 0.4f, 1.0f);
	glColor3f(1, 0.749, 0.529);
	gluSphere(quadric, 0.11, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.55f, 0.4f, -3.05f);
	glScalef(1.0f, 0.4f, 1.0f);
	glColor3f(1, 0.749, 0.529);
	gluSphere(quadric, 0.11, 10, 10);
	glPopMatrix();

	// r leg
	glPushMatrix();
	glColor3f(0.878, 0.6, 0);
	glTranslatef(0.15f, 0.05f, -3.0f);
	glRotatef(90.0f, 1.0, 0.0, 0.0);
	glScalef(0.15, 0.3, 1.0);
	gluCylinder(quadric, 0.9, 0.8, 0.25, 30, 30);
	glPopMatrix();
	//l leg
	glPushMatrix();
	glColor3f(0.878, 0.6, 0);
	glTranslatef(-0.15f, 0.05f, -3.0f);
	glRotatef(90.0f, 1.0, 0.0, 0.0);
	glScalef(0.15, 0.3, 1.0);
	gluCylinder(quadric, 0.9, 0.8, 0.25, 30, 30);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.15f, -0.25f, -3.0f);
	glScalef(1.0f, 2.0f, 1.0f);
	glColor3f(1, 0.749, 0.529);
	gluSphere(quadric, 0.09, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.15f, -0.25f, -3.0f);
	glScalef(1.0f, 2.0f, 1.0f);
	glColor3f(1, 0.749, 0.529);
	gluSphere(quadric, 0.09, 10, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.58f, -3.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(0.3f, 0.5f, 1.0f);
	glColor3f(1, 0.749, 0.529);
	gluCylinder(quadric, 0.3, 0.3, 0.5, 30, 30);
	glPopMatrix();
}

void glass(void)
{
	glPushMatrix();
	glTranslatef(0.0f, 0.58f, -3.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(0.3f, 0.5f, 1.0f);
	glColor3f(0.408, 0.408, 0.69);
	gluCylinder(quadric, 0.4, 0.2, 0.3, 30, 30);
	glPopMatrix();
}
 
void plate(void)
{
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, -3.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(0.1f, 0.2f, 1.0f);
	glColor3f(0.878, 0.6, 0.529);
	gluDisk(quadric, 0.1, 1.2,  30, 30);
	glPopMatrix();
}