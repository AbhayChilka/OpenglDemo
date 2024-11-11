var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

//webgl related variables
const VertexAtrributEnum = {
                             AMC_ATTRIBUTE_POSITION : 0,
                             AMC_ATTRIBUTE_COLOR : 1,
                             AMC_ATTRIBUTE_NORMAL : 2,
                             AMC_ATTRIBUTE_TEXCOORD : 3
                           };

var ShaderProgramObeject = null;

var perspectiveProjetionMatrix;

var modelViewMatrixUniform;
var projectionMatrixUniform;
var modelMatrixUniform;

var keyPressUniform = 0;

//light
var lightAmbientUniform  = 0;
var lightDefuseUniform  = 0;
var lightSpecularUniform  = 0;
var lightPositionUniform  = 0;

//material
var materialAmbientUniform  = 0;
var materialDefuseUniform  = 0;
var materialSpecularUniform  = 0;
var materialShininessUniform  = 0;

var bLightEnable = false;

var lightAmbient = new Float32Array([0.1, 0.1, 0.1, 1.0]);
var lightDefuse = new Float32Array([1.0, 1.0, 1.0, 1.0]);
var lightSpecular = new Float32Array([1.0, 1.0, 1.0, 1.0]);
var lightPosition =  new Float32Array([100.0, 100.0, 100.0, 1.0]);

var materialAmbient = new Float32Array([0.0, 0.0, 0.0, 1.0]);
var materialDefuse = new Float32Array([1.0, 1.0, 1.0, 1.0]);
var materialSpecular = new Float32Array([1.0, 1.0, 1.0, 1.0]);
var materialShininess = 50.0;

var sphere = null;

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
                                    (
                                        "#version 300 es" +
                                        "\n" +
                                        "precision mediump int;"+
                                        "in vec4 aPosition;"+
                                        "in vec3 aNormal;"+
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
                                        "void main(void)"+
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
                                        "oFong_ADS_Light = vec3(1.0, 1.0, 1.0);"+
                                        "}"+
                                        "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"+
                                        "}"
                                     );

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
                                    (
                                        "#version 300 es" +
                                        "\n" +
                                        "precision highp float;"+
                                        "precision mediump int;"+
                                        "in vec3 oFong_ADS_Light;"+
                                        "uniform int uKeyPress;"+
                                        "out vec4 FragColor;"+
                                        "void main(void)"+
                                        "{"+
                                            " if(uKeyPress == 1)"+
                                            "{"+
                                                "FragColor = vec4(oFong_ADS_Light, 1.0);"+
                                            "}"+
                                            "else"+
                                            "{"+
                                                "FragColor = vec4(1.0, 1.0, 1.0, 1.0);"+
                                            "}"+
                                        "}"
                                    );

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

	modelMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uModelMatrix");
	modelViewMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uViewMatrix");
	projectionMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uProjectionMatrix");

	lightAmbientUniform = gl.getUniformLocation(ShaderProgramObeject, "uLightAmbient");
	lightDefuseUniform = gl.getUniformLocation(ShaderProgramObeject, "uLightDefuse");
	lightSpecularUniform = gl.getUniformLocation(ShaderProgramObeject, "uLightSpecular");
	lightPositionUniform = gl.getUniformLocation(ShaderProgramObeject, "uLightPosition");

	materialAmbientUniform = gl.getUniformLocation(ShaderProgramObeject, "uMaterialAmbient");
	materialDefuseUniform = gl.getUniformLocation(ShaderProgramObeject, "uMaterialDefuse");
	materialSpecularUniform = gl.getUniformLocation(ShaderProgramObeject, "uMaterialSpecular");
	materialShininessUniform = gl.getUniformLocation(ShaderProgramObeject, "uMaterialShininess");

	keyPressUniform = gl.getUniformLocation(ShaderProgramObeject, "uKeyPress");

    //geometry 
    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);
                                                                                    
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

    //transformation
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var modelViewProjection = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -5.0]);
    mat4.multiply(modelViewProjection, perspectiveProjetionMatrix, modelMatrix);

    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(modelViewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjetionMatrix);

    if(bLightEnable == true)
        {
            gl.uniform1i(keyPressUniform, 1);
            gl.uniform3f(lightAmbientUniform,  lightAmbient[0],lightAmbient[1],lightAmbient[2]);
            gl.uniform3f(lightDefuseUniform, lightDefuse[0], lightDefuse[1], lightDefuse[2]);
            gl.uniform3f(lightSpecularUniform,  lightSpecular[0], lightSpecular[1], lightSpecular[2]);
            gl.uniform4f(lightPositionUniform, lightPosition[0], lightPosition[1], lightPosition[2], lightPosition[3]);
            gl.uniform3f(materialAmbientUniform,  materialAmbient[0], materialAmbient[1], materialAmbient[2]);
            gl.uniform3f(materialDefuseUniform,  materialDefuse[0], materialDefuse[1], materialDefuse[2]);
            gl.uniform3f(materialSpecularUniform,  materialSpecular[0], materialSpecular[1], materialSpecular[2] );
            gl.uniform1f(materialShininessUniform, materialShininess);
        }
        else
        {
            gl.uniform1i(keyPressUniform, 0);
        }
    
    sphere.draw();

    gl.useProgram(null);

    update();
    //set double buffering
    requestAnimationFrame(display, canvas);
}

function update()
{
    //code
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

    if (sphere)
		{
			sphere.deallocate();
			sphere = null;
		}
}