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

var PerVertexShaderProgramObeject = null;
var PerFragmentShaderProgramObeject = null;

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
var chooseShader = 'v';

var lightAmbient = new Float32Array([0.1, 0.1, 0.1, 1.0]);
var lightDefuse = new Float32Array([1.0, 1.0, 1.0, 1.0]);
var lightSpecular = new Float32Array([1.0, 1.0, 1.0, 1.0]);
var lightPosition =  new Float32Array([100.0, 100.0, 100.0, 1.0]);

var materialAmbient = new Float32Array([0.0, 0.0, 0.0, 1.0]);
var materialDefuse = new Float32Array([0.5, 0.2, 0.7, 1.0]);
var materialSpecular = new Float32Array([0.7, 0.7, 0.7, 1.0]);
var materialShininess = 10.0;

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

        case 65:
        case 97:
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
        
        case 70:
        case 102:
            if(bLightEnable == true)
            {
                chooseShader = 'f';
                modelMatrixUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uModelMatrix");
                modelViewMatrixUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uViewMatrix");
                projectionMatrixUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uProjectionMatrix");
            
                lightAmbientUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uLightAmbient");
                lightDefuseUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uLightDefuse");
                lightSpecularUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uLightSpecular");
                lightPositionUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uLightPosition");
            
                materialAmbientUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uMaterialAmbient");
                materialDefuseUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uMaterialDefuse");
                materialSpecularUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uMaterialSpecular");
                materialShininessUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uMaterialShininess");
            
                keyPressUniform = gl.getUniformLocation(PerFragmentShaderProgramObeject, "uKeyPress");

            }
            break;
 
        case 86:
        case 118:
            if(bLightEnable == true)
            {
                chooseShader = 'v';
                modelMatrixUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uModelMatrix");
                modelViewMatrixUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uViewMatrix");
                projectionMatrixUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uProjectionMatrix");
            
                lightAmbientUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uLightAmbient");
                lightDefuseUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uLightDefuse");
                lightSpecularUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uLightSpecular");
                lightPositionUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uLightPosition");
            
                materialAmbientUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uMaterialAmbient");
                materialDefuseUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uMaterialDefuse");
                materialSpecularUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uMaterialSpecular");
                materialShininessUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uMaterialShininess");
            
                keyPressUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uKeyPress");
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

//-------------------------------------PerVertex Operations-------------------------------------\\

    //VERTEX shader
    var PerVertexvertexShaderSourceCode =  
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

    var PerVertexvertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(PerVertexvertexShaderObject, PerVertexvertexShaderSourceCode);
    gl.compileShader(PerVertexvertexShaderObject);

    if(gl.getShaderParameter(PerVertexvertexShaderObject, gl.COMPILE_STATUS)== false)
    {
        var error = gl.getShaderInfoLog(PerVertexvertexShaderObject);
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
    var PerVertexfragmentShaderSourceCode = 
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

    var PerVertexfragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(PerVertexfragmentShaderObject, PerVertexfragmentShaderSourceCode);
    gl.compileShader(PerVertexfragmentShaderObject);

    if(gl.getShaderParameter(PerVertexfragmentShaderObject, gl.COMPILE_STATUS)== false)
    {
        var error = gl.getShaderInfoLog(PerVertexfragmentShaderObject);
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
    PerVertexShaderProgramObeject = gl.createProgram();

    //attach 2 shader program
    gl.attachShader(PerVertexShaderProgramObeject, PerVertexvertexShaderObject);
    gl.attachShader(PerVertexShaderProgramObeject, PerVertexfragmentShaderObject);

    //pre linking binding
    gl.bindAttribLocation(PerVertexShaderProgramObeject, VertexAtrributEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
    gl.bindAttribLocation(PerVertexShaderProgramObeject, VertexAtrributEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
    gl.linkProgram(PerVertexShaderProgramObeject);

    if(gl.getProgramParameter(PerVertexShaderProgramObeject, gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramInfoLog(PerVertexShaderProgramObeject);
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

    modelMatrixUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uModelMatrix");
    modelViewMatrixUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uProjectionMatrix");

    lightAmbientUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uLightAmbient");
    lightDefuseUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uLightDefuse");
    lightSpecularUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uLightSpecular");
    lightPositionUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uLightPosition");

    materialAmbientUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uMaterialAmbient");
    materialDefuseUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uMaterialDefuse");
    materialSpecularUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uMaterialSpecular");
    materialShininessUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uMaterialShininess");

    keyPressUniform = gl.getUniformLocation(PerVertexShaderProgramObeject, "uKeyPress");

//--------------------------------------------------------------------------------------\\  

//-------------------------------------PerFragment Operations-------------------------------------\\
    //VERTEX shader
    var PerFragmentvertexShaderSourceCode =  
                                    (
                                        "#version 300 es" +
                                        "\n" +
                                        "precision mediump int;"+
                                        "in vec4 aPosition;"+
                                        "in vec3 aNormal;"+
                                        "uniform mat4 uModelMatrix;"+
                                        "uniform mat4 uViewMatrix;"+
                                        "uniform mat4 uProjectionMatrix;"+
                                        "uniform vec4 uLightPosition;"+
                                        "uniform int uKeyPress;"+
                                        "out vec3 otransformNormals;"+
                                        "out vec3 olightDirection;"+
                                        "out vec3 oviewverVector;"+
                                        "void main(void)"+
                                        "{"+
                                            "if(uKeyPress == 1)"+
                                            "{"+
                                                "vec4 iCoordinates = uViewMatrix * uModelMatrix * aPosition;"+
                                                "otransformNormals = mat3(uViewMatrix * uModelMatrix)*aNormal;"+
                                                "olightDirection = vec3(uLightPosition - iCoordinates);"+
                                                "oviewverVector = -iCoordinates.xyz;"+
                                            "}"+
                                            "else"+
                                            "{"+
                                                "otransformNormals = vec3(0.0, 0.0, 0.0);"+
                                                "olightDirection = vec3(0.0, 0.0, 0.0);"+
                                                "oviewverVector = vec3(0.0, 0.0, 0.0);"+
                                            "}"+
                                        "gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;"+
                                        "}"
                                    );

    var PerFragmentvertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(PerFragmentvertexShaderObject, PerFragmentvertexShaderSourceCode);
    gl.compileShader(PerFragmentvertexShaderObject);

    if(gl.getShaderParameter(PerFragmentvertexShaderObject, gl.COMPILE_STATUS)== false)
    {
        var error = gl.getShaderInfoLog(PerFragmentvertexShaderObject);
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
    var PerFragmentfragmentShaderSourceCode = 
                                    (
                                        "#version 300 es" +
                                        "\n" +
                                        "precision highp float;"+
                                        "precision mediump int;"+
                                        "in vec3 otransformNormals;"+
                                        "in vec3 olightDirection;"+
                                        "in vec3 oviewverVector;"+
                                        "uniform vec3 uLightAmbient;"+
                                        "uniform vec3 uLightDefuse;"+
                                        "uniform vec3 uLightSpecular;"+
                                        "uniform vec3 uMaterialAmbient;"+
                                        "uniform vec3 uMaterialDefuse;"+
                                        "uniform vec3 uMaterialSpecular;"+
                                        "uniform float uMaterialShininess;"+
                                        "uniform int uKeyPress;"+
                                        "out vec4 FragColor;"+
                                        "void main(void)"+
                                        "{"+
                                        "vec3 Phong_ADS_Light;"+
                                           " if(uKeyPress == 1)"+
                                            "{"+
                                                "vec3 normalizedTransformNormals = normalize(otransformNormals);"+
                                                "vec3 normalizedLightDirection = normalize(olightDirection);"+
                                                "vec3 normalizedViewverVector = normalize( oviewverVector);"+
                                                "vec3 ambientLight = uLightAmbient * uMaterialAmbient;"+
                                                "vec3 defuseLight = uLightDefuse * uMaterialDefuse *max(dot(normalizedLightDirection, normalizedTransformNormals), 0.0);"+
                                                "vec3 reflectionVector = reflect(-normalizedLightDirection, normalizedTransformNormals);"+
                                                "vec3 specularLight = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector, normalizedViewverVector ), 0.0), uMaterialShininess);"+
                                                "Phong_ADS_Light = ambientLight + defuseLight + specularLight;"+
                                            "}"+
                                            "else"+
                                            "{"+
                                                "Phong_ADS_Light = vec3(1.0,1.0, 1.0);"+
                                            "}"+
                                            "FragColor = vec4(Phong_ADS_Light, 1.0);"+
                                        "}"
                                    );

    var PerFragmentfragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(PerFragmentfragmentShaderObject, PerFragmentfragmentShaderSourceCode);
    gl.compileShader(PerFragmentfragmentShaderObject);

    if(gl.getShaderParameter(PerFragmentfragmentShaderObject, gl.COMPILE_STATUS)== false)
    {
        var error = gl.getShaderInfoLog(PerFragmentfragmentShaderObject);
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
    PerFragmentShaderProgramObeject = gl.createProgram();

    //attach 2 shader program
    gl.attachShader(PerFragmentShaderProgramObeject, PerFragmentvertexShaderObject);
    gl.attachShader(PerFragmentShaderProgramObeject, PerFragmentfragmentShaderObject);

    //pre linking binding
    gl.bindAttribLocation(PerFragmentShaderProgramObeject, VertexAtrributEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
    gl.bindAttribLocation(PerFragmentShaderProgramObeject, VertexAtrributEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
    gl.linkProgram(PerFragmentShaderProgramObeject);

    if(gl.getProgramParameter(PerFragmentShaderProgramObeject, gl.LINK_STATUS) == false)
    {
        var error = gl.getProgramInfoLog(PerFragmentShaderProgramObeject);
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

//--------------------------------------------------------------------------------------\\  

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

    if(chooseShader == 'v')
    {
        gl.useProgram(PerVertexShaderProgramObeject);
    }
    else if(chooseShader == 'f')
    {
        gl.useProgram(PerFragmentShaderProgramObeject);

    }
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
    if(PerVertexShaderProgramObeject)   //this can be != null
    {
        gl.useProgram(PerVertexShaderProgramObeject);
        var shaderObjects = gl.getAttachedShaders(PerVertexShaderProgramObeject);
        if(shaderObjects && shaderObjects.length > 0)
        {
            for(let i = 0; i < shaderObjects.length; i++)
            {
                gl.detachShader(PerVertexShaderProgramObeject, shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(PerVertexShaderProgramObeject);
        PerVertexShaderProgramObeject = null;
    }    

    if(PerFragmentShaderProgramObeject)   //this can be != null
    {
        gl.useProgram(PerFragmentShaderProgramObeject);
        var shaderObjects = gl.getAttachedShaders(PerFragmentShaderProgramObeject);
        if(shaderObjects && shaderObjects.length > 0)
        {
            for(let i = 0; i < shaderObjects.length; i++)
            {
                gl.detachShader(PerFragmentShaderProgramObeject, shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;
            }
        }
        gl.useProgram(null);
        gl.deleteProgram(PerFragmentShaderProgramObeject);
        PerFragmentShaderProgramObeject = null;
    }    

    if (sphere)
		{
			sphere.deallocate();
			sphere = null;
		}
}