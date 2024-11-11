//Standard Header Files
#include<stdio.h> //printf
#include<stdlib.h> //exit
#include<memory.h> //memset

//x11 Header Files
#include<X11/Xlib.h>  //For All Xwindow API <-> Windows.h
#include<X11/Xutil.h> //XVisualInfo and Related API's
#include<X11/XKBlib.h>

//opengl header files
#include <GL/gl.h>

#include <GL/glx.h>

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Global Variable Declaration
Display *display = NULL;
Colormap colormap;
Window window;
XVisualInfo *visualInfo = NULL;;

//opengl global variable
GLXContext glxContext = NULL;

Bool bFullscreen = False;
Bool bActiveWindow = False;

int main(void)
{
	
	//Local FUNCTION Declaration
	void uninitialise(void);
	void toggleFullScreen(void);
	void initialise(void);
	void resize(int, int);
	void draw(void);
	void update(void);
	
	//Local Variable Declarations
	int defaultScreen;
	XSetWindowAttributes windowAttributes;
    int styleMask;
	Atom windowManagerDelete;
	XEvent event;
	KeySym keySym;
	char keys[26];
	int frameBufferAttributes[] = {
								   GLX_DOUBLEBUFFER, True,
								   GLX_RGBA,
								   GLX_RED_SIZE, 8,
								   GLX_GREEN_SIZE,8,
								   GLX_BLUE_SIZE, 8,
								   GLX_ALPHA_SIZE, 8,
								   GLX_DEPTH_SIZE, 24,
								   None
								   };
	
	Bool bDone = False;

	int ScreenWidth , ScreenHeight;

	//code
	//Step1- Open connection with X-Server And Get the Display Interface
	display = XOpenDisplay(NULL);

	if(display == NULL)
	{
      	printf("XOpenDisplay() Failed \n");
		uninitialise();
	  	exit(1);
	}

	//Step2- Get Default Screen From Above Display
	defaultScreen = XDefaultScreen(display);


	//Step3- Get Default Depth From above Two
	//	defaultDepth = XDefaultDepth(display,defaultScreen);


	//Step4- Get VisualInfo From Above Three
	visualInfo = glXChooseVisual(display, defaultScreen, frameBufferAttributes);
	if(visualInfo == NULL)
	{
		printf("glXChooseVisual() Failed \n");
		uninitialise();
		exit(1);
	}


	//Step5- Set Window Attributes/Properties	 WindowAttributes - WNDCLASSEX
	memset((void*)&windowAttributes,0,sizeof(XSetWindowAttributes));

	windowAttributes.border_pixel = 0; 
	windowAttributes.background_pixel = XBlackPixel(display,visualInfo->screen);
	windowAttributes.background_pixmap = 0;
	windowAttributes.colormap = XCreateColormap(display,
												XRootWindow(display,visualInfo->screen),
												visualInfo->visual,
												AllocNone
												);
	
	//Step6- Assign This colormap to Global ColorMap
	colormap = windowAttributes.colormap;


	//Step7- Set The Style Of Window
	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;

	
	//Step8- Create The Window
	window = XCreateWindow(display,
							XRootWindow(display,visualInfo->screen),
							0,   //top Left x-Cordinate
							0,   //top Left y-Cordinate
							WIN_WIDTH,  //Window Width
							WIN_HEIGHT, //Window Height
							0,   //Boarder Width -> 0 -Default
							visualInfo->depth,
							InputOutput,
							visualInfo->visual,
							styleMask,
							&windowAttributes
							);
	

	if(!window)
	{
		printf("XCreateWindow() Failed \n");
		uninitialise();
		exit(1);
	}

	//Step9- Specify To Which events this window should response
	XSelectInput(display,
				window,
				ExposureMask | VisibilityChangeMask | StructureNotifyMask | FocusChangeMask |
				KeyPressMask | ButtonPressMask | PointerMotionMask);
    

	//Step10- Specify Window Manager Delete Atom
	windowManagerDelete = XInternAtom(display,"WM_DELETE_WINDOW",True);

	//Step11- Add/set Above Atom as Protocol For Window Manager
	XSetWMProtocols(display,window,&windowManagerDelete,1);


	//Step12- Give Caption-Name To Window
	XStoreName(display,window,"Abhay Bharat Chilka:XWindow");

	//Step13- Show/Map the Window 
	XMapWindow(display,window);

	//center the window 
	ScreenWidth =  XWidthOfScreen(XScreenOfDisplay(display, visualInfo->screen));
	ScreenHeight = XHeightOfScreen(XScreenOfDisplay(display, visualInfo->screen));

	XMoveWindow(display, window, (ScreenWidth - WIN_WIDTH) /2 , (ScreenHeight - WIN_HEIGHT )/2 );

	//opengl initilisation
	initialise();
	
	// Game Loop
	while (bDone == False)
	{
		while(XPending(display))
		{
			memset((void*)&event, 0, sizeof(XEvent));
			XNextEvent(display, &event);
			switch(event.type)
			{
				case MapNotify:					// Windows os --> show window (should be treated as WM_CREATE)
					
				break;
					
				case FocusIn:						// Windows os ---> setfocus
					bActiveWindow = True;
				break;
					
				case FocusOut:						// Windows os ---> KillFocus
					bActiveWindow = False;
				break;
				
				case ConfigureNotify:				// structureNotify internally provide configureNotify event // Windows os ---> WM_SIZE
					resize(event.xconfigure.width, event.xconfigure.height);
				break;
				
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:
							
							break;
						case 2:
							
							break;
						case 3:
							
							break;
							
						default:
						break;
					}
				break;
				
				case KeyPress:
					keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);
					switch(keySym)
					{
						case XK_Escape:
							bDone = True;
						break;
						
						default:
						break;
					}
					
					XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
					switch(keys[0])
					{
						case 'F':
						case 'f':
							if (bFullscreen == False)
							{
								toggleFullScreen();
								bFullscreen = True;
							}
							else
							{
								toggleFullScreen();
								bFullscreen = False;
							}
						break;
						
						default:
						break;
					}
				break;
				
				case 33:
					bDone = True;
				break;
				
				default:
				break;		
			
			}
		}
		
		// Rendering
		if (bActiveWindow == True)
		{
			draw();
			
			update();
		}
		
	}

	
	uninitialise();

	return 0;
}


void toggleFullScreen(void)
{
	//Local variable declarations
	Atom windowManagerStatenormal;
	Atom windowManagerStateFullscreen;

	XEvent event;

	//code
	windowManagerStatenormal= XInternAtom(display, "_NET_WM_STATE", False);

	windowManagerStateFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

	///Mame SetThe Event structure and fill it with above two atoms 
	memset((void *)&event , 0 , sizeof(XEvent));

	event.type = ClientMessage;
	event.xclient.window = window;
	event.xclient.message_type = windowManagerStatenormal;
	event.xclient.format = 32;
	event.xclient.data.l[0] = bFullscreen?0:1;
	event.xclient.data.l[1] = windowManagerStateFullscreen;

	//send event
	XSendEvent(display,
			   XRootWindow(display, visualInfo->screen),
				False,
				SubstructureNotifyMask,
				&event);
	
}

void initialise(void)
{
	//local function declaration
	void resize(int, int);
	void uninitialise(void);

	//code
	//create opengl context
	glxContext = glXCreateContext(display, visualInfo, NULL, True);
	if (glxContext == NULL)
	{
		printf("glXCreateContext failed\n");
		uninitialise();
		exit(1);
	}

	//make this context as current context
	if(glXMakeCurrent(display, window, glxContext) == False)
	{
		printf("in initialise  glXMakeCurrent failde\n");
		uninitialise();
		exit(1);
	}	

	// Enabling Depth
	glShadeModel(GL_SMOOTH);						   // Beautification Line (optional)
	glClearDepth(1.0f);								   // Compulsory
	glEnable(GL_DEPTH_TEST);						   // Compulsory
	glDepthFunc(GL_LEQUAL);							   // Compulsory
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Beuatification (optional)

	//glclear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);

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

void draw(void)
{
	void Vertical();
	void Horizontal();
	void Triangle();
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 
	glTranslatef(0.0f, 0.0f, -3.7f);

	Vertical();
	Horizontal();	
	Triangle();

	glXSwapBuffers(display, window);
}

void update(void)
{
	//code
}

void uninitialise(void)
{
	//local variable declarations
	GLXContext currentGLXContext = NULL;

	//code
	if (visualInfo)
	{
		free(visualInfo);
		visualInfo = NULL;
	}

	//uncurrent 
	currentGLXContext = glXGetCurrentContext();

	if (currentGLXContext!= NULL && currentGLXContext == glxContext)
	{
		glXMakeCurrent(display, 0, 0);
	}

	if (glxContext != NULL)
	{
		glXDestroyContext(display, glxContext);
		glxContext = NULL;
	}

	if(window)
	{
		XDestroyWindow(display,window);
	}

	if(colormap)
	{
		XFreeColormap(display,colormap);
	}

	if(display)
	{
		XCloseDisplay(display);
		display = NULL;
	}
	
}

void Vertical()
{
	//******************************Vertical Lines*************************************//
	float x_1_pv = 0.0f;
	float y_1_pv = 9.0f;
	float x_2_pv = 0.0f;
	float y_2_pv = -9.0f;
	for(int i = 0; i <= 40; i++)
	{
		if (i % 5 == 0)
		{
			glLineWidth(2);
		}
		else
		glLineWidth(1);
		
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);

		glVertex3f(x_1_pv, y_1_pv, 0);
		glVertex3f(x_2_pv, y_2_pv, 0);

		glEnd();

		x_1_pv = x_1_pv + 0.068f;
		x_2_pv = x_2_pv + 0.068f;
	}

	float x_1_nv = 0.0f;
	float y_1_nv = 9.0f;
	float x_2_nv = 0.0f;
	float y_2_nv = -9.0f;
	for(int i = 0; i <= 40; i++)
	{
		if ( i % 5 == 0)
		{
			glLineWidth(2.0f);
		}
		else
		glLineWidth(1.0f);
		
		glColor3f(0.0f, 0.0f, 1.0f);
		glBegin(GL_LINES);

		glVertex3f(x_1_nv, y_1_nv, 0);
		glVertex3f(x_2_nv, y_2_nv, 0);

		glEnd();

		x_1_nv = x_1_nv - 0.068f;
		x_2_nv = x_2_nv - 0.068f;
	}

	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 9.0f, 0.0f);
	glVertex3f(0.0f, -9.0f, 0.0f);	
	glEnd();
}

void Horizontal()
{
	//*****************************************Horizontal Lines*****************************************//
	float x_1_ph= 9.0f;
	float y_1_ph = 0.0f;
	float x_2_ph = -9.0f;
	float y_2_ph = 0.0f;
	for(int i = 0; i <= 40; i++)
	{
		glColor3f(0.0f, 0.0f, 1.0f);

		if (i % 5 == 0)
		{
			glLineWidth(2.0f);
		}
		else
		glLineWidth(1.0f);
		
		glBegin(GL_LINES);

		glVertex3f(x_1_ph, y_1_ph, 0);
		glVertex3f(x_2_ph, y_2_ph, 0);

		glEnd();

		y_1_ph = y_1_ph + 0.0375f;
		y_2_ph = y_2_ph + 0.0375f;


	}

	float x_1_nh = 9.0f;
	float y_1_nh = 0.0f;
	float x_2_nh = -9.0f;
	float y_2_nh = 0.0f;
	for(int i = 0; i <= 40; i++)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		if (i % 5 == 0)
		{
			glLineWidth(2.0f);
		}
		else
		glLineWidth(1.0f);

		glBegin(GL_LINES);

		glVertex3f(x_1_nh, y_1_nh, 0);
		glVertex3f(x_2_nh, y_2_nh, 0);

		glEnd();

		y_1_nh = y_1_nh - 0.0375f;
		y_2_nh = y_2_nh - 0.0375f;
	}

	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-9.0f, 0.0f, 0.0f);
	glVertex3f(9.0f, 0.0f, 0.0f);	
	glEnd();
}

void Triangle()
{
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glEnd();
}
