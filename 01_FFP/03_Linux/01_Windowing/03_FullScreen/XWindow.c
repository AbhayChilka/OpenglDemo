//Standard Header Files
#include<stdio.h> //printf
#include<stdlib.h> //exit
#include<memory.h> //memset

//x11 Header Files
#include<X11/Xlib.h>  //For All Xwindow API <-> Windows.h
#include<X11/Xutil.h> //XVisualInfo and Related API's
#include<X11/XKBlib.h>

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Global Variable Declaration
Display* display = NULL;
Colormap colormap;
Window window;
XVisualInfo visualInfo;

Bool bFullscreen = False;

int main(void)
{
	
	//Local Variable Declaration
	void uninitialise(void);
	void toggleFullScreen(void);

	//Local Variable Declarations
	int defaultScreen;
	int defaultDepth;
	Status status;
	XSetWindowAttributes windowAttributes;
    int styleMask;
	Atom windowManagerDelete;
	XEvent event;
	
	KeySym keySym;
	char keys[26];

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
	defaultDepth = XDefaultDepth(display,defaultScreen);


	//Step4- Get VisualInfo From Above Three
	memset((void*)&visualInfo,0,sizeof(XVisualInfo));
	
	status = XMatchVisualInfo(display,defaultScreen,defaultDepth,TrueColor,&visualInfo);

	if(status == 0)
	{
		printf("XMatchVisualInfo() Failed \n");
		uninitialise();
		exit(1);
	}


	//Step5- Set Window Attributes/Properties	 WindowAttributes - WNDCLASSEX
	memset((void*)&windowAttributes,0,sizeof(XSetWindowAttributes));

	windowAttributes.border_pixel = 0; 
	windowAttributes.background_pixel = XBlackPixel(display,visualInfo.screen);
	windowAttributes.background_pixmap = 0;
	windowAttributes.colormap = XCreateColormap(display,
												XRootWindow(display,visualInfo.screen),
												visualInfo.visual,
												AllocNone
												);
	
	//Step6- Assign This colormap to Global ColorMap
	colormap = windowAttributes.colormap;


	//Step7- Set The Style Of Window
	styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;

	
	//Step8- Create The Window
	window = XCreateWindow(display,
							XRootWindow(display,visualInfo.screen),
							0,   //top Left x-Cordinate
							0,   //top Left y-Cordinate
							WIN_WIDTH,  //Window Width
							WIN_HEIGHT, //Window Height
							0,   //Boarder Width -> 0 -Default
							visualInfo.depth,
							InputOutput,
							visualInfo.visual,
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
				ExposureMask | VisibilityChangeMask | StructureNotifyMask | 
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
	ScreenWidth =  XWidthOfScreen(XScreenOfDisplay(display, visualInfo.screen));
	ScreenHeight = XHeightOfScreen(XScreenOfDisplay(display, visualInfo.screen));

	XMoveWindow(display, window, (ScreenWidth - WIN_WIDTH) /2 , (ScreenHeight - WIN_HEIGHT )/2 );

	//Event-Loop
	while(1)
	{
		XNextEvent(display,&event);

		switch(event.type)
		{
			case KeyPress:
				keySym = XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0);

				switch(keySym)
				{
					case XK_Escape:
						uninitialise();
						exit(0);
					break;
					
					default:
					break;

				}

			XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
			switch (keys[0])
			{
			case 'F':
			case 'f':
				if(bFullscreen == False)
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
				uninitialise();
				exit(0);
			break;

			default:
			break;

		}

	}

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
			   XRootWindow(display, visualInfo.screen),
				False,
				SubstructureNotifyMask,
				&event);
}

void uninitialise(void)
{
	//code
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



