
var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

//webgl related variables
const VertexAtrributEnum = {
                             AMC_ATTRIBUTE_POSITION : 0,
                             AMC_ATTRIBUTE_COLOR : 1
                           };

var ShaderProgramObeject = null;

var vao_cube = null;
var vbo_cube_position = null;
var vbo_square_color = null;

var vao_pyramid = null;
var vbo_pyramid_position = null;
var vbo_pyramid_color = null;

var mvpMatrixUniform;
var perspectiveProjetionMatrix;

var tAngle = 0.0;
var sAngle = 0.0;

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
                                        "in vec4 aPosition;" +
                                        "in vec4 aColor;" +
                                        "uniform mat4 uMVPMatrix;" +
                                        "out vec4 oColor;" +
                                        "void main(void)" +
                                        "{" +
                                        "gl_Position= uMVPMatrix*aPosition;" +
                                        "oColor=aColor;" +
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
                                        "precision highp float;" +
                                        "in vec4 oColor;" +
                                        "out vec4 FragColor;" +
                                        "void main(void)" +
                                        "{" +
                                        "FragColor=oColor;" +
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
    gl.bindAttribLocation(ShaderProgramObeject, VertexAtrributEnum.AMC_ATTRIBUTE_COLOR, "aColor");
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

    mvpMatrixUniform = gl.getUniformLocation(ShaderProgramObeject, "uMVPMatrix");

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

    //vao
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    //vbo
    vbo_cube_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_position);

    gl.bufferData(gl.ARRAY_BUFFER, cube_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vbo_cube_color = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_color);

    gl.bufferData(gl.ARRAY_BUFFER, cube_color, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);

    //triangle    
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

    vbo_pyramid_color = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_color);

    gl.bufferData(gl.ARRAY_BUFFER, pyramid_color, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_COLOR);
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

    //transformation
    var modelViewMatrix = mat4.create();
    var modelViewProjection = mat4.create();
    var modelrotateMatrix = mat4.create();
    var modelscaleMatrix = mat4.create();

    mat4.scale(modelscaleMatrix, modelscaleMatrix, (0.75, 0.75, 0.75));
    mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -6.0]);
    mat4.rotateX(modelrotateMatrix, modelrotateMatrix, (sAngle * Math.PI) / 180.0);
    mat4.rotateY(modelrotateMatrix, modelrotateMatrix, (sAngle * Math.PI) / 180.0);
    mat4.rotateZ(modelrotateMatrix, modelrotateMatrix, (sAngle * Math.PI) / 180.0);
    mat4.multiply(modelViewMatrix, modelViewMatrix, modelrotateMatrix);
    mat4.multiply(modelViewProjection, perspectiveProjetionMatrix, modelViewMatrix);
    mat4.multiply(modelViewMatrix, modelViewMatrix, modelscaleMatrix);

    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjection);

    gl.bindVertexArray(vao_cube);

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
   
    gl.bindVertexArray(null);

    var modelViewMatrix = mat4.create();
    var modelViewProjection = mat4.create();
    var modelrotateMatrix = mat4.create();

    mat4.rotateY(modelrotateMatrix, modelrotateMatrix, (tAngle * Math.PI) / 180.0);
    mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -6.0]);
    mat4.multiply(modelViewMatrix, modelViewMatrix, modelrotateMatrix);
    mat4.multiply(modelViewProjection, perspectiveProjetionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjection);

    gl.bindVertexArray(vao_pyramid);

    gl.drawArrays(gl.TRIANGLES, 0, 12);

    gl.bindVertexArray(null);

    gl.useProgram(null);

    update();
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
    if(vbo_cube_color)
        {
            gl.deleteBuffer(vbo_cube_color);
            vbo_cube_color = null;
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

    //triangle
    //vbo
    if(vbo_pyramid_color)
        {
            gl.deleteBuffer(vbo_pyramid_color);
            vbo_pyramid_color = null;
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