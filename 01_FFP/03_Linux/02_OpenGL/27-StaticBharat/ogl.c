//Standard Header Files
#include<stdio.h> //printf
#include<stdlib.h> //exit
#include<memory.h> //memset

#include<math.h>

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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 
/*
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
*/

glTranslatef(-2.5f, 0.0f, -5.0f);
//	glTranslatef(b_x, 0.0f, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	B();
	glLoadIdentity();

	glTranslatef(-1.6f, 0.0f, -5.0f);
//	glTranslatef(h_x, h_y, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	H();
	glLoadIdentity();

	glTranslatef(-0.7f, 0.0f, -5.0f);
//	glTranslatef(a_x, a_y, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	A();
	glLoadIdentity();

	glTranslatef(0.2f, 0.0f, -5.0f);
//	glTranslatef(r_x, r_y, -5.0);
	glScalef(1.3, 1.3, 0.0);
	R();
	glLoadIdentity();

	glTranslatef(1.10f, 0.0f, -5.0f);
//	glTranslatef(a2_x, a2_y, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	A();
	glLoadIdentity();

	glTranslatef(2.3f, 0.0f, -5.0f);
//	glTranslatef(t_x, 0.0f, -5.0f);
	glScalef(1.3, 1.3, 0.0);
	T();
	glLoadIdentity();


	glXSwapBuffers(display, window);
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
		j += 0.0010f;
		jetmidx = lerp(-4.0f ,-2.5f, j);

		j1x = lerp(-4.0f, -2.5f, j);
		j1y = lerp(2.0f, 0.02f, j);

		angle = lerp(-90.0f, 0.0f, j);
		j3x = lerp(-4.0f, -2.5f, j);
		j3y = lerp(-2.0f, -0.02f, j);			
	}		  
	else if (j_all <= 1.0f)
	{
		j_all += 0.0005f;
		jetmidx = lerp(-2.5f ,2.5f, j_all);
		j1x = lerp(-2.5f, 2.5f, j_all);
		j3x = lerp(-2.5f, 2.5f, j_all);	 		
	}
	else if (j_out <= 1.0f)
	{
		j_out += 0.0010f;
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

void B()
{
	glBegin(GL_QUADS);
	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.10,0.6f);
	glVertex2f(0.01,0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.1,0.5f);
	glVertex2f(-0.20,0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f,-0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.1f, -0.5f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.05f, 0.55f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.5f,0.45f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, 0.35f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.5f,0.45f);
	glVertex2f(0.4f, 0.35f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.05f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.5f, 0.05f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.5f, 0.05f);
	glVertex2f(0.5f, -0.05f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f, -0.05f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.01f, 0.05f);
	
	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.25f, -0.05f);
	glVertex2f(0.35f, -0.05f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.45f, -0.2f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.35f, -0.2f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.45f, -0.2f);
	glVertex2f(0.35f, -0.2f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.35f, -0.4f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.45f, -0.4f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.35f, -0.35f);
	glVertex2f(0.45f, -0.4f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f,-0.55f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.1f, -0.5f);
	glEnd();	
}

void H()
{
	glBegin(GL_QUADS);
	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.10,0.6f);
	glVertex2f(0.01,0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.1,0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.20,0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f,-0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.1f, -0.5f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.1f, -0.5f);
	glVertex2f(0.01f,-0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.10f, -0.6f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.01f,-0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.4f, 0.5f);
	glVertex2f(0.5f,0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.6f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.3f,0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.4f, 0.5f);
	glVertex2f(0.5f, 0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.5f,-0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -0.5f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.4f, -0.5f);
	glVertex2f(0.5f,-0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.6f, -0.6f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.5f,-0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.5f, -0.05f);
	glVertex2f(0.5f, 0.05f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f, 0.05f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.01f, -0.05f);

	glEnd();
}

void A()
{
	glBegin(GL_QUADS);
	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.10,0.6f);
	glVertex2f(0.01,0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.1,0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.20,0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f,-0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.1f, -0.5f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.1f, -0.5f);
	glVertex2f(0.01f,-0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.10f, -0.6f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.01f,-0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.4f, 0.45f);
	glVertex2f(0.5f, 0.45f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.5f,-0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -0.5f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.4f, -0.5f);
	glVertex2f(0.5f,-0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.6f, -0.6f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.5f,-0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.05f, 0.55f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.5f,0.45f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, 0.35f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.5f, -0.05f);
	glVertex2f(0.5f, 0.05f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f, 0.05f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.01f, -0.05f);

	glEnd();
}

void R()
{
	glBegin(GL_QUADS);
	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.10,0.6f);
	glVertex2f(0.01,0.5f);
	glVertex2f(-0.1,0.5f);
	glVertex2f(-0.20,0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f,-0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.1f, -0.5f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.01f, 0.5f);
	glVertex2f(0.05f, 0.55f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.5f,0.45f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, 0.35f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.5f,0.45f);
	glVertex2f(0.4f, 0.35f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.05f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.5f, 0.05f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.5f, 0.05f);
	glVertex2f(0.5f, -0.05f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f, -0.05f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.01f, 0.05f);
	
	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.15f, -0.05f);
	glVertex2f(0.25f, -0.05f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.45f, -0.2f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.35f, -0.2f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.45f, -0.2f);
	glVertex2f(0.35f, -0.2f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.35f, -0.5f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.45f, -0.5f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.1f, -0.5f);
	glVertex2f(0.01f,-0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.10f, -0.6f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.01f,-0.6f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(0.35f, -0.5f);
	glVertex2f(0.45f,-0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.55f, -0.6f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.45f,-0.6f);

	glEnd();
}

void T()
{
	glBegin(GL_QUADS);	
	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.05f, 0.5f);
	glVertex2f(0.1f, 0.45f);
	glVertex2f(0.3f,0.5f);
	glVertex2f(0.1f, 0.55f);

	glVertex2f(-0.05f, 0.5f);
	glVertex2f(-0.15f, 0.45f);
	glVertex2f(-0.35f,0.5f);
	glVertex2f(-0.15f, 0.55f);

	glColor3f(1.5f, 0.5f, 0.0f);
	glVertex2f(-0.1f, 0.5f);
	glVertex2f(0.01f, 0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.01f,-0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.1f, -0.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
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
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(-0.5f, 0.0f);
	glVertex2f(-1.5f, 0.1f);
	glVertex2f(-1.5f, -0.1f);	
	glEnd();
}

void Fog2()
{
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(-0.5f,0.0f);
	glVertex2f(-1.5f, 0.1f);
	glVertex2f(-1.5f, -0.1f);
	glEnd();
}

void Fog3()
{
	glBegin(GL_POLYGON);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-0.5f,0.0f);
	glVertex2f(-1.5f, 0.1f);
	glVertex2f(-1.5f, -0.1f);
	glEnd();
}