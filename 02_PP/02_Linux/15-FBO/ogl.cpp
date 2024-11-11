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

#include "vmath.h"
using namespace vmath;

#include "Sphere.h"

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define FBO_WIDTH 512
#define FBO_HEIGHT 512

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

// cube releated global variables
GLuint shaderProgramObject_cube = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD
};

GLuint vao_cube = 0;
GLuint vbo_position_cube = 0;
GLuint vbo_texcoord_cube = 0;

GLuint mvpMatrixUniform_cube = 0;

mat4 perspectiveProjetionMatrix_cube;

GLuint textureSamplerUniform_cube = 0;

//
GLfloat angle_cube = 0.0f;
GLint winWidth = 0.0f;
GLint winHeight = 0.0f;

// sphere releated global variables

GLuint pervertex_shaderProgramObject_sphere = 0;
GLuint perfragment_shaderProgramObject_sphere = 0;

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

Bool bLightingEnable = False;

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

// FBO releated global variables
GLuint FBO = 0;
GLuint RBO = 0;
GLuint textureFBO;
Bool bFBOResult = False;


int main(void)
{
	//Local FUNCTION Declaration
	void toggleFullScreen(void);
	void uninitialise_cube(void);
	int initialise_cube(void);
	void resize_cube(int, int);
	void draw_cube(void);
	void update_cube(void);
	
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
		uninitialise_cube();
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
		uninitialise_cube();
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
		uninitialise_cube();
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
	initialise_cube();
	
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
					resize_cube(event.xconfigure.width, event.xconfigure.height);
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
					
					XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
					switch(keys[0])
					{
                    case 'V':
                    case 'v':

                        choosenShader = 'v';

                        modelMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uModelMatrix");
                        modelViewMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uViewMatrix");
                        projectionMatrixUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uProjectionMatrix");

                        lightAmbientUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[0]");
                        lightDefuseUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[0]");
                        lightSpecularUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[0]");
                        lightPositionUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[0]");

                        lightAmbientUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[1]");
                        lightDefuseUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[1]");
                        lightSpecularUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[1]");
                        lightPositionUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[1]");

                        lightAmbientUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[2]");
                        lightDefuseUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[2]");
                        lightSpecularUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[2]");
                        lightPositionUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[2]");

                        materialAmbientUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialAmbient");
                        materialDefuseUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialDefuse");
                        materialSpecularUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialSpecular");
                        materialShininessUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uMaterialShininess");

                        keyPressUniform_sphere = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uKeyPress");

                        break;

                    case 'F':
                    case 'f':
                        if (bLightingEnable == True)
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
                        if (bLightingEnable == False)
                        {
                            bLightingEnable = True;
                        }
                        else
                        {
                            bLightingEnable = False;
                        }

                        break;

                    case 'Q':
                    case 'q':
                        bDone = True;
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
			draw_cube();
			
			update_cube();
		}
		
	}
	uninitialise_cube();

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

int initialise_cube(void)
{
	//local function declaration
	void printgGLInfo(void);

	void resize_cube(int, int);
	void uninitialise_cube(void);
    Bool createFBO(GLint, GLint);
	Bool initialize_sphere(GLint, GLint);

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
		uninitialise_cube();
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
			uninitialise_cube();
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
		uninitialise_cube();
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
									"#version 460 core" \
									"\n" \
									"in vec4 aPosition;" \
									"in vec2 aTexCoord;" \
									"uniform mat4 uMVPMatrix;" \
									"out vec2 oTexCoord;" \
									"void main(void)" \
									"{" \
									"gl_Position=aPosition;" \
									"oTexCoord=aTexCoord;" \
									"gl_Position= uMVPMatrix*aPosition;" \
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
		uninitialise_cube();
	}
	
	//fragment shader
	const GLchar* fragmentShaderSourceCode = 
											"#version 460 core" \
											"\n" \
											"in vec2 oTexCoord;" \
											"uniform sampler2D uTextureSampler;"\		
											"out vec4 FragColor;" \
											"void main(void)" \
											"{" \
											"FragColor=texture(uTextureSampler, oTexCoord);" \
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
		uninitialise_cube();
	}	

	//shader program
	shaderProgramObject_cube = glCreateProgram();

	glAttachShader(shaderProgramObject_cube, vertexShaderObject);
	glAttachShader(shaderProgramObject_cube, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject_cube, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_cube, AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

	glLinkProgram(shaderProgramObject_cube);

	status = 0;
	infoLogLength = 0;
	szinfolog = NULL;

	glGetProgramiv(shaderProgramObject_cube, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_cube, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szinfolog =(GLchar*)malloc(infoLogLength);
			if (szinfolog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_cube, infoLogLength, NULL, szinfolog);
				fprintf(gpFile, "Shader Program linking error log: %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}			
		}	
		uninitialise_cube();
	}

	//get shader uniform locations
	mvpMatrixUniform_cube = glGetUniformLocation(shaderProgramObject_cube, "uMVPMatrix");

	textureSamplerUniform_cube = glGetUniformLocation(shaderProgramObject_cube, "uTextureSampler");
	

	const GLfloat square_position[] =
		{
			// front
			1.0f, 1.0f, 1.0f,	// top-right of front
			-1.0f, 1.0f, 1.0f,	// top-left of front
			-1.0f, -1.0f, 1.0f, // bottom-left of front
			1.0f, -1.0f, 1.0f,	// bottom-right of front

			// right
			1.0f, 1.0f, -1.0f,	// top-right of right
			1.0f, 1.0f, 1.0f,	// top-left of right
			1.0f, -1.0f, 1.0f,	// bottom-left of right
			1.0f, -1.0f, -1.0f, // bottom-right of right

			// back
			1.0f, 1.0f, -1.0f,	 // top-right of back
			-1.0f, 1.0f, -1.0f,	 // top-left of back
			-1.0f, -1.0f, -1.0f, // bottom-left of back
			1.0f, -1.0f, -1.0f,	 // bottom-right of back

			// left
			-1.0f, 1.0f, 1.0f,	 // top-right of left
			-1.0f, 1.0f, -1.0f,	 // top-left of left
			-1.0f, -1.0f, -1.0f, // bottom-left of left
			-1.0f, -1.0f, 1.0f,	 // bottom-right of left

			// top
			1.0f, 1.0f, -1.0f,	// top-right of top
			-1.0f, 1.0f, -1.0f, // top-left of top
			-1.0f, 1.0f, 1.0f,	// bottom-left of top
			1.0f, 1.0f, 1.0f,	// bottom-right of top

			// bottom
			1.0f, -1.0f, 1.0f,	 // top-right of bottom
			-1.0f, -1.0f, 1.0f,	 // top-left of bottom
			-1.0f, -1.0f, -1.0f, // bottom-left of bottom
			1.0f, -1.0f, -1.0f	 // bottom-right of bottom
		};

	const GLfloat square_texcoord[] =
		{
			// front
			1.0f, 1.0f, // top-right of front
			0.0f, 1.0f, // top-left of front
			0.0f, 0.0f, // bottom-left of front
			1.0f, 0.0f, // bottom-right of front

			// right
			1.0f, 1.0f, // top-right of right
			0.0f, 1.0f, // top-left of right
			0.0f, 0.0f, // bottom-left of right
			1.0f, 0.0f, // bottom-right of right

			// back
			1.0f, 1.0f, // top-right of back
			0.0f, 1.0f, // top-left of back
			0.0f, 0.0f, // bottom-left of back
			1.0f, 0.0f, // bottom-right of back

			// left
			1.0f, 1.0f, // top-right of left
			0.0f, 1.0f, // top-left of left
			0.0f, 0.0f, // bottom-left of left
			1.0f, 0.0f, // bottom-right of left

			// top
			1.0f, 1.0f, // top-right of top
			0.0f, 1.0f, // top-left of top
			0.0f, 0.0f, // bottom-left of top
			1.0f, 0.0f, // bottom-right of top

			// bottom
			1.0f, 1.0f, // top-right of bottom
			0.0f, 1.0f, // top-left of bottom
			0.0f, 0.0f, // bottom-left of bottom
			1.0f, 0.0f	// bottom-right of bottom
		};

	//square
	//VAO
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);

	//VBO for position
	glGenBuffers(1, &vbo_position_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_position), square_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for texcoord
	glGenBuffers(1, &vbo_texcoord_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_texcoord), square_texcoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// Enabling Depth
	glClearDepth(1.0f);		 // Compulsory
	glEnable(GL_DEPTH_TEST); // Compulsory
	glDepthFunc(GL_LEQUAL);	 // Compulsory

	// set the clear color of window to blue
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// tell opengl to enable texture
	glEnable(GL_TEXTURE_2D);

	// initialise
	perspectiveProjetionMatrix_cube = vmath::mat4::identity();

	resize_cube(WIN_WIDTH, WIN_HEIGHT);
	// here opengl starts

    // FBO releated code
	if (createFBO(FBO_WIDTH, FBO_HEIGHT) == True)
	{
		bFBOResult = initialize_sphere(FBO_WIDTH, FBO_HEIGHT);
	}

	return (0);
}

Bool createFBO(GLint textureWidth, GLint textureHeight)
{
	// check capacity of render buffer
	GLint maxRenderBufferSize;

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);

	if (maxRenderBufferSize < textureWidth || maxRenderBufferSize < textureHeight)
	{
		fprintf(gpFile, "texture sizes are overflow");
		return False;
	}

	// create customeframebuffer
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create texture for fbo in which we are going to render sphere
	glGenTextures(1, &textureFBO);
	glBindTexture(GL_TEXTURE_2D, textureFBO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);

	// attach above texture to framebuffer at default color attachment 0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureFBO, 0);

	// now create render buffer to hold depth of custome fbo
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	// set storage of above render buffer of texture size for depth
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, textureWidth, textureHeight);

	// attach above depth releated renderbuffer to fbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// check framebuffer status successfull or not
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile, "frame buffer statu is not complete");
		return False;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return True;
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

Bool initialize_sphere(GLint textureWidth, GLint textureHeight)
{
	// function declarations
	void resize_sphere(int, int);
	void uninitialize_sphere(void);

	// code

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
				fprintf(gpFile, "VertexShader Compilation error log : %s\n", szinfolog);
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
				fprintf(gpFile, "FragmentShader Compilation error log : %s\n", szinfolog);
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
				fprintf(gpFile, "Shader Program linking error log: %s\n", szinfolog);
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
	lightDefuseUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[0]");
	lightSpecularUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[0]");
	lightPositionUniform_sphere[0] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[0]");

	lightAmbientUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[1]");
	lightDefuseUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[1]");
	lightSpecularUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[1]");
	lightPositionUniform_sphere[1] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[1]");

	lightAmbientUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightAmbient[2]");
	lightDefuseUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightDefuse[2]");
	lightSpecularUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightSpecular[2]");
	lightPositionUniform_sphere[2] = glGetUniformLocation(pervertex_shaderProgramObject_sphere, "uLightPosition[2]");

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
				fprintf(gpFile, "VertexShader Compilation error log : %s\n", szinfolog);
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
				fprintf(gpFile, "FragmentShader Compilation error log : %s\n", szinfolog);
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
				fprintf(gpFile, "Shader Program linking error log: %s\n", szinfolog);
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
	light[1].defuse = vec3(0.0f, 1.0f, 0.0f); // Blue Light Source
	light[2].defuse = vec3(0.0f, 0.0f, 1.0f); // Green Light

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

	return (True);
}

void resize_cube(int width, int height)
{
	// code
	if (height <= 0)
		height = 1;

	winWidth = width;
	winHeight = height;

	// resize
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// set perspective projection matrix
	perspectiveProjetionMatrix_cube = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
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

void draw_cube(void)
{
	void draw_sphere(GLint, GLint);
	void update_sphere(void);
	void resize_sphere(int, int);

	// code
	// render fbo scene
	if (bFBOResult == True)
	{
		draw_sphere(FBO_WIDTH, FBO_HEIGHT);
		update_sphere();
	}
	// call resize cune again to compenset chain
	resize_cube(winWidth, winHeight);

	// reset color to white to compenset change done by display sphere
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject_cube);

	// transformation
	// cube
	vmath::mat4 modelViewMatrix = vmath::mat4 ::identity();

	mat4 translationmatrix = mat4::identity();
	translationmatrix = vmath::translate(0.0f, 0.0f, -6.0f);
	mat4 rotationmatrix = mat4::identity();
	vmath::mat4 modelViewProjectionMatrix = vmath::mat4 ::identity();

	// push above mvp into vertex shaders mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform_cube, 1, GL_FALSE, modelViewProjectionMatrix);

	// transformation
	// cube
	mat4 scalematrix = mat4::identity();
	scalematrix = vmath::scale(0.75f, 0.75f, 0.75f);
	mat4 rotationmatrix1 = vmath::rotate(angle_cube, 1.0f, 0.0f, 0.0f);
	mat4 rotationmatrix2 = vmath::rotate(angle_cube, 0.0f, 1.0f, 0.0f);
	mat4 rotationmatrix3 = vmath::rotate(angle_cube, 0.0f, 0.0f, 1.0f);
	rotationmatrix = rotationmatrix1 * rotationmatrix2 * rotationmatrix3;
	modelViewMatrix = translationmatrix * scalematrix * rotationmatrix;
	modelViewProjectionMatrix = vmath::mat4 ::identity();
	modelViewProjectionMatrix = perspectiveProjetionMatrix_cube * modelViewMatrix; // order is very important

	// push above mvp into vertex shaders mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform_cube, 1, GL_FALSE, modelViewProjectionMatrix);

	// for texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureFBO);
	glUniform1i(textureSamplerUniform_cube, 0);

	glBindVertexArray(vao_cube);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	glXSwapBuffers(display, window);
}

void draw_sphere(GLint textureWidth, GLint textureHeight)
{
	void resize_sphere(int, int);

	// code

	// call rez
	resize_sphere(FBO_WIDTH, FBO_HEIGHT);

	// bind with fbo
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// set color black to componset change done by display sphere

	vmath::mat4 modelViewMatrix = vmath::mat4 ::identity();

	mat4 translationmatrix = mat4::identity();

	mat4 rotationmatrix = mat4::identity();

	mat4 scalematrix = mat4::identity();

	// transformation
	// sphere
	translationmatrix = vmath::translate(0.0f, 0.0f, -3.0f);

	// view matrix
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

	if (bLightingEnable == True)
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

	// unbind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void update_cube(void)
{
	// code
	angle_cube = angle_cube - 0.1f;
	if (angle_cube <= 0.0f)
	{
		angle_cube = angle_cube + 360.0f;
	}
}

void update_sphere(void)
{
	// code
	angle_sphere += 0.005f;

	light[0].position[0] = 5.0f * cos(angle_sphere) + 5.0f * sin(angle_sphere);
	light[0].position[1] = 0.0f;
	light[0].position[2] = 5.0f * cos(angle_sphere) - 5.0f * sin(angle_sphere);
	light[0].position[3] = 1.0f;

	light[1].position[0] = 0.0f;
	light[1].position[1] = 5.0f * cos(angle_sphere) + 5.0f * sin(angle_sphere);
	light[1].position[2] = 5.0f * cos(angle_sphere) - 5.0f * sin(angle_sphere);
	light[1].position[3] = 1.0f;

	light[2].position[0] = 5.0f * cos(angle_sphere) + 5.0f * sin(angle_sphere);
	light[2].position[1] = 5.0f * cos(angle_sphere) - 5.0f * sin(angle_sphere);
	light[2].position[2] = 0.0f;
	light[2].position[3] = 1.0f;
}


void uninitialise_cube(void)
{
	//local variable declarations
	GLXContext currentGLXContext = NULL;
    void uninitialize_sphere(void);

	//code
    uninitialize_sphere();
	// code
	if (shaderProgramObject_cube)
	{
		glUseProgram(shaderProgramObject_cube);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject_cube, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_cube, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachObjectARB(shaderProgramObject_cube, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_cube);
		shaderProgramObject_cube = 0;
	}

	// cube
	if (vbo_texcoord_cube)
	{
		glDeleteBuffers(1, &vbo_texcoord_cube);
		vbo_texcoord_cube = 0;
	}

	// delete vbo of positoion
	if (vbo_position_cube)
	{
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}

	// delete vao
	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
	}

	if (textureFBO)
	{
		glDeleteTextures(1, &textureFBO);
		textureFBO = 0;
	}

	if (RBO)
	{
		glDeleteRenderbuffers(1, &RBO);
		RBO = 0;
	}

	if (FBO)
	{
		glDeleteFramebuffers(1, &FBO);
		FBO = 0;
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

void uninitialize_sphere(void)
{
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
}



