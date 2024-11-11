#import <foundation/foundation.h> 
#import <cocoa/cocoa.h> 

@interface AppDelegate: NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

int main(int argc, char *argv[])
{
	// code
    //create auto released poool for memory management
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
    //create global / shared nsapp aplication object
	NSApp = [NSApplication sharedApplication];
	
    //set its delegate to our owm custuom app
	[NSApp setDelegate: [[AppDelegate alloc]init]];
 		
    //start run loop
	[NSApp run];
	
    //let auto release pool release all
	[pool release];
	
	return 0;
}

//implementation of our custome app delegate 
@implementation AppDelegate
{
	NSWindow *window;
    View *view;
}

-(void) applicationDidFinishLaunching: (NSNotification *) notification 
{
	// code
    //declare rectangle for frame or border of our window    
	NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);   
    //create window
	window = [[NSWindow alloc] initWithContentRect: win_rect
              styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
		      backing: NSBackingStoreBuffered  
		      defer: NO];
 
    //GIVE TITLE TO THE WINDOW 
	[window setTitle: @"ABC: Cocoa Window"];
	[window center];
    view = [[View alloc]initWithFrame:win_rect];
    [window setContentView: view];
	[window setDelegate: self];  
	[window makeKeyAndOrderFront: self];  
}

-(void)applicationWillTerminate:(NSNotification *) notification
{
	// code
}

-(void) windowWillClose:(NSNotification *) notification  
{
	[NSApp terminate: self];  
}

-(void) dealloc 
{
    [super dealloc];
    [view release];
    [window release];
}
@end

//implementation of custome view interface 
