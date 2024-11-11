g++ -c -o ogl.o -I /usr/include-IGLU ogl.cpp  -w
g++ -o ogl -L /usr/lib/x86_64-linux-gnu  ogl.o -lX11 -lGL -lGLU -lGLEW

