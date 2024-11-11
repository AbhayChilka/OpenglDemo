package com.spherealbedowhite.window;

import android.content.Context; 
import java.nio.ShortBuffer;

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

    private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];

    private int numVertices;
    private int numElements;

    //private int mvpMatrixUniform;
    private int modelMatrixUniform  = 0;
    private int ViewMatrixUniform = 0;

    private int projectionMatrixUniform = 0;
    private float[] perspectiveProjetionMatrix = new float[16];
   
    // material
    private int materialAmbientUniform = 0;
    private int materialDefuseUniform = 0;
    private int materialSpecularUniform = 0;
    private int materialShininessUniform = 0;
    // light
    private int lightDefuseUniform = 0;
    private int lightAmbientUniform = 0;
    private int lightSpecularUniform = 0;
    private int lightPositionUniform = 0;
    private int keyPressUniform = 0;

/*    private float lightDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float materialDefuse[] = {0.5f, 0.5f, 0.5f, 0.5f};
    private float lightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};*/

    private float LightAmbiant[] = {0.1f, 0.1f, 0.1f, 1.0f};
    private float LightDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float LigthSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float LightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};

    private float materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    private float materialDefuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    private float materialShininess = 50.0f;
 
    private boolean bLightingEnable = false;

    float sphere_vertices[]=new float[1146];
    float sphere_normals[]=new float[1146];
    float sphere_textures[]=new float[764];
    short sphere_elements[]=new short[2280];

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
                                                "precision mediump int;" +
                                                "in vec4 aPosition;"  + 
                                                "in vec3 aNormal;" + 
                                                "uniform mat4 uModelMatrix;"+
                                                "uniform mat4 uViewMatrix;"+
                                                "uniform mat4 uProjectionMatrix;"+
                                                "uniform vec3 uLightAmbient;"+
                                                "uniform vec3 uLightDefuse;"+
                                                "uniform vec3 uLightSpecular;"+
                                                "uniform vec4 uLightPosition;"+
                                                "uniform vec3 uMaterialAmbient;"+
                                                "uniform vec3 uMaterialDefuse;"+
                                                "uniform vec3 uMaterialSpecular;"+
                                                "uniform float uMaterialShininess;"+
                                                "uniform int uKeyPress;"+
                                                "out vec3 oFong_ADS_Light;"+
                                                "void main(void)" +
                                                "{"+
                                                "if(uKeyPress == 1)"+
                                                "{"+
                                                "vec4 iCoordinates = uViewMatrix * uModelMatrix * aPosition;"+
                                                "vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix)*aNormal);"+
                                                "vec3 lightDirection = normalize(vec3(uLightPosition - iCoordinates));"+
                                                "vec3 reflectionVector = reflect(-lightDirection, transformedNormals);"+
                                                "vec3 viewerVector = normalize(-iCoordinates.xyz);"+
                                                "vec3 ambientLight = uLightAmbient * uMaterialAmbient;"+
                                                "vec3 defuseLight = uLightDefuse * uMaterialDefuse *max(dot(lightDirection, transformedNormals), 0.0);"+
                                                "vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector, viewerVector), 0.0), uMaterialShininess);"+
                                                "oFong_ADS_Light = ambientLight + defuseLight + specularLight;"+
                                                "}"+
                                                "else"+
                                                "{"+
                                                "oFong_ADS_Light = vec3(0.0, 0.0, 0.0);"+
                                                "}"+
                                                "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"+
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
                                                    "precision mediump int;" +
                                                    "in vec3 oFong_ADS_Light;" +
                                                    "uniform int uKeyPress;" +
                                                    "out vec4 FragColor;" +
                                                    "void main(void)" +
                                                    "{" +
                                                    " if(uKeyPress == 1)" +
                                                    "{" +
                                                    "FragColor = vec4(oFong_ADS_Light, 1.0);" +
                                                    "}" +
                                                    "else" +
                                                    "{" +
                                                    "FragColor = vec4(1.0, 1.0, 1.0, 1.0);"+
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

        modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uModelMatrix");
        ViewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uViewMatrix");
        projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
        lightAmbientUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightAmbient");
        lightDefuseUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightDefuse");
        lightSpecularUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightSpecular");
        lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition");
        materialAmbientUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialAmbient");
        materialDefuseUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialDefuse");
        materialSpecularUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialSpecular");
        materialShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialShininess");

        keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKeyPress");

        Sphere sphere=new Sphere();
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // vao
        GLES32.glGenVertexArrays(1,vao_sphere,0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // position vbo
        GLES32.glGenBuffers(1,vbo_sphere_position,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,sphere_vertices.length * 4,verticesBuffer, GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, 3,GLES32.GL_FLOAT,false,0,0);
        
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // normal vbo
        GLES32.glGenBuffers(1,vbo_sphere_normal,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,sphere_normals.length * 4,verticesBuffer,GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL,3,GLES32.GL_FLOAT,false,0,0);
        
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // element vbo
        GLES32.glGenBuffers(1,vbo_sphere_element,0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

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

        float[] modelMatrix = new float[16];
        float[] ViewMatrix = new float[16];
        float[] translationMatrix = new float[16];
        float[] rotationMatrix = new float[16];        
        
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);

        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(ViewMatrix, 0);

        Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -3.0f);

        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(ViewMatrixUniform, 1, false, ViewMatrix, 0);
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjetionMatrix, 0);
       
        if(bLightingEnable == true)
        {
            GLES32.glUniform1i(keyPressUniform, 1);
            GLES32.glUniform3fv(lightAmbientUniform, 1, LightAmbiant, 0);
            GLES32.glUniform3fv(lightDefuseUniform, 1, LightDefuse, 0);
            GLES32.glUniform3fv(lightSpecularUniform, 1, LigthSpecular, 0);
            GLES32.glUniform4fv(lightPositionUniform, 1, LightPosition, 0);
            GLES32.glUniform3fv(materialAmbientUniform, 1, materialAmbient, 0);
            GLES32.glUniform3fv(materialDefuseUniform, 1, materialDefuse, 0);
            GLES32.glUniform3fv(materialSpecularUniform, 1, materialSpecular, 0);
            GLES32.glUniform1f(materialShininessUniform, materialShininess);
        }
        else
        {
            GLES32.glUniform1i(keyPressUniform, 0);
        }

        // bind vao
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
        
        // unbind vao
        GLES32.glBindVertexArray(0);

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

        // destroy vao
        if(vao_sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        
        // destroy position vbo
        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo
        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }
    }
}
