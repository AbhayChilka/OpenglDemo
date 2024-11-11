
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

//webgl related variables
const VertexAtrributEnum = {
                             AMC_ATTRIBUTE_POSITION : 0,
                             AMC_ATTRIBUTE_COLOR : 1,
                             AMC_ATTRIBUTE_NORMAL : 2
                           };

var ShaderProgramObeject = null;

//pyramid vao and vbo
var vao_pyramid = null;
var vbo_pyramid_position = null;
var vbo_pyramid_color = null;
var vbo_pyramid_normal = null;

//angle for rotation
var tAngle = 0.0;
var bLightEnable = false;
var bAnimationEnable = false;

var perspectiveProjetionMatrix;

var modelMatrixUniform = 0;
var viewMatrixUniform = 0;
var projectionMatrixUniform = 0;

var keyPressUniform = 0;

var lightAmbientUniform = [2];
var lightDefuseUniform = [2];
var lightSpecularUniform = [2];
var lightPositionUniform = [2];

var  materialAmbientUniform ;
var  materialDefuseUniform ;
var  materialSpecularUniform;
var materialShininessUniform;

var lightAmbient1 = [0.0, 0.0, 0.0];
var lightDefuse1 = [1.0, 0.0, 0.0];
var lightSpecular1 = [1.0, 0.0, 0.0];
var lightPosition1 = [-2.0, 0.0, 0.0, 1.0];

var lightAmbient2 = [0.0, 0.0, 0.0];
var lightDefuse2 = [0.0, 0.0, 1.0];
var lightSpecular2 = [0.0, 0.0, 1.0];
var lightPosition2 = [2.0, 0.0, 0.0, 1.0];

var materialAmbient = [0.0, 0.0, 0.0, 1.0];
var materialDefuse = [1.0, 1.0, 1.0, 1.0];
var materialSpecular = [1.0, 1.0, 1.0, 1.0];
var materialShininess = 128.0;

var uniform;

var requestAnimationFrame =
                            window.requestAnimationFrame ||         //chrome
                            window.webkitRequestAnimationFrame || //safari
                            window.mozRequestAnimationFrame ||     //moz
                            window.oRequestAnimationFrame ||        //opera
                            window.msRequestAnimationFrame;         //edge

//our main function
function main()
{
    //get canvas
    canvas = document.getElementById("ABC");
    if(canvas == null)
    {
        console.log("getting canvas failed");
    }
    else
    {
        console.log("getting canvas successeded");
    }

    //set canvas width and height for future use
    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;
    
    //register for keyboard events
    window.addEventListener("keydown", keyDown, false);

    //register for mouse events
    window.addEventListener("click", mouseDown, false);

    window.addEventListener("resize",resize, false);

    initialise();

    resize();

    display();
}

function keyDown(event)
{
    switch(event.keyCode)
    {
        case 81:
        case 113:
            uninitialise();
            window.close();
            break;
            
        case 70:
        case 102:
            toggleFullscreen();
            break;

        case 65:
        case 97:
            if (bAnimationEnable == false)
				{
					bAnimationEnable = true;
				}
				else
				{
					bAnimationEnable = false;
				}
            break;

        case 76:
        case 108:
            if (bLightEnable == false)
				{
					bLightEnable = true;
				}
				else
				{
					bLightEnable = false;
				}

            break;
        default:
            break;
    }

}

function mouseDown()
{
}

function toggleFullscreen()
{
    var fullscreen_element = 
                            document.fullscreenElement || 
                            document.webkitFullscreenElement ||
                            document.mozFullScreenElement ||
                            document.msFullscreenElement ||
                            null;

    //if not fullscreen
    if(fullscreen_element == null)
    {
        if(canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if(canvas.webkitRequestFullscreen)
            canvas.webkitRequestFullscreen();
        else if(canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if(canvas.msRequestFullscreen)
             canvas.msRequestFullscreen();

        bFullscreen = true;
    }
    else
    {
        if(document.exitFullscreen)
            document.exitFullscreen();
        else if(document.webkitExitFullscreen)
            document.webkitExitFullscreen();
        else if(document.mozCancelFullScreen)
            document.mozCancelFullScreen();
        else if(document.msExitFullscreen)
            document.msExitFullscreen();    
        
        bFullscreen = false;
    }
}

function initialise()
{
    //code
    //get context from above canvas
    gl = canvas.getContext("webgl2");
    if(gl == null)
    {
        console.log("getting webgl2 context failed");
    }
    else
    {
        console.log("getting webgl2 context successeded");
    }
    
    //set webgl2 context view width and view height properties
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    //VERTEX shader
    var vertexShaderSourceCode =
    "#version 300 es" +
    "\n" +
    "precision lowp int;"+
    "precision mediump float;"+
    "in vec4 aPosition;" +
    "in vec3 aNormal;"+
    "uniform mat4 uModelMatrix;"+
    "uniform mat4 uViewMatrix;"+
    "uniform mat4 uProjectionMatrix;"+
    "uniform vec4 uLightPosition[2];"+
    "uniform int uKeyPress;"+
    "out vec3 oTransformedNormals;"+
    "out vec3 oViewerVector;"+
    "out vec3 oLightDirection[2];"+
    "void main(void)"+
    "{"+
    "if(uKeyPress == 1)"+
    "{"+
    "vec4 iCoordinates = uViewMatrix * uModelMatrix * aPosition;"+
    "oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;"+
    "oViewerVector = -iCoordinates.xyz;"+
    "for(int i=0; i<2 ; i++)"+
    "{"+
    "oLightDirection[i] = vec3(uLightPosition[i] - iCoordinates);"+
    "}"+
    "}"+
    "else"+
    "{"+
    "oTransformedNormals = vec3(0.0f,0.0f,0.0f);"+
    "oViewerVector = vec3(0.0f,0.0f,0.0f);"+
    "oLightDirection[0] = vec3(0.0f,0.0f,0.0f);"+
    "oLightDirection[1] = vec3(0.0f,0.0f,0.0f);"+
    "}"+
    "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"+
    "}";

    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if(gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)== false)
    {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if(error.length > 0)
            {
                var log = "Vertex Shader Compilation error : " + error;
                alert(log);
                uninitialise();
            }
    }
    else
    {
        console.log("Vertex Shader Compile Successfully");
    }

    //Fragment shader
    var fragmentShaderSourceCode = 
    "#version 300 es" +
    "\n" +
    "precision lowp int;"+
    "precision mediump float;"+
    "in vec3 oTransformedNormals;"+
    "in vec3 oViewerVector;"+
    "in vec3 oLightDirection[2];"+
    "uniform vec3 uLightAmbient[2];"+
    "uniform vec3 uLightDefuse[2];"+
    "uniform vec3 uLightSpecular[2];"+
    "uniform vec3 uMaterialAmbient;"+
    "uniform vec3 uMaterialDefuse;"+
    "uniform vec3 uMaterialSpecular;"+
    "uniform float uMaterialShininess;"+
    "uniform int uKeyPress;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
    "vec3 phongADSLight = vec3(0.0f,0.0f,0.0f);"+
    "if(uKeyPress == 1)"+
    "{"+
    "vec3 viewerVector = normalize(oViewerVector);"+
    "vec3 transformedNormals = normalize(oTransformedNormals);"+
    "vec3 lightDirection[2];"+
    "vec3 defuseLight[2];"+
    "vec3 ambientLight[2];"+
    "vec3 specularLight[2];"+
    "vec3 reflectionVector[2];"+
    "for(int i=0; i<2 ; i++)"+
    "{"+
    "lightDirection[i] = normalize(oLightDirection[i]);"+
    "reflectionVector[i] = reflect(-lightDirection[i],transformedNormals);"+
    "ambientLight[i] = uLightAmbient[i] * uMaterialAmbient;"+
    "defuseLight[i] = uLightDefuse[i] * uMaterialDefuse * max(dot(lightDirection[i],transformedNormals),0.0);"+
    "specularLight[i] = uLightSpecular[i] * uMaterialSpecular * pow(max(dot(reflectionVector[i],viewerVector),0.0),uMaterialShininess);"+
    "phongADSLight = phongADSLight + ambientLight[i] + defuseLight[i] + specularLight[i];"+
    "}"+
    "}"+
    "else"+
    "{"+
    "phongADSLight = vec3(1.0f,1.0f,1.0f);"+
    "}"+
    "FragColor = vec4(phongADSLight,1.0f);"+
"}";

    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);

    if(gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS)== false)
    {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if(error.length > 0)
            {
                var log = "Fragment Shader Compilation error : " + error;
                alert(log);
                uninitialise();
            }
    }
    else
    {
        console.log("Fragment Shader Compile Successfully");
    }
                                   
    //SHADER program
    ShaderProgramObeject = gl.createProgram();

    //attach 2 shader program
    gl.attachShader(ShaderProgramObeject, vertexShaderObject);
    gl.attachShader(ShaderProgramObeject, fragmentShaderObject);

    //pre linking binding
    gl.bindAttribLocation(ShaderProgramObeject, VertexAtrributEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
    gl.bindAttribLocation(ShaderProgramObeject, VertexAtrributEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
    gl.linkProgram(ShaderProgramObeject);

    if(gl.getProgramParameter(ShaderProgramObeject, gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramInfoLog(ShaderProgramObeject);
        if(error.length > 0)
            {
                var log = "Shader Program Linking error : " + error;
            }
            alert(log);
            uninitialise();
        }
    else
    {
        console.log("Shader Program Link Successfully");
    }

 	keyPressUniform = gl.getUniformLocation(ShaderProgramObeject, "uKeyPress");
    modelMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uModelMatrix");
    viewMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uProjectionMatrix");

    lightAmbientUniform[0] = gl.getUniformLocation(ShaderProgramObeject, "uLightAmbient[0]");
    lightDefuseUniform[0] = gl.getUniformLocation(ShaderProgramObeject, "uLightDefuse[0]");
    lightSpecularUniform[0] = gl.getUniformLocation(ShaderProgramObeject, "uLightSpecular[0]");
    lightPositionUniform[0] = gl.getUniformLocation(ShaderProgramObeject, "uLightPosition[0]");

    lightAmbientUniform[1] = gl.getUniformLocation(ShaderProgramObeject, "uLightAmbient[1]");
    lightDefuseUniform[1] = gl.getUniformLocation(ShaderProgramObeject, "uLightDefuse[1]");
    lightSpecularUniform[1] = gl.getUniformLocation(ShaderProgramObeject, "uLightSpecular[1]");
    lightPositionUniform[1] = gl.getUniformLocation(ShaderProgramObeject, "uLightPosition[1]");

    materialAmbientUniform = gl.getUniformLocation(ShaderProgramObeject, "uMaterialAmbient");
    materialDefuseUniform = gl.getUniformLocation(ShaderProgramObeject, "uMaterialDefuse");
    materialSpecularUniform =  gl.getUniformLocation(ShaderProgramObeject, "uMaterialSpecular");
    materialShininessUniform =  gl.getUniformLocation(ShaderProgramObeject, "uMaterialShininess");
   
    //geometry 

    var pyramid_position = new Float32Array([
        // front
        0.0,  1.0,  0.0, // front-top
        -1.0, -1.0,  1.0, // front-left
        1.0, -1.0,  1.0, // front-right
        
        // right
        0.0,  1.0,  0.0, // right-top
        1.0, -1.0,  1.0, // right-left
        1.0, -1.0, -1.0, // right-right

        // back
        0.0,  1.0,  0.0, // back-top
        1.0, -1.0, -1.0, // back-left
        -1.0, -1.0, -1.0, // back-right

        // left
        0.0,  1.0,  0.0, // left-top
        -1.0, -1.0, -1.0, // left-left
        -1.0, -1.0,  1.0 // left-right
            ]);

    var pyramid_color = new Float32Array([
                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0,

                1.0, 0.0, 0.0,
                0.0, 0.0, 1.0,
                0.0, 1.0, 0.0,

                1.0, 0.0, 0.0,
                0.0, 1.0, 0.0,
                0.0, 0.0, 1.0,

                1.0, 0.0, 0.0,
                0.0, 0.0, 1.0,
                0.0, 1.0, 0.0
                ]);
                
    var pyramid_normal = new Float32Array([
        0.000000, 0.447214, 0.894427, // front-top
        0.000000, 0.447214, 0.894427, // front-left
        0.000000, 0.447214, 0.894427, // front-right

        // right
        0.894427, 0.447214, 0.000000, // right-top
        0.894427, 0.447214, 0.000000, // right-left
        0.894427, 0.447214, 0.000000, // right-right

        // back
        0.000000, 0.447214, -0.894427, // back-top
        0.000000, 0.447214, -0.894427, // back-left
        0.000000, 0.447214, -0.894427, // back-right

        // left
        -0.894427, 0.447214, 0.000000, // left-top
        -0.894427, 0.447214, 0.000000, // left-left
        -0.894427, 0.447214, 0.000000, // left-right

    ])

    //vao
    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

    //vbo
    vbo_pyramid_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_position);

    gl.bufferData(gl.ARRAY_BUFFER, pyramid_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo normal
    vbo_pyramid_normal = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_normal);

    gl.bufferData(gl.ARRAY_BUFFER, pyramid_normal, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_NORMAL);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);

    //DEPTH                             
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    //set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    //initialise projection matrix
    perspectiveProjetionMatrix = mat4.create();
}

function resize()
{
    //code
    if(bFullscreen == true)
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else
    {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    //set viewport
    gl.viewport(0, 0,canvas.width, canvas.height);

    ///set perspective position
    mat4.perspective(perspectiveProjetionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0); 

}

function display()
{
    //code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(ShaderProgramObeject);

    var modelMatrix = mat4.create();
    var modelrotateMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.rotateY(modelrotateMatrix, modelrotateMatrix, (tAngle * Math.PI) / 180.0);
    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -5.0]);
    mat4.multiply(modelMatrix, modelMatrix, modelrotateMatrix);
   // mat4.multiply(viewMatrix, perspectiveProjetionMatrix, modelMatrix);

    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjetionMatrix);

    if(bLightEnable == true)
    {
        gl.uniform1i(keyPressUniform, 1);

        gl.uniform3f(lightAmbientUniform[0], lightAmbient1[0] , lightAmbient1[1] , lightAmbient1[2]);
        gl.uniform3f(lightDefuseUniform[0], lightDefuse1[0] , lightDefuse1[1] , lightDefuse1[2] );
        gl.uniform3f(lightSpecularUniform[0], lightSpecular1[0] , lightSpecular1[1] , lightSpecular1[2]);
        gl.uniform4f(lightPositionUniform[0], lightPosition1[0] , lightPosition1[1] ,lightPosition1[2] ,lightPosition1[3]);

        gl.uniform3f(lightAmbientUniform[1], lightAmbient2[0] , lightAmbient2[1] , lightAmbient2[2]);
        gl.uniform3f(lightDefuseUniform[1], lightDefuse2[0] , lightDefuse2[1] , lightDefuse2[2]);
        gl.uniform3f(lightSpecularUniform[1], lightSpecular2[0] , lightSpecular2[1] , lightSpecular2[2]);
        gl.uniform4f(lightPositionUniform[1], lightPosition2[0] , lightPosition2[1] ,lightPosition2[2] ,lightPosition1[3]);

        gl.uniform3f(materialAmbientUniform, materialAmbient[0] , materialAmbient[1] , materialAmbient[2]);
        gl.uniform3f(materialDefuseUniform, materialDefuse[0] , materialDefuse[1] , materialDefuse[2]);
        gl.uniform3f(materialSpecularUniform, materialSpecular[0] , materialSpecular[1] , materialSpecular[2]);
        gl.uniform1f(materialShininessUniform, materialShininess);
    }
    else
    {
        gl.uniform1i(keyPressUniform, 0);
    }

    gl.bindVertexArray(vao_pyramid);

    gl.drawArrays(gl.TRIANGLES, 0, 12);

    gl.bindVertexArray(null);

    gl.useProgram(null);

    if (bAnimationEnable == true)
        {
            update();
        }

    //set double buffering
    requestAnimationFrame(display, canvas);
}

function update()
{
    //code
	tAngle = tAngle + 0.1;
	if (tAngle >= 360.0)
	{
		tAngle = tAngle - 360.0;
	}

}

function uninitialise()
{
    //code
    if(ShaderProgramObeject)   //this can be != null
    {
        gl.useProgram(ShaderProgramObeject);
        var shaderObjects = gl.getAttachedShaders(ShaderProgramObeject);
        if(shaderObjects && shaderObjects.length > 0)
        {
            for(let i = 0; i < shaderObjects.length; i++)
            {
                gl.detachShader(ShaderProgramObeject, shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(ShaderProgramObeject);
        ShaderProgramObeject = null;
    }

    //square
    //vbo
    if(vbo_pyramid_normal)
        {
            gl.deleteBuffer(vbo_pyramid_normal);
            vbo_pyramid_normal = null;
        }

    if(vbo_pyramid_position)
        {
            gl.deleteBuffer(vbo_pyramid_position);
            vbo_pyramid_position = null;
        }
    
        //vao
        if(vao_pyramid)
        {
            gl.deleteVertexArray(vao_pyramid);
            vao_pyramid = null;
        }
}