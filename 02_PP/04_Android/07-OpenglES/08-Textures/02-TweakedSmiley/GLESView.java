package com.tweaketexturesmiley.window;

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

//texture 
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener
{
    private GestureDetector gestureDetector;
    private Context context;
    private int[] num = new int[1];

    private int shaderProgramObject;
    private int[] vao_square = new int[1];
    private int[]  vbo_position_square = new int[1];
    private int[]  vbo_texcoord_square = new int[1];
    private int[] texture_smiley  = new int[1];
    private int mvpMatrixUniform;
    private int textureSamplerUniform;
    private int keyPressUniform;
    private float[] perspectiveProjetionMatrix = new float[16];
     
    private  float[] square_texcoord = new float[8];

    //tweak
    private int singleTap;

    public GLESView(Context context1)
    {
        super(context1);

        context = context1;

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
        singleTap++;

        if(singleTap > 4)
        {
        singleTap = 0;
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
                                                "precision highp float;" +
                                                "in vec4 aPosition;" +
                                                "in vec2 aTexCoord;"  +
                                                "uniform mat4 uMVPMatrix;" +
                                                "out vec2 oTexCoord;"   +
                                                "void main(void)" +
                                                "{" +
                                                "gl_Position= uMVPMatrix*aPosition;" +
                                                "oTexCoord=aTexCoord;"  +
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
                                                    "in vec2 oTexCoord;"    +
                                                    "uniform highp sampler2D uTextureSampler;" +
                                                    "uniform int uKeyPress;" +
                                                    "out vec4 FragColor;" +
                                                    "void main(void)" +
                                                    "{" +
                                                        "if(uKeyPress == 0)" +
                                                        "{" +
                                                        "FragColor = vec4(1.0f,1.0f,1.0f,1.0f);"+
                                                        "}"+
                                                        "else"+
                                                        "{"+
                                                        "FragColor=texture(uTextureSampler, oTexCoord);"+
                                                        "}"+
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
        GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");
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
	    textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uTextureSampler");
        keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKeyPress");
        
        //geometry, shape
    	final float square_position[] = new float[]
									 {
										1.0f, 1.0f, 0.0f,
										-1.0f, 1.0f, 0.0f,
										-1.0f, -1.0f, 0.0f,
										1.0f, -1.0f, 0.0f
									 };

        //vao
        GLES32.glGenVertexArrays(1, vao_square, 0);
        GLES32.glBindVertexArray(vao_square[0]);

    	//VBO for position
        GLES32.glGenBuffers(1, vbo_position_square, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_square[0]);

        //prepare array for glbuffer data 
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(square_position.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(square_position);
        positionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, square_position.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

    	//VBO for texcoord
        GLES32.glGenBuffers(1, vbo_texcoord_square, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord_square[0]);

        //prepare array for glbuffer data 
        byteBuffer = ByteBuffer.allocateDirect(square_texcoord.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer coordBuffer = byteBuffer.asFloatBuffer();
        coordBuffer.put(square_texcoord);
        coordBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, square_texcoord.length * 4, coordBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_TEXCOORD);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

	    GLES32.glBindVertexArray(0);

        texture_smiley[0] = loadGLTexture(R.raw.smiley);

        //depth enable setting
        GLES32.glClearDepthf(1.0f);
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        //enalbe backfacecULLING
        GLES32.glEnable(GLES32.GL_CULL_FACE);

        //clear color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        Matrix.setIdentityM(perspectiveProjetionMatrix, 0);
    }

    private int loadGLTexture(int imageResourceId) 
    {
            //create bitmapFactory option object
            BitmapFactory.Options options = new BitmapFactory.Options();
            //dont scale image
            options.inScaled = false;

            //create bitmap image from image resource
            Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageResourceId, options);
           
            int[] texture = new int[1];

            GLES32.glGenTextures(1, texture, 0);
            GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
            GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);

            GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
            GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);

            GLUtils.texImage2D( GLES32.GL_TEXTURE_2D,0,bitmap, 0);

            GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
            
            GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

            return texture[0];
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
        //VARIABLE DECLARATION
        float[] square_texcoord = new float[8];  

        //code
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        //render

        GLES32.glUseProgram(shaderProgramObject);
        //tranformations

        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        
        Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -3.0f);
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjetionMatrix, 0, modelViewMatrix, 0);

        GLES32.glUniformMatrix4fv(mvpMatrixUniform, 1, false, modelViewProjectionMatrix, 0);

        GLES32.glActiveTexture(0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_smiley[0]);
        GLES32.glUniform1i(textureSamplerUniform, 0);

        GLES32.glBindVertexArray(vao_square[0]);

        //texture coordinates
        if (singleTap == 1)
        {
                    square_texcoord[0] = 1.0f;
                    square_texcoord[1] = 1.0f;

                    square_texcoord[2] = 0.0f;
                    square_texcoord[3] = 1.0f;

                    square_texcoord[4] = 0.0f;
                    square_texcoord[5] = 0.0f;

                    square_texcoord[6] = 1.0f;
                    square_texcoord[7] = 0.0f;

                    GLES32.glUniform1i(keyPressUniform, 1);			
        }
        else if (singleTap == 2)
        {
                    square_texcoord[0] = 0.5f;
                    square_texcoord[1] = 0.5f;

                    square_texcoord[2] = 0.0f;
                    square_texcoord[3] = 0.5f;

                    square_texcoord[4] = 0.0f;
                    square_texcoord[5] = 0.0f;

                    square_texcoord[6] = 0.5f;
                    square_texcoord[7] = 0.0f;

                    GLES32.glUniform1i(keyPressUniform, 1);
        }	
        else if (singleTap == 3)
        {
                    square_texcoord[0] = 2.0f;
                    square_texcoord[1] = 2.0f;

                    square_texcoord[2] = 0.0f;
                    square_texcoord[3] = 2.0f;

                    square_texcoord[4] = 0.0f;
                    square_texcoord[5] = 0.0f;

                    square_texcoord[6] = 2.0f;
                    square_texcoord[7] = 0.0f;

                    GLES32.glUniform1i(keyPressUniform, 1);	
        }	
        else if (singleTap == 4)
        {
                    square_texcoord[0] = 0.5f;
                    square_texcoord[1] = 0.5f;

                    square_texcoord[2] = 0.5f;
                    square_texcoord[3] = 0.5f;

                    square_texcoord[4] = 0.5f;
                    square_texcoord[5] = 0.5f;

                    square_texcoord[6] = 0.5f;
                    square_texcoord[7] = 0.5f;

                    GLES32.glUniform1i(keyPressUniform, 1);			
        }	
        else
        {
            GLES32.glUniform1i(keyPressUniform, 0);
        }

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(square_texcoord.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer coordBuffer = byteBuffer.asFloatBuffer();
        coordBuffer.put(square_texcoord);
        coordBuffer.position(0);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord_square[0]);
            
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, square_texcoord.length * 4, coordBuffer, GLES32.GL_DYNAMIC_DRAW);
                
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

        GLES32.glBindVertexArray(0);

        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

        GLES32.glUseProgram(0);

        requestRender();
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

        if(vbo_texcoord_square[0] > 0)
        {
            GLES32.glDeleteBuffers(1, vbo_texcoord_square, 0);
            vbo_texcoord_square[0] = 0;
        }

        if(vbo_position_square[0] > 0)
        {
            GLES32.glDeleteBuffers(1, vbo_position_square, 0);
            vbo_position_square[0] = 0;
        }

        if(vao_square[0] > 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_square, 0);
            vao_square[0] = 0;
        }
    }

}
