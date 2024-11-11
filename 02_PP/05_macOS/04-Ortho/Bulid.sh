mkdir -p OGL.app/Contents/MacOS
clang -Wno-deprecated-declarations -c -o OGL.o OGL.m -framework Cocoa -framework QuartzCore -framework OpenGL