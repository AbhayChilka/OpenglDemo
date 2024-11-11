package com.two2dshapesrotation.window;

import android.content.Context; 

//opegl releated
import android.opengl.GLSurfaceView;
import android.opengl.GLES32;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig; 

// Event related packages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

///JAVA IO
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener
{
    private GestureDetector gestureDetector;

    private int[] num = new int[1];

    private int shaderProgramObject;
    private int[] vao_triangle = new int[1];
    private int[] vbo_triangle = new int[1];
    private int[] vao_square = new int[1];
    private int[] vbo_square = new int[1];
    private int mvpMatrixUniform;
    private float[] perspectiveProjetionMatrix = new float[16];
    private float angle_trinagle = 0.0f;
    private float angle_square = 0.0f;

    public GLESView(Context context)
    {
        super(context);

        //opengl releated
        setEGLContextClientVersion(3);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        //create and set gesture object
        gestureDetector = new GestureDetector(context, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    //implementation of 3 methods of GLSurfaceView
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        //code
        initialize(gl);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        //code
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl)
    {
        //code
        display();
        update();
    }

    //implementation of onTouch event method of viewclass
    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        if(!gestureDetector.onTouchEvent(e))
        {
            super.onTouchEvent(e);
        }
        return true;
    }

    //implementation of 3 methods of on double tap listner interface

    @Override
    public boolean onDoubleTap(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent e)
    {
        return true;
    }

    //implementation of 6 methods of on gesture listner interface
    @Override
    public boolean onDown(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onFling(MotionEvent eOne, MotionEvent eTwo, float velocityX, float velocityY)
    {
        return true;
    }

    @Override
    public void onLongPress(MotionEvent e)
    {
    }

    @Override
    public boolean onScroll(MotionEvent eOne, MotionEvent eTwo, float distanceX, float distanceY)
    {
        uninitialize();

        System.exit(0);
        return true;
    }

    @Override
    public void onShowPress(MotionEvent e)
    {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent e)
    {
        return true;
    }

    //implementation pf private methods
    private void initialize(GL10 gl)
    {
        //code
        //print opengles info
        printGLInfo(gl);

        //vertex shader 
        final String vertexShaderSourceCode = String.format
                                            (
                                                "#version 320 es" +
                                                "\n" +
                                                "in vec4 aPosition;" +
                                                "uniform mat4 uMVPMatrix;" +
                                                "void main(void)" +
                                                "{" +
                                                "gl_Position= uMVPMatrix*aPosition;" +
                                                "}"
                                            );

        int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

	    GLES32.glCompileShader(vertexShaderObject);
                                    
        int[] shaderCompileStatus = new int[1];
        int[] infoLogLength = new int[1];
        String infolog = null;
                                        
        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
            if(infoLogLength[0] > 0)
            {
                infolog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("ABC: VertexShader Compilation error log :" + infolog);
                uninitialize();
                System.exit(0);
            }
        }

        //fragment shader
        final String fragmentShaderSourceCode =  String.format
                                                (
        											"#version 320 es" +
                                                    "\n" +
                                                    "precision highp float;" +
                                                    "out vec4 FragColor;" +
                                                    "void main(void)" +
                                                    "{" +
                                                    "FragColor=vec4(1.0, 1.0, 1.0, 1.0);" +
                                                    "}"
                                                
                                                );

        int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);

	    GLES32.glCompileShader(fragmentShaderObject);
                                    
        shaderCompileStatus[0] = 0;
        infoLogLength[0] = 0;
        infolog = null;
                                        
        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
            if(infoLogLength[0] > 0)
            {
                infolog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("ABC: FragmentShader Compilation error log :" + infolog);
                uninitialize();
                System.exit(0);
            }
        }

        //shader program
        shaderProgramObject = GLES32.glCreateProgram();

        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
        
        GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
        GLES32.glLinkProgram(shaderProgramObject);

        int[] programLinkStatus = new int[1];
        infoLogLength[0] = 0;
        infolog = null;

        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, programLinkStatus, 0);
        if(programLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);
            if(infoLogLength[0] > 0)
            {
                infolog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("ABC: Shader Program Link Compilation error log :" + infolog);
                uninitialize();
                System.exit(0);
            }
        }

        mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

        //geometry, shape
        final float[] triangle_position = new float[]
                                        {
                                     	  0.0f, 1.0f, 0.0f,
										 -1.0f, -1.0f, 0.0f,
										 1.0f, -1.0f, 0.0f
                                        };

 
     	final float square_position[] = new float[]
									 {
										1.0f, 1.0f, 0.0f,
										-1.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 0.0f,
										1.0f, -1.0f, 0.0f
									 };

       //triangle
        //vao
        GLES32.glGenVertexArrays(1, vao_triangle, 0);
        GLES32.glBindVertexArray(vao_triangle[0]);

    	//VBO for position
        GLES32.glGenBuffers(1, vbo_triangle, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_triangle[0]);

        //prepare array for glbuffer data 
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangle_position.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(triangle_position);
        positionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangle_position.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

	    GLES32.glBindVertexArray(0);

        //square
        //vao
        GLES32.glGenVertexArrays(1, vao_square, 0);
        GLES32.glBindVertexArray(vao_square[0]);

    	//VBO for position
        GLES32.glGenBuffers(1, vbo_square, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_square[0]);

        //prepare array for glbuffer data 
        byteBuffer = ByteBuffer.allocateDirect(square_position.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(square_position);
        positionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, square_position.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

	    GLES32.glBindVertexArray(0);

        //depth enable setting
        GLES32.glClearDepthf(1.0f);
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        //clear color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        Matrix.setIdentityM(perspectiveProjetionMatrix, 0);
    }

    private void printGLInfo(GL10 gl)
    {
        //code
        System.out.println("ABC: OpenGL-ES Renderer :" + gl.glGetString(GL10.GL_RENDERER));
        System.out.println("ABC: OpenGL-ES Version :" + gl.glGetString(GL10.GL_VERSION));
        System.out.println("ABC: GLSL version: " + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
    }

    private void resize(int width, int height)
    {
        //code
        if(height <=0)
        {
            height = 1;
        }

        GLES32.glViewport(0, 0, width, height);

        Matrix.perspectiveM(perspectiveProjetionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
    }

    private void display()
    {
        //code
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        //render

        GLES32.glUseProgram(shaderProgramObject);
        //tranformations

        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        //triangle     
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        Matrix.translateM(modelViewMatrix, 0, -1.5f, 0.0f, -5.0f);
	    Matrix.rotateM(modelViewMatrix, 0, angle_trinagle, 0.0f, 1.0f, 0.0f);
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjetionMatrix, 0, modelViewMatrix, 0);

        GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

        GLES32.glBindVertexArray(vao_triangle[0]);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 3);

    	GLES32.glBindVertexArray(0);

        //square
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        Matrix.translateM(modelViewMatrix, 0, 1.5f, 0.0f, -5.0f);
	    Matrix.rotateM(modelViewMatrix, 0, angle_square, 1.0f, 0.0f, 0.0f);
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjetionMatrix, 0, modelViewMatrix, 0);

        GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

        GLES32.glBindVertexArray(vao_square[0]);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

    	GLES32.glBindVertexArray(0);

    	GLES32.glUseProgram(0);

        requestRender();
    }

    private void update()
    {
        angle_trinagle = angle_trinagle + 0.1f;
        if (angle_trinagle >= 360.0f)
        {
            angle_trinagle = angle_trinagle - 360.0f;
        }

        angle_square = angle_square - 0.1f;
        if (angle_square <= 0.0f)
        {
            angle_square = angle_square + 360.0f;
        }
    }
    private void uninitialize()
    {
        //code
        if(shaderProgramObject > 0)
        {
            GLES32.glUseProgram(shaderProgramObject);
            int[] retVal = new int[1];

	    	GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, retVal, 0);

            if(retVal[0] > 0)
            {
                int numAttachShaders = retVal[0];

                int[] shaderObjects = new int[numAttachShaders];

                GLES32.glGetAttachedShaders(shaderProgramObject, numAttachShaders, retVal,0, shaderObjects, 0);
                for (int i = 0; i < numAttachShaders; i++)
                {
                    GLES32.glDetachShader(shaderProgramObject, shaderObjects[i]);
                    GLES32.glDeleteShader(shaderObjects[i]);
                    shaderObjects[i] = 0;
                }
            }  
            GLES32.glUseProgram(0);
            GLES32.glDeleteProgram(shaderProgramObject);
		    shaderProgramObject = 0;
        }

        //square
        if(vbo_square[0] > 0)
        {
            GLES32.glDeleteBuffers(1, vbo_square, 0);
            vbo_square[0] = 0;
        }

        if(vao_square[0] > 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_square, 0);
            vao_square[0] = 0;
        }

        //triangle
        if(vbo_triangle[0] > 0)
        {
            GLES32.glDeleteBuffers(1, vbo_triangle, 0);
            vbo_triangle[0] = 0;
        }

        if(vao_triangle[0] > 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_triangle, 0);
            vao_triangle[0] = 0;
        }
    }
}

