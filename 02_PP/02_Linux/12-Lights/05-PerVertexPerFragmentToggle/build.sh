g++ -c -o ogl.o -I /usr/include -L . -lSphere ogl.cpp
g++ -o ogl ogl.o -L . -lX11 -lGL -lGLEW -lSphere

