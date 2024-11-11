//Objective-C is c With Classes
//#import avoids inclusion of same header files multiple times 
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

//Global Variable Declarations
FILE *gpFILE = NULL;

@interface AppDelegate:NSObject <NSApplicationDelegate,NSWindowDelegate>
@end

@interface GLView:NSView 
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
    [window setTitle:@"GRP:Cocoa Window"];

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


-(id)initWithFrame:(NSRect)frame
{
    //code
    self = [super initWithFrame:frame];

    if(self)
    {

    }

    return self;
}

//WM_PAINT
-(void)drawRect:(NSRect)dirtyRect
{
    //code
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
}
@end

