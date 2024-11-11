#import<Foundation/Foundation.h>
#import<Cocoa/Cocoa.h>

#import<QuartzCore/CVSisplayLink.h> //corevideo display link

#import <OpenGL/gl3.h>  // imports from OpenGL programable pipeline
#import <OpenGL/gl3ext.h>  // OpenGL extensions (glew)

//Global Function declaration
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *);

//Global Variable Declarations
FILE *gpFILE = NULL;

@interface AppDelegate:NSObject <NSApplicationDelegate,NSWindowDelegate>
@end

@interface GLView:NSOpenGLView 
@end

//Main Function

int main(int argc,char*argv[])
{
    //code
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
    
    //NSApp objectShared Between all applications
    NSApp = [NSApplication sharedApplication];
    [NSApp setDelegate:[[AppDelegate alloc]init]];
    [NSApp run]; //Run Loop
    [pool release];


    return 0;
}

//implementation of AppDelegate
@implementation AppDelegate
{
    NSWindow *window;
    GLView *glView;
}

//Methods of NSApplicationDelegate
//WM_CREATE
-(void)applicationDidFinishLaunching:(NSNotification *)notification
{
    //code
    //Create Log File
    NSBundle *appBundle = [NSBundle mainBundle];
    NSString *appDirPath = [appBundle bundlePath];

    NSString *parentDirPath = [appDirPath stringByDeletingLastPathComponent];

    NSString *logFileNameWithPath = [NSString stringWithFormat:@"%@/log.txt",parentDirPath];

    const char* pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

    gpFILE = fopen(pszLogFileNameWithPath,"w");
    if(gpFILE == NULL)
    {
        NSLog(@"Log file Cannot be Created\n");
        [self release];
        [NSApp terminate:self];
    }

    fprintf(gpFILE,"Programme Started Successfully\n");


    //Declare Rectangle for frame of our window
    NSRect winRect = NSMakeRect(0.0,0.0,800.0,600.0);

    //Create The Window
    window = [[NSWindow alloc]initWithContentRect:winRect styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                        backing:NSBackingStoreBuffered
                                        defer:NO];
    
    //Give Title to the Window
    [window setTitle:@"Abhay Chilka:Cocoa Window"];

    //Centering of Window
    [window center];

    //painting window Backfround Black
    [window setBackgroundColor:[NSColor blackColor]];

    //Create the Custom View
    glView = [[GLView alloc]initWithFrame:winRect];

    //Set This newly created custom view as view of our newly created widnow
    [window setContentView:glView];

    //Set Window Delegate
    [window setDelegate:self];

    //Actually show the window, give its keyboard focus and make it top on z order
    [window makeKeyAndOrderFront:self];

}

//WM_DESTROY
-(void)applicationWillTerminate:(NSNotification *)notification
{
    if(gpFILE != NULL)
    {
        fprintf(gpFILE,"Programme Finished Successfully");
        fclose(gpFILE);
        gpFILE = NULL;
    }

}

//Methods of NSWindowDelegate
-(void)windowWillclose:(NSNotification *)notification
{
    //code
    //Application will close after closing the window
    [NSApp terminate:self];
}

//Method of NSObject
-(void)dealloc
{
    [super dealloc];
    [glView release];
    [window release];
}

@end


//implementation of custom view interface
@implementation GLView
{
    CVDisplayLinkRef displayLink;
}

-(id)initWithFrame:(NSRect)frame
{
    //code
    self = [super initWithFrame:frame];

    if(self)
    {   //declare opengl pixel format
        NSOpenGLPixelFormatAttribute attributes[] = 
        {
            NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
            NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),  
            NSOpenGLPFAColorSize, 24,
            NSOpenGLPFADepthSize, 24,
            NSOpenGLPFAAlphaSize, 8,
            NSOpenGLPFANoRecovery,  
            NSOpenGLPFAAccelerated,  
            NSOpenGLPFADoubleBuffer,
            0
        };

        //create pixel format using above attribute
        NSOpenGLPixelFormat *PixelFormat = [[[NSOpenGLPixelFormat alloc]initWithAttributes: attributes]autorelease];
        if(glPixelFormat == nil)
        {
            fprintf(gpFILE, "OpenGL pixel format failed");
            [self uninitialize];
            [self release];
            [NSApp  terminate: self];
        }
    

        NSOpenGLContext *glContext = [[[NSOpenGLContext alloc] initWithFormat:PixelFormat shareContext: nil]autorelease];
        if(glContext == nil)
        {
            fprintf(logFile, "[NSOpenGLContext initWithFormat]: failed\n");
            [self uninitialize];
            [self release];
            [NSApp terminate: self];
        }

        //set pixel format
        [self setPixelFormat : PixelFormat];

        //set opengl context
        [self setOpenGLContext : glContext];
    }
    return self;
}

-(void) prepareOpenGL
{
    //codde
    [super prepareOpenGL];

    [[self openGLContext]makeCurrentContext];

    //matching monitor retrace with double buffer swapping
    GLint swapInterval = 1;
    [[self openGLContext]setValues: &swapInterval forParameter: NSOpenGLCPSwapInterval];

    int result = [self initialize];

    //create and start display link
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);

    //nsopengl pixel format to cgl pixel format
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelformat]CGLPixelFormatObj];

    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];

    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

    CVDisplayLinkStart(displayLink);
}

-(void)reshape
{
    //code
    [super reshape];

    [[self openGLContext]makeCurrentContext];

    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    NSRect rect = [self bounds];
    
    int width = rect.size.width;
    int height = rect.size.height;

    [self resize:width :height];
    
    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

//WM_PAINT
-(void)drawRect:(NSRect)dirtyRect
{
    //code
    //call rendering function here to avoid flickering
    [self drawView];
}

-(void)drawView
{
    [[self openGLContext] makeCurrentContext];

    CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    [self display];

    CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);      //swapbuffer()

    CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

//function call in pre frame
-(CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];

    //call rendering function here to
    [self drawView];
    [self myupdate];

    [pool release];

    return(kCVReturnScccess);
}

-(int)initialize
{
    //code
    [self printGLInfo];

	//Vertex Shader
	const GLchar* vertexShaderSourceCode = 
									"#version 410 core" \
									"\n" \
									"in vec4 aPosition;" \
									"uniform mat4 uMVPMatrix;" \
									"void main(void)" \
									"{" \
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
				fprintf(gpFILE, "VertexShader Compilation error log : %s\n", szinfolog);
				free(szinfolog);
				szinfolog = NULL;
			}
		}		
        [self uninitialize];
        [self release];
        [NSApp terminate: self];
	}
	
	//fragment shader
	const GLchar* fragmentShaderSourceCode = 
											"#version 410 core" \
											"\n" \
											"out vec4 FragColor;" \
											"void main(void)" \
											"{" \
											"FragColor=vec4(1.0, 1.0, 1.0, 1.0);" \
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
        [self uninitialize];
        [self release];
        [NSApp terminate: self];
	}	

	//shader program
	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
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
        [self uninitialize];
        [self release];
        [NSApp terminate: self];
	}

    //depth    
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);    
    glDepthFunc(GL_LEQUAL);

    //clear color
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    [self resize:WIN_WIDTH :WIN_HEIGHT];

    return(0);
}

-(void)printGLInfo
{
    //code
    fprintf(gpFILE, "OpenGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFILE, "OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFILE, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFILE, "OpenGL GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(gpFILE, "-------------------------------------------\n");

}

-(void)resize:(int)width : (int)height
{
    //code
	if (height <= 0)
		height = 1;

	//resize
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

-(void)display
{
    //code
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);

	//transformation
	mat4 modelViewMatrix = vmath::mat4::identity();
	mat4 modelViewProjectionMatrix = orthographicProjetionMatrix * modelViewMatrix; 	//order is very important

	//push above mvp into vertex shaders mvpuniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);

	glUseProgram(0);
}

-(void)myupdate
{
    //code
}

-(void) uninitialize
{
    // code
}

-(BOOL)acceptsFirstResponder
{
    //code
    [[self window]makeFirstResponder:self];

    return YES;
}

// - instance method
// + static method

-(void)keyDown:(NSEvent *)event
{
    //code
    int key = (int)[[event characters]characterAtIndex:0];

    switch(key)
    {
        case 27: //Escape
            [self uninitialize];
            [self release];
            [NSApp terminate:self];
        break;

        case 'F':
        case 'f':
            [[self window]toggleFullScreen:self];
        break;
        
        default:
        break;

    }
}

-(void)mouseDown:(NSEvent *)event
{
   
}

-(void)rightMouseDown:(NSEvent *)event
{
    
}

//Method of NSObject
-(void)dealloc
{
    [super dealloc];

    [self uninitialize];

    if(displayLink)
    {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = nil;
    }
}
@end

//definition of global callback function
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *now, const CVTimeStamp *outputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
    //code
    CVReturn result = [(GLView*)displayLinkContext getFrameForTime: outputTime];
    return(result);
}