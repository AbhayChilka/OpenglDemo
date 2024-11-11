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

var ldUniform = 0;
var kdUniform = 0;
var lightPositionUniform = 0;
var keyPressUniform =0;

var bLightEnable = false;
var bAnimationEnable = false;

var lightDefuse = [1.0, 1.0, 1.0];
var materialDefuse = [0.5, 0.5, 0.5];
var lightPosition = [0.0, 0.0, 2.0, 1.0];

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
                                        "precision lowp int;"+
                                        "precision mediump float;"+
                                        "in vec4 aPosition;" +
                                        "in vec3 aNormal;" +
                                        "uniform mat4 uModelViewMatrix;" +
                                        "uniform mat4 uProjectionMatrix;" +
                                        "uniform vec3 uLd;" +
                                        "uniform vec3 uKd;"+
                                        "uniform vec4 uLightPosition;" +
                                        "uniform highp int uKeyPress;" +
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
                                            "else"+
                                            "{"+
                                            "oDefuseLight = vec3(0.0, 0.0, 0.0);"+
                                            "}"+
                                            "gl_Position = uProjectionMatrix * uModelViewMatrix * aPosition;"+
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
                                        "precision lowp int;"+
                                        "precision mediump float;"+
                                        "in vec4 oColor;" +
                                        "in vec3 oDefuseLight;"+
                                        "uniform highp int uKeyPress;" +
                                        "out vec4 FragColor;" +
                                        "void main(void)" +
                                        "{" +
                                            "if(uKeyPress == 1)"+
                                            "{"+
                                                "FragColor = vec4(oDefuseLight, 1.0);"+
                                            "}"+
                                            "else"+
                                            "{"+
                                                "FragColor = vec4(1.0, 0.0, 0.0, 1.0);"+
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

    modelViewMatrixUniform =  gl.getUniformLocation(ShaderProgramObeject, "uModelViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uProjectionMatrix");
    ldUniform = gl.getUniformLocation(ShaderProgramObeject, "uLd");
	kdUniform = gl.getUniformLocation(ShaderProgramObeject, "uKd");
	lightPositionUniform = gl.getUniformLocation(ShaderProgramObeject, "uLightPosition");
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
    var modelViewMatrix = mat4.create();
    var modelViewProjection = mat4.create();
    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -5.0]);
    mat4.multiply(modelViewProjection, perspectiveProjetionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(modelViewMatrixUniform, false, modelViewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjetionMatrix);

    if(bLightEnable == true)
        {
            gl.uniform1i(keyPressUniform, 1);
            gl.uniform3fv(ldUniform,lightDefuse);
            gl.uniform3fv(kdUniform,materialDefuse);
            gl.uniform4fv(lightPositionUniform,lightPosition);
        }
        else
        {
            gl.uniform1i(keyPressUniform, 0);
        }
    
    sphere.draw();

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