//Standard Header Files
#include<stdio.h> //printf
#include<stdlib.h> //exit
#include<memory.h> //memset

//x11 Header Files
#include<X11/Xlib.h>  //For All Xwindow API <-> Windows.h
#include<X11/Xutil.h> //XVisualInfo and Related API's
#include<X11/XKBlib.h>

//opengl header files
#include <GL/glew.h>

#include <GL/gl.h>

#include <GL/glx.h>

#include "Sphere.h"

#include "vmath.h"
using namespace vmath;

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Global Variable Declaration
FILE *gpFile = NULL;
Display *display = NULL;
Colormap colormap;
Window window;
XVisualInfo *visualInfo = NULL;;

//pp releated
typedef GLXContext(*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig glxFBConfig;

//opengl global variable
GLXContext glxContext = NULL;

Bool bFullscreen = False;
Bool bActiveWindow = False;

GLuint shaderProgramObject = 0;

enum 
{
	AMC_ATTRIBUTE_POSITION = 0, 
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL
};


GLuint vao_sphere = 0;
GLuint vbo_sphere_position = 0;
GLuint vbo_sphere_normal = 0;
GLuint vbo_sphere_element = 0;
//GLuint vbo_color_cube = 0;

GLuint gNumElements;
GLuint gNumVertices;
//GLuint mvpMatrixUniform = 0;
GLuint modelViewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;
GLuint ldUniform = 0;
GLuint kdUniform = 0;
GLuint lightPositionUniform = 0;
GLuint keyPressUniform = 0;

Bool bLightingEnable = False;
Bool bAnimationEnable = False;

GLfloat lightDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialDefuse[] = {0.5f, 0.5f, 0.5f, 0.5f};
GLfloat lightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

mat4 perspectiveProjetionMatrix;

//rotation
GLfloat angle_sphere = 0.0f;


int main(void)
{
	//Local FUNCTION Declaration
	void uninitialise(void);
	void toggleFullScreen(void);
	int initialise(void);
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
									//PP
								   GLX_X_RENDERABLE, True,
								   GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
								   GLX_RENDER_TYPE, GLX_RGBA_BIT,  
								   GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
								   //AS BEFORE
								   GLX_RED_SIZE, 8,
								   GLX_GREEN_SIZE,8,
								   GLX_BLUE_SIZE, 8,
								   GLX_ALPHA_SIZE, 8,
								   GLX_DEPTH_SIZE, 24,
								   GLX_STENCIL_SIZE, 8,
								   GLX_DOUBLEBUFFER, True,
								   None
								   };
	
	Bool bDone = False;

	int ScreenWidth , ScreenHeight;

	//pp releated 
	GLXFBConfig *glxFBConfigs = NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *tempXVisualInfo = NULL;
	int numFBConfigs;

	int bestFrameBufferConfig = -1, bestNumberOfSamples = -1;
	int worstFrameBufferConfig = -1, worstNumberOfSamples = 999;
	int sampleBuffers, samples;
	int i;

	//code

	//logfile code
	gpFile = fopen("log.txt", "w");
	if(gpFile == NULL)
	{
		printf("log file cant be created\n");
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file is created successfully");
	}

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
	glxFBConfigs = glXChooseFBConfig(display, XDefaultScreen(display), frameBufferAttributes, &numFBConfigs);
	if( glxFBConfigs == NULL)
	{
		fprintf(gpFile ,"matching glxFBConfigs cant be found\n");
		uninitialise();
		exit(-1);
	}
	fprintf(gpFile, "%d matching FBConfigs founds", numFBConfigs);

	//find best matching FBConfig from above array
	for(i = 0; i < numFBConfigs; i++)
	{
		tempXVisualInfo = glXGetVisualFromFBConfig(display, glxFBConfigs[i]);
		if(tempXVisualInfo != NULL)
		{
			//get sample buffers
			glXGetFBConfigAttrib(display, glxFBConfigs[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
			//get samples
			glXGetFBConfigAttrib(display, glxFBConfigs[i], GLX_SAMPLES, &samples);

			if(bestFrameBufferConfig < 0 || sampleBuffers && samples > bestNumberOfSamples)
			{
				bestFrameBufferConfig = i;
				bestNumberOfSamples = samples;
			}

			if(worstFrameBufferConfig < 0 || !sampleBuffers || samples < worstNumberOfSamples)
			{
				worstFrameBufferConfig = i;
				worstNumberOfSamples = samples;
			}

			XFree(tempXVisualInfo);
		}
	}
	//accordingly get best glxfb config
	bestGLXFBConfig = glxFBConfigs[bestFrameBufferConfig];

	//assign this found best glxfb to global glxfb
	glxFBConfig = bestGLXFBConfig;

	//free memory given to array
	XFree(glxFBConfigs);

	//now get final visual from best fb config
	visualInfo = glXGetVisualFromFBConfig(display, bestGLXFBConfig);
	fprintf(gpFile, "the choosen visual id is = 0x%lu\n", visualInfo->visualid);

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

						case 'A':
						case 'a':
								if (bAnimationEnable == False)
								{
									bAnimationEnable = True;
								}
								else
								{
									bAnimationEnable = False;
								}
								break;

						case 'L':
						case 'l':
								if (bLightingEnable == False)
								{
									bLightingEnable = True;
								}
								else
								{
									bLightingEnable = False;
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

int initialise(void)
{
	//local function declaration
	void resize(int, int);
	void uninitialise(void);
	void printgGLInfo(void);

	//local variable declarations
	int attribs_New[] = {
							GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
							GLX_CONTEXT_MINOR_VERSION_ARB, 6,
							GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
							None		
						};
	
	//code
	//get address of function in function pointer
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((GLubyte *)"glXCreateContextAttribsARB");
	if(glXCreateContextAttribsARB == NULL)
	{
		fprintf(gpFile, "Cannot get required ");
		uninitialise();
		return(-1);
	}

	//create pp compatible glxcontext
  	glxContext = glXCreateContextAttribsARB(display, glxFBConfig, 0, True, attribs_New);
	if(!glxContext)
	{
		fprintf(gpFile, "core profile based glx context cant be obtain.\nfollowing back to old context\n");
		int attribs_Old[] = {
								GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
								GLX_CONTEXT_MINOR_VERSION_ARB, 0,
								None		
							};
  	glxContext = glXCreateContextAttribsARB(display, glxFBConfig, 0, True, attribs_Old);
		if(!glxContext)
		{
			fprintf(gpFile, "old glxContext cant found\n");
			uninitialise();
			return(-2);
		}
		else
		{
			fprintf(gpFile, "old glxcontext found\n");		
		}
	}	
	else
	{
		fprintf(gpFile,"coreprofile glxContext obtain successfully\n");
	}

	//is context support direct rendering
	if(!glXIsDirect(display, glxContext))
	{
		fprintf(gpFile, "not supportig direct rendering\n");
	}
	else
	{
		fprintf(gpFile,"supporting direct rendering\n");
	}

	//make this context as current context
	if(glXMakeCurrent(display, window, glxContext) == False)
	{
		printf("in initialise  glXMakeCurrent failde\n");
		uninitialise();
		return(-3);
	}	

	if(glewInit()!=GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed\n");
		return(-4);
	}	
	
	//printglinfo
	printgGLInfo();

//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
									"#version 460 core" 
									"\n" \
									"in vec4 aPosition;" \
									"in vec3 aNormal;" \
									"uniform mat4 uModelViewMatrix;" \
									"uniform mat4 uProjectionMatrix;" \
									"uniform vec3 uLd;" \
									"uniform vec3 uKd;"\
									"uniform vec4 uLightPosition;" \
									"uniform int uKeyPress;" \
									"out vec3 oDefuseLight;" \
									"void main(void)" \
									"{" 
										"if(uKeyPress == 1)"
										"{"
										"vec4 iPosition = uModelViewMatrix * aPosition;" \
										"mat3 normalMatrix = mat3(transpose(inverse(uModelViewMatrix)));" \
										"vec3 n = normalize(normalMatrix * aNormal);" \
										"vec3 s = normalize(vec3(uLightPosition - iPosition));" \
										"oDefuseLight = uLd * uKd *dot(s, n);"\
										"}"
										"else"
										"{"
										"oDefuseLight = vec3(0.0, 0.0, 0.0);"
										"}"
										"gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;"
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
				fprintf(gpFile, "VertexShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}		
		uninitialise();
	}
	
	//fragment shader
	const GLchar* fragmentShaderSourceCode = 
											"#version 460 core" \
											"\n" \
											"in vec4 oColor;" \
											"in vec3 oDefuseLight;"\
											"uniform int uKeyPress;" \
											"out vec4 FragColor;" \
											"void main(void)" \
											"{" \
											" if(uKeyPress == 1)"
											 "{"
											 	"FragColor = vec4(oDefuseLight, 1.0);"
												"}"
												"else"
												"{"
												"FragColor = vec4(1.0, 1.0, 1.0, 1.0);"
												"}"
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
				fprintf(gpFile, "FragmentShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}		
		uninitialise();
	}	

	//shader program
	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor");
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "aNormal");

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
				fprintf(gpFile, "Shader Program linking error log: %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}			
		}	
		uninitialise();
	}

	//get shader uniform locations
	modelViewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uModelViewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
	ldUniform = glGetUniformLocation(shaderProgramObject, "uLd");
	kdUniform = glGetUniformLocation(shaderProgramObject, "uKd");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "uLightPosition");
	keyPressUniform = glGetUniformLocation(shaderProgramObject, "uKeyPress");


 	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

   // vao sphere
    glGenVertexArrays(1, &vao_sphere);
    glBindVertexArray(vao_sphere);

    // position vbo sphere
    glGenBuffers(1, &vbo_sphere_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

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
	glClearDepth(1.0f);								   // Compulsory
	glEnable(GL_DEPTH_TEST);						   // Compulsory
	glDepthFunc(GL_LEQUAL);							   // Compulsory

	//glclear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//initialise
	perspectiveProjetionMatrix = vmath::mat4::identity();
	//warmup resize
	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void printgGLInfo(void)
{
	//variable declaratios
	GLint numExtensions; 
	GLint i;

	//code
	fprintf(gpFile, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "OpenGL GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFile, "-------------------------------------------\n");
	//listing of supported extensions
	glGetIntegerv( GL_NUM_EXTENSIONS, &numExtensions);

	for ( i = 0; i < numExtensions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}	
	fprintf(gpFile, "-------------------------------------------\n");

}


void resize(int width, int height)
{
	//code
	if (height <= 0)
		height = 1;

	//resize
	//resize
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjetionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void draw(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);



	glUseProgram(shaderProgramObject);

	vmath::mat4 modelViewMatrix = vmath::mat4 :: identity();

	mat4 translationmatrix = mat4::identity();

	mat4 rotationmatrix = mat4::identity();

	mat4 scalematrix = mat4::identity();

	//transformation
	//sphere
	translationmatrix = vmath::translate(0.0f, 0.0f, -6.0f);
	scalematrix = vmath::scale(2.0f, 2.0f, 2.0f);
	mat4 rotationmatrix1 = vmath::rotate(angle_sphere, 1.0f, 0.0f, 0.0f);
	mat4 rotationmatrix2 = vmath::rotate(angle_sphere, 0.0f, 1.0f, 0.0f);
	mat4 rotationmatrix3 = vmath::rotate(angle_sphere, 0.0f, 0.0f, 1.0f);
	rotationmatrix = rotationmatrix1 * rotationmatrix2 * rotationmatrix3;
	modelViewMatrix = translationmatrix * scalematrix * rotationmatrix;

	//push above mvp into vertex shaders mvpuniform
	glUniformMatrix4fv(modelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjetionMatrix);

	if (bLightingEnable == True)
	{
		glUniform1i(keyPressUniform, 1);
		glUniform3fv(ldUniform, 1, lightDefuse);
		glUniform3fv(kdUniform, 1, materialDefuse);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
	}
	else
	{
		glUniform1i(keyPressUniform, 0);
	}

    // *** bind vao ***
    glBindVertexArray(vao_sphere);

    // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element);
    glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

    // *** unbind vao ***
    glBindVertexArray(0);

	glUseProgram(0);

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


	//pyramid
	//delete vbo of positoion
	if (vbo_sphere_position)
	{
		glDeleteBuffers(1, &vbo_sphere_position);
		vbo_sphere_position = 0;
	}
	
	//delete vao
	if (vao_sphere)
	{
		glDeleteVertexArrays(1, &vao_sphere);
		vao_sphere = 0;
	}

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

	if (gpFile)
	{
		fprintf(gpFile,"Program Ended Successfully...\n");
		fclose(gpFile);
		gpFile = NULL;
	}

}


