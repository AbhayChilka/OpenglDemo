
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

var vao_cube = null;
var vbo_cube_position = null;
var vbo_square_color = null;
var vbo_cube_normal = null;

var sAngle = 0.0;

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

    modelViewMatrixUniform =  gl.getUniformLocation(ShaderProgramObeject, "uModelViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uProjectionMatrix");
    ldUniform = gl.getUniformLocation(ShaderProgramObeject, "uLd");
	kdUniform = gl.getUniformLocation(ShaderProgramObeject, "uKd");
	lightPositionUniform = gl.getUniformLocation(ShaderProgramObeject, "uLightPosition");
	keyPressUniform = gl.getUniformLocation(ShaderProgramObeject, "uKeyPress");

    //geometry 

    //square attribute array declation
    var cube_position = new Float32Array([
										// front
										1.0,  1.0,  1.0, // top-right of front
										-1.0,  1.0,  1.0, // top-left of front
										-1.0, -1.0,  1.0, // bottom-left of front
										1.0, -1.0,  1.0, // bottom-right of front

										// right
										1.0,  1.0, -1.0, // top-right of right
										1.0,  1.0,  1.0, // top-left of right
										1.0, -1.0,  1.0, // bottom-left of right
										1.0, -1.0, -1.0, // bottom-right of right

										// back
										1.0,  1.0, -1.0, // top-right of back
										-1.0,  1.0, -1.0, // top-left of back
										-1.0, -1.0, -1.0, // bottom-left of back
										1.0, -1.0, -1.0, // bottom-right of back

										// left
										-1.0,  1.0,  1.0, // top-right of left
										-1.0,  1.0, -1.0, // top-left of left
										-1.0, -1.0, -1.0, // bottom-left of left
										-1.0, -1.0,  1.0, // bottom-right of left

										// top
										1.0,  1.0, -1.0, // top-right of top
										-1.0, 1.0, -1.0, // top-left of top
										-1.0, 1.0,  1.0, // bottom-left of top
										1.0, 1.0, 1.0, // bottom-right of top

										// bottom
										1.0, -1.0,  1.0, // top-right of bottom
										-1.0, -1.0,  1.0, // top-left of bottom
										-1.0, -1.0, -1.0, // bottom-left of bottom
										1.0, -1.0, -1.0 // bottom-right of bottom
                                            ]);

    var cube_color = new Float32Array([
										// front
										1.0, 0.0, 0.0, // top-right of front
										1.0, 0.0, 0.0, // top-left of front
										1.0, 0.0, 0.0, // bottom-left of front
										1.0, 0.0, 0.0, // bottom-right of front

										// right
										0.0, 0.0, 1.0, // top-right of right
										0.0, 0.0, 1.0, // top-left of right
										0.0, 0.0, 1.0, // bottom-left of right
										0.0, 0.0, 1.0, // bottom-right of right

										// back
										1.0, 1.0, 0.0, // top-right of back
										1.0, 1.0, 0.0, // top-left of back
										1.0, 1.0, 0.0, // bottom-left of back
										1.0, 1.0, 0.0, // bottom-right of back

										// left
										1.0, 0.0, 1.0, // top-right of left
										1.0, 0.0, 1.0, // top-left of left
										1.0, 0.0, 1.0, // bottom-left of left
										1.0, 0.0, 1.0, // bottom-right of left

										// top
										0.0, 1.0, 0.0, // top-right of top
										0.0, 1.0, 0.0, // top-left of top
										0.0, 1.0, 0.0, // bottom-left of top
										0.0, 1.0, 0.0, // bottom-right of top

										// bottom
										1.0, 0.5, 0.0, // top-right of bottom
										1.0, 0.5, 0.0, // top-left of bottom
										1.0, 0.5, 0.0, // bottom-left of bottom
										1.0, 0.5, 0.0, // bottom-right of bottom
                                        ]);                  

    var cube_normal = new Float32Array([
										// front surface
										0.0,  0.0,  1.0, // top-right of front
										0.0,  0.0,  1.0, // top-left of front
										0.0,  0.0,  1.0, // bottom-left of front
										0.0,  0.0,  1.0, // bottom-right of front

										// right surface
										1.0,  0.0,  0.0, // top-right of right
										1.0,  0.0,  0.0, // top-left of right
										1.0,  0.0,  0.0, // bottom-left of right
										1.0,  0.0,  0.0, // bottom-right of right

										// back surface
										0.0,  0.0, -1.0, // top-right of back
										0.0,  0.0, -1.0, // top-left of back
										0.0,  0.0, -1.0, // bottom-left of back
										0.0,  0.0, -1.0, // bottom-right of back

										// left surface
										-1.0,  0.0,  0.0, // top-right of left
										-1.0,  0.0,  0.0, // top-left of left
										-1.0,  0.0,  0.0, // bottom-left of left
										-1.0,  0.0,  0.0, // bottom-right of left

										// top surface
										0.0,  1.0,  0.0, // top-right of top
										0.0,  1.0,  0.0, // top-left of top
										0.0,  1.0,  0.0, // bottom-left of top
										0.0,  1.0,  0.0, // bottom-right of top

										// bottom surface
										0.0, -1.0,  0.0, // top-right of bottom
										0.0, -1.0,  0.0, // top-left of bottom
										0.0, -1.0,  0.0, // bottom-left of bottom
										0.0, -1.0,  0.0 // bottom-right of bottom
                                ]);
    //vao
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    //vbo position
    vbo_cube_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_position);

    gl.bufferData(gl.ARRAY_BUFFER, cube_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo normal
    vbo_cube_normal = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_normal);

    gl.bufferData(gl.ARRAY_BUFFER, cube_normal, gl.STATIC_DRAW);
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

    //Transformations
    var modelViewMatrix = mat4.create();
    var rotationMatrixX = mat4.create();
    var rotationMatrixY = mat4.create();
    var rotationMatrixZ = mat4.create();


    //cube
    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -5.0]);
    mat4.rotateX(rotationMatrixX, rotationMatrixX, sAngle * Math.PI / 180.0);
    mat4.rotateY(rotationMatrixY, rotationMatrixY, sAngle * Math.PI / 180.0);
    mat4.rotateZ(rotationMatrixZ, rotationMatrixZ, sAngle * Math.PI / 180.0);


    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrixX);
    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrixY);
    mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrixZ);

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

    gl.bindVertexArray(vao_cube);
    
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

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
	sAngle = sAngle - 0.1;
	if (sAngle <= 0.0)
	{
		sAngle = sAngle + 360.0;
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
    if(vbo_cube_normal)
        {
            gl.deleteBuffer(vbo_cube_normal);
            vbo_cube_normal = null;
        }

    if(vbo_cube_position)
    {
        gl.deleteBuffer(vbo_cube_position);
        vbo_cube_position = null;
    }

    //vao
    if(vao_cube)
    {
        gl.deleteVertexArray(vao_cube);
        vao_cube = null;
    }    
}