package com.pyramiddefuse.window;

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
    private int[] vao_pyramid = new int[1];
    private int[] vbo_position_pyramid = new int[1];
    private int[] vbo_normal_pyramid = new int[1];

    private int mvpMatrixUniform;
    private int modelViewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;
    private int ldUniform = 0;
    private int kdUniform = 0;
    private int lightPositionUniform = 0;
    private int keyPressUniform = 0;

    private boolean  bLightingEnable = false;
    private boolean  bAnimationEnable = false;

    private float[] perspectiveProjetionMatrix = new float[16];
    private float angle_pyramid = 0.0f;

    private float lightDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float materialDefuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
    private float lightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};

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
        if(bAnimationEnable == true)
        {
            update();
        }
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
        if(bLightingEnable == false)
        {
            bLightingEnable = true;
        }
        else
        {
            bLightingEnable = false;
        }
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
        if(bAnimationEnable == false)
        {
            bAnimationEnable = true;
        }
        else
        {
            bAnimationEnable = false;
        }
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
                                                "in vec3 aNormal;" +
                                                "uniform mat4 uModelViewMatrix;" +
                                                "uniform mat4 uProjectionMatrix;" +
                                                "uniform vec3 uLd;" +
                                                "uniform vec3 uKd;" +
                                                "uniform vec4 uLightPosition;" +
                                                "uniform int uKeyPress;" +
                                                "out vec3 oDefuseLight;" +
                                                "void main(void)" +
                                                "{" +
                                                    "if(uKeyPress == 1)"+
                                                    "{"+
                                                    "vec4 iPosition = uModelViewMatrix * aPosition;" +
                                                    "mat3 normalMatrix = mat3(transpose(inverse(uModelViewMatrix)));" +
                                                    "vec3 n = normalize(normalMatrix * aNormal);" +
                                                    "vec3 s = normalize(vec3(uLightPosition - iPosition));" +
                                                    "oDefuseLight = uLd * uKd *dot(s, n);"+
                                                    "}"+
                                                    "else" +
                                                    "{"+
                                                    "oDefuseLight = vec3(1.0, 1.0, 1.0);"+
                                                    "}"+
                                                    "gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;"+
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
                                                    "in vec3 oDefuseLight;"+
                                                    "out vec4 FragColor;" +
                                                    "void main(void)" +
                                                    "{" +
                                                    "  FragColor = vec4(oDefuseLight, 1.0);"+    
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
        GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
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

//      mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
        modelViewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uModelViewMatrix");
	    projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
        ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLd");
	    kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKd");
	    lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition");
	    keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKeyPress");

        //geometry, shape
        final float pyramid_position[] = new float[]
                                        { 
                                            // front
                                            0.0f, 1.0f, 0.0f,
                                            -1.0f, -1.0f, 1.0f,
                                            1.0f, -1.0f, 1.0f,

                                            // right
                                            0.0f, 1.0f, 0.0f,
                                            1.0f, -1.0f, 1.0f,
                                            1.0f, -1.0f, -1.0f,

                                            // back
                                            0.0f, 1.0f, 0.0f,
                                            1.0f, -1.0f, -1.0f,
                                            -1.0f, -1.0f, -1.0f,

                                            // left
                                            0.0f, 1.0f, 0.0f,
                                            -1.0f, -1.0f, -1.0f,
                                            -1.0f, -1.0f, 1.0f
                                        };

        final float pyramid_normal[] = new float[]
                                        {
                                            // front
                                            0.000000f, 0.447214f, 0.894427f, // front-top
                                            0.000000f, 0.447214f, 0.894427f, // front-left
                                            0.000000f, 0.447214f, 0.894427f, // front-right

                                            // right
                                            0.894427f, 0.447214f, 0.000000f, // right-top
                                            0.894427f, 0.447214f, 0.000000f, // right-left
                                            0.894427f, 0.447214f, 0.000000f, // right-right

                                            // back
                                            0.000000f, 0.447214f, -0.894427f, // back-top
                                            0.000000f, 0.447214f, -0.894427f, // back-left
                                            0.000000f, 0.447214f, -0.894427f, // back-right

                                            // left
                                            -0.894427f, 0.447214f, 0.000000f, // left-top
                                            -0.894427f, 0.447214f, 0.000000f, // left-left
                                            -0.894427f, 0.447214f, 0.000000f, // left-right
                                        };

       //pyramid
        //vao
        GLES32.glGenVertexArrays(1, vao_pyramid, 0);
        GLES32.glBindVertexArray(vao_pyramid[0]);

    	//VBO for position
        GLES32.glGenBuffers(1, vbo_position_pyramid, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_pyramid[0]);

        //prepare array for glbuffer data 
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(pyramid_position.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(pyramid_position);
        positionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramid_position.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

    	//VBO for color
        GLES32.glGenBuffers(1, vbo_normal_pyramid, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal_pyramid[0]);

        //prepare array for glbuffer data 
        byteBuffer = ByteBuffer.allocateDirect(pyramid_normal.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer normalBuffer = byteBuffer.asFloatBuffer();
        normalBuffer.put(pyramid_normal);
        normalBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramid_normal.length * 4, normalBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL);
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

       // float[] modelViewMatrix = new float[16];
        //float[] modelViewProjectionMatrix = new float[16];

        float[] translationMatrix = new float[16];
        float[] rotationMatrix = new float[16];
        float[] modelViewMatrix = new float[16];

        //triangle     
        Matrix.setIdentityM(modelViewMatrix, 0);
 
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -5.0f);
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, translationMatrix, 0);

        Matrix.setIdentityM(rotationMatrix, 0);
	    Matrix.rotateM(modelViewMatrix, 0, angle_pyramid, 0.0f, 1.0f, 0.0f);
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);

        GLES32.glUniformMatrix4fv(modelViewMatrixUniform, 1, false, modelViewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjetionMatrix, 0);

        if(bLightingEnable == true)
        {
            GLES32.glUniform1i(keyPressUniform, 1);
            GLES32.glUniform3fv(ldUniform, 1, lightDefuse, 0);
            GLES32.glUniform3fv(kdUniform, 1, materialDefuse, 0);
            GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);               
        }
        else
        {
            GLES32.glUniform1i(keyPressUniform, 0);
        }

        GLES32.glBindVertexArray(vao_pyramid[0]);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

    	GLES32.glBindVertexArray(0);

    	GLES32.glUseProgram(0);

        requestRender();
    }

    private void update()
    {
        angle_pyramid = angle_pyramid + 0.5f;
        if (angle_pyramid >= 360.0f)
        {
            angle_pyramid = angle_pyramid - 360.0f;
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
        if(vbo_position_pyramid[0] > 0)
        {
            GLES32.glDeleteBuffers(1, vbo_position_pyramid, 0);
            vbo_position_pyramid[0] = 0;
        }

        if(vao_pyramid[0] > 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_pyramid, 0);
            vao_pyramid[0] = 0;
        }

        if(vbo_normal_pyramid[0] > 0)
        {
            GLES32.glDeleteBuffers(1, vbo_normal_pyramid, 0);
            vbo_normal_pyramid[0] = 0;
        }
    }
}

