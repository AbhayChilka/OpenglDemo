package com.TwoStaticLightVertex.window;

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

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener,OnGestureListener
{
    private GestureDetector gestureDetector;

    //OpenGL related variables
    private int shaderProgramObject;

    private int[] vaoPyramid = new int[1];
    private int[] vboPyramidPosition = new int[1];
    private int[] vboPyramidNormal=new int[1];

    //unifrom

    // Uniform Location For Transformation
    private int modelMatrixUniform = 0;
    private int viewMatrixUniform = 0;
    private int projectionMatrixUniform = 0;

    // unifrom location for Lights Attributes
    private int[] lightAmbientUniform= new int[2];
    private int[] lightDefuseUniform= new int[2];
    private int[] lightSpecularUniform= new int[2];
    private int[] lightPositionUniform = new int[2];

    // unifrom location for Material Attributes
    private int materialAmbientUniform = 0;
    private int materialDefuseUniform = 0;
    private int materialSpecularUniform = 0;
    private int materialShininessUniform = 0;

    private boolean  bAnimationEnable = false;
    private int keyPressUniform = 0;

    // Material Reflective Properties
    private float[] materialAmbient = new float[] {0.0f, 0.0f, 0.0f, 1.0f};
    private float[] materialDefuse = new float[]{1.0f, 1.0f, 1.0f, 1.0f};
    private float[] materialSpecular = new float[] {1.0f, 1.0f, 1.0f, 1.0f};
    private float materialShininess = 128.0f;

    boolean bLightingEnable = false;

    Light[] light = new Light[2];

    private float[] perspectiveProjectionMatrix = new float[16];

    //uniforms

    private float angle_pyramid;

    private class Light
    {
        public float[] ambient = new float[3];
        public float[] defuse = new float[3];
        public float[] specular = new float[3];
        public float[] position = new float[4];
    }

    public GLESView(Context context)
    {
        super(context);

        //OpenGLES Related
        //Create set Current OpenGLES Context for version 3
        setEGLContextClientVersion(3);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        //Event Related
        //Create and Set GestureDetector Object
        gestureDetector = new GestureDetector(context,this,null,false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    //Implementation of 3 GLSurfaceView.Renderer Interface Methods
    @Override
    public void onSurfaceCreated(GL10 gl,EGLConfig config)
    {
        //code
        initialise(gl);
    }

    @Override
    public void onSurfaceChanged(GL10 gl,int width,int height)
    {
        //code
        resize(width,height);
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

    //Implementation of onTouchEvent of view class (AppCompatTextView is extended from view class)
    @Override
    public boolean onTouchEvent(MotionEvent e)
    {

        //code
        if(!gestureDetector.onTouchEvent(e)) {
            super.onTouchEvent(e);
        }
        return true;
    }


    //Implementation of 3 methods of OnDoubleTapListener Interface
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

    //Implementation of 6 methods of OnGestureListener Interface
    @Override
    public boolean onDown(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onFling(MotionEvent e1,MotionEvent e2,float velocityX,float velocityY)
    {

        return true;
    }
    @Override
    public void onLongPress(MotionEvent e)
    {

    }

    //Swipe
    @Override
    public boolean onScroll(MotionEvent e1,MotionEvent e2,float distanceX,float distanceY)
    {
        //code
        uninitialise();
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


    //Implementation of private Methods
    private void initialise(GL10 gl)
    {
        //code
        //print OpenGLES Information
        printGLInfo(gl);

        //Shaders
        // Step1 - Write Vertex Shader
    	final String vertexShaderSourceCode = String.format
                (
                "#version 320 es"+
                "\n"+
                "in vec4 aPosition;"+
                "in vec3 aNormal;"+
                "uniform mat4 uModelMatrix;"+
                "uniform mat4 uViewMatrix;"+
                "uniform mat4 uProjectionMatrix;"+
                "uniform vec3 uLightDefuse[2];"+
                "uniform vec3 uLightAmbient[2];"+
                "uniform vec3 uLightSpecular[2];"+
                "uniform vec4 uLightPosition[2];"+
                "uniform vec3 uMaterialDefuse;"+
                "uniform vec3 uMaterialAmbient;"+
                "uniform vec3 uMaterialSpecular;"+
                "uniform float uMaterialShininess;"+
                "uniform int uKeyPress;"+
                "out vec3 oPhong_ADS_Light;"+
                "void main(void)"+
                "{"+
                "	oPhong_ADS_Light = vec3(0.0f,0.0f,0.0f);"+
                "	if(uKeyPress == 1)"+
                "	{"+
                "		vec3 lightDirection[2];"+
                "		vec3 defuseLight[2];"+
                "		vec3 ambientLight[2];"+
                "		vec3 specularLight[2];"+
                "		vec3 reflectionVector[2];"+
                "		vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;"+
                "		vec3 transformedNormals = normalize(mat3(uViewMatrix * uModelMatrix) * aNormal);"+
                "		for(int i=0; i<2 ; i++)"+
                "		{"+
                "		   lightDirection[i] = normalize(vec3(uLightPosition[i] - eyeCoordinates));"+
                "		   reflectionVector[i] = reflect(-lightDirection[i],transformedNormals);"+
                "		   vec3 viewerVector = normalize(-eyeCoordinates.xyz);"+
                "		   ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;"+
                "		   defuseLight[i] = uLightDefuse[i] * uMaterialDefuse * max(dot(lightDirection[i],transformedNormals),0.0);"+
                "		   specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i],viewerVector),0.0),uMaterialShininess);"+
                "		   oPhong_ADS_Light = oPhong_ADS_Light + ambientLight[i] + defuseLight[i] + specularLight[i];"+
                "		}"+
                "	}"+
                "	else"+
                "	{"+
                "		oPhong_ADS_Light = vec3(1.0f,1.0f,1.0f);"+
                "	}"+
                "   gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"+
                "}"
                );


        // Step2 - Create Vertex Shader Object
        int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        // Step3 - Assign Vertex Shader source code to Vertex shader object
        GLES32.glShaderSource(vertexShaderObject,vertexShaderSourceCode);

        // Step4 - Compile Vertex Shader
        GLES32.glCompileShader(vertexShaderObject);

        // Step5 - check for vertex shader compilation errors if any
        //
        int[] status = new int[1];
        int[] infoLogLength = new int[1];
        String szInfoLog = null;

        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, status,0);

        if (status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength,0);

            if (infoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("ABC:Vertex Shader Compilation Error Log : "+ szInfoLog);
            }
            uninitialise();
            System.exit(0);
        }

        // Step6 - Write Source code Of Fragment Shader
    	final String fragmentShaderSourceCode = String.format
                (
                    "#version 320 es"+
                    "\n"+
                    "precision highp float;"+
                    "in vec3 oPhong_ADS_Light;"+
                    "out vec4 FragColor;"+
                    "void main(void)"+
                    "{"+
                    "	FragColor = vec4(oPhong_ADS_Light,1.0f);"+
                    "}");


        // Create Fragmnet Shader Object
        int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        // Assign Fragment shader source code fragment shader object
        GLES32.glShaderSource(fragmentShaderObject,fragmentShaderSourceCode);

        // Compile Fragment shader object
        GLES32.glCompileShader(fragmentShaderObject);


         status[0] = 0;
         infoLogLength[0] = 0;
         szInfoLog = null;


        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, status,0);

        if (status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength,0);

            if (infoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("ABC:Fragment Shader Compilation Error Log : "+ szInfoLog);
            }
            uninitialise();
            System.exit(0);
        }


        // Shader Program
        shaderProgramObject = GLES32.glCreateProgram();

        // Attach vertex shader to program object
        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);

        // Attach fragment shader to program object
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

        // Bind Attribute Locations with Shader program Object
        GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
        GLES32.glBindAttribLocation(shaderProgramObject, VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
       
        // Link Shader Program
        GLES32.glLinkProgram(shaderProgramObject);

        // Error Checking
        status[0] = 0;
        infoLogLength[0] = 0;
        szInfoLog = null;

        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, status,0);

        if (status[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength,0);

            if (infoLogLength[0] > 0)
            {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("ABC:Program object Link Error Log : "+ szInfoLog);
            }
            uninitialise();
            System.exit(0);
        }

        // Get Shaders Uniform Locations
        modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uModelMatrix");
        viewMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uViewMatrix");
        projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");

        // Light uniform
        lightAmbientUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightAmbient[0]");
        lightDefuseUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightDefuse[0]");
        lightSpecularUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightSpecular[0]");
        lightPositionUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition[0]");

        lightAmbientUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightAmbient[1]");
        lightDefuseUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightDefuse[1]");
        lightSpecularUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightSpecular[1]");
        lightPositionUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition[1]");

        // material uniform locations
        materialAmbientUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialAmbient");
        materialDefuseUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialDefuse");
        materialSpecularUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialSpecular");
        materialShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialShininess");

        keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKeyPress");


        //Pyramid data
        final float[] pyramid_position = new float[]
        {
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

	final float[] pyramid_normal = new float[]
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
                    -0.894427f, 0.447214f, 0.000000f  // left-right
            };
       

        //vao Pyramid
        GLES32.glGenVertexArrays(1,vaoPyramid,0);
        GLES32.glBindVertexArray(vaoPyramid[0]);

        //vbo Pyramid Position
        GLES32.glGenBuffers(1,vboPyramidPosition,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vboPyramidPosition[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(pyramid_position.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer floatBuffer = byteBuffer.asFloatBuffer();
        floatBuffer.put(pyramid_position);
        floatBuffer.position(0);


        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,pyramid_position.length*4,floatBuffer,GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION,3,GLES32.GL_FLOAT,false,0,0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);

         //vbo Cube Normals
        GLES32.glGenBuffers(1,vboPyramidNormal,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vboPyramidNormal[0]);

        byteBuffer = ByteBuffer.allocateDirect(pyramid_normal.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        floatBuffer = byteBuffer.asFloatBuffer();
        floatBuffer.put(pyramid_normal);
        floatBuffer.position(0);


        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,pyramid_normal.length*4,floatBuffer,GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL,3,GLES32.GL_FLOAT,false,0,0);
        GLES32.glEnableVertexAttribArray(VertexAttributesEnum.AMC_ATTRIBUTE_NORMAL);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);

     
        GLES32.glBindVertexArray(0);

        // Light0
        light[0] = new Light();
        light[0].ambient = new float[]{0.0f, 0.0f, 0.0f};
        light[0].defuse = new float[]{1.0f, 0.0f, 0.0f}; // REd Light Source
        light[0].specular = new float[]{1.0f, 0.0f, 0.0f};
        light[0].position = new float[]{-2.0f, 0.0f, 0.0f, 1.0f};

        // Light1
        light[1] = new Light();
        light[1].ambient = new float[]{0.0f, 0.0f, 0.0f};
        light[1].defuse = new float[]{0.0f, 0.0f, 1.0f}; // Blue Light Source
        light[1].specular = new float[]{0.0f, 0.0f, 1.0f};
        light[1].position =new float[]{2.0f, 0.0f, 0.0f, 1.0f};

        
        //Depth Enable Setting
        GLES32.glClearDepthf(1.0f);
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        //Clear Color
        GLES32.glClearColor(0.0f,0.0f,0.0f,1.0f);

        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private void printGLInfo(GL10 gl)
    {
        //code
        System.out.println("ABC:OpenGLES Renderer: " + gl.glGetString(GL10.GL_RENDERER));
        System.out.println("ABC:OpenGLES Version :" + gl.glGetString(GL10.GL_VERSION));
        System.out.println("ABC:OpenGLES Shading Language Version :" + gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION));
    }

    private void resize(int width,int height)
    {
        //code
        if(height<=0)
            height = 1;

        GLES32.glViewport(0,0,width,height);

        Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
    }

    private void display()
    {
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];
        float[] rotationMatrix = new float[16];

        //code
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        GLES32.glUseProgram(shaderProgramObject);

        Matrix.setIdentityM(modelMatrix,0);
        Matrix.setIdentityM(viewMatrix,0);
        Matrix.setIdentityM(rotationMatrix,0);

        Matrix.rotateM(rotationMatrix,0,angle_pyramid,0.0f,1.0f,0.0f);
        Matrix.translateM(modelMatrix,0,0.0f,0.0f,-5.0f);

        Matrix.multiplyMM(modelMatrix,0,modelMatrix,0,rotationMatrix,0);


        // Push above ModelView Projection matrix into vertex shader's mvpUniform
        GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix,0);
        GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix,0);
        GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix,0);

        if (bLightingEnable == true)
        {
            GLES32.glUniform1i(keyPressUniform, 1); // Light uniforms

            // Ligh0 Uniform
            GLES32.glUniform3fv(lightDefuseUniform[0], 1, light[0].defuse,0);
            GLES32.glUniform3fv(lightAmbientUniform[0], 1, light[0].ambient,0);
            GLES32.glUniform3fv(lightSpecularUniform[0], 1, light[0].specular,0);
            GLES32.glUniform4fv(lightPositionUniform[0], 1, light[0].position,0);

            // Light1
            GLES32.glUniform3fv(lightDefuseUniform[1], 1, light[1].defuse,0);
            GLES32.glUniform3fv(lightAmbientUniform[1], 1, light[1].ambient,0);
            GLES32.glUniform3fv(lightSpecularUniform[1], 1, light[1].specular,0);
            GLES32.glUniform4fv(lightPositionUniform[1], 1, light[1].position,0);

            // Material Uniforms
            GLES32.glUniform3fv(materialDefuseUniform, 1, materialDefuse,0);
            GLES32.glUniform3fv(materialAmbientUniform, 1, materialAmbient,0);
            GLES32.glUniform3fv(materialSpecularUniform, 1, materialSpecular,0);
            GLES32.glUniform1f(materialShininessUniform, materialShininess);
        }
        else
        {
            GLES32.glUniform1i(keyPressUniform, 0);
        }

        GLES32.glBindVertexArray(vaoPyramid[0]);

        GLES32.glDrawArrays(GLES32.GL_TRIANGLES,0,12);

        GLES32.glUseProgram(0);

        //Render
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

    private void uninitialise()
    {
        //code
        if(shaderProgramObject > 0)
        {
            int[] retVal = new int[1];

            GLES32.glUseProgram(shaderProgramObject);

            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS,retVal,0);

            int numShaders = retVal[0];

            if (numShaders > 0)
            {
                int[] pShaders = new int[numShaders];


                GLES32.glGetAttachedShaders(shaderProgramObject,numShaders,retVal,0,pShaders,0);


                    for (int i = 0; i < numShaders; i++)
                    {
                        GLES32.glDetachShader(shaderProgramObject, pShaders[i]);
                        GLES32.glDeleteShader(pShaders[i]);
                        pShaders[i] = 0;
                    }
            }

            GLES32.glUseProgram(0);
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }

        if(vaoPyramid[0]>0 )
        {
            GLES32.glDeleteVertexArrays(1,vaoPyramid,0);
            vaoPyramid[0]=0;
        }

        if(vboPyramidNormal[0] > 0)
        {
            GLES32.glDeleteBuffers(1,vboPyramidNormal,0);
            vboPyramidNormal[0]=0;
        }

        if(vboPyramidPosition[0] > 0)
        {
            GLES32.glDeleteBuffers(1,vboPyramidPosition,0);
            vboPyramidPosition[0] = 0;
        }


    }
}
