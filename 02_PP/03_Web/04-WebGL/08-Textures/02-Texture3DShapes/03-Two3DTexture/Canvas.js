var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

//webgl related variables
const VertexAtrributEnum = {
                             AMC_ATTRIBUTE_POSITION : 0,
                             AMC_ATTRIBUTE_COLOR : 1,
                             AMC_ATTRIBUTE_TEXCOORD : 2
                           };

var ShaderProgramObeject = null;

var vao_cube = null;
var vbo_cube_position = null;
var vbo_square_color = null;
var vbo_cube_texcoord = null;

var vao_pyramid = null;
var vbo_pyramid_position = null;
var vbo_pyramid_color = null;
var vbo_pyramid_texcoord = null;

var mvpMatrixUniform;
var textureSamplerUniform;
var perspectiveProjetionMatrix;

var tAngle = 0.0;
var sAngle = 0.0;

var texture_stone = null;
var texture_kundali = null;

var textureSamplerUniform;

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

function loadGLTexture(filename) 
{
    var texture = gl.createTexture();

    texture.image = new Image();
    texture.image.src = filename;
    texture.image.onload = function () 
    {
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, texture.image);

        gl.generateMipmap(gl.TEXTURE_2D);
        gl.bindTexture(gl.TEXTURE_2D, null);

    }
    return texture;
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
                                        "in vec2 aTexCord;" +
                                        "uniform mat4 uMVPMatrix;" +
                                        "out vec2 oTexCord;" +
                                        "void main(void)" +
                                        "{" +
                                        "gl_Position=aPosition;"+
                                        "gl_Position= uMVPMatrix*aPosition;" +
                                        "oTexCord = aTexCord ;" +
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
                                        "in vec2 oTexCord;" +
                                        "uniform sampler2D uTextureSampler;"+
                                        "out vec4 FragColor;" +
                                        "void main(void)" +
                                        "{" +
                                        "FragColor= texture(uTextureSampler,oTexCord);" +
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
    gl.bindAttribLocation(ShaderProgramObeject, VertexAtrributEnum.AMC_ATTRIBUTE_TEXCOORD, "aTexCord");
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
	textureSamplerUniform = gl.getUniformLocation(ShaderProgramObeject, "uTextureSampler");

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

    var cube_texcoord = new Float32Array([
								// front
								1.0, 1.0, // top-right of front
								0.0, 1.0, // top-left of front
								0.0, 0.0, // bottom-left of front
								1.0, 0.0, // bottom-right of front

								// right
								1.0, 1.0, // top-right of right
								0.0, 1.0, // top-left of right
								0.0, 0.0, // bottom-left of right
								1.0, 0.0, // bottom-right of right

								// back
								1.0, 1.0, // top-right of back
								0.0, 1.0, // top-left of back
								0.0, 0.0, // bottom-left of back
								1.0, 0.0, // bottom-right of back

								// left
								1.0, 1.0, // top-right of left
								0.0, 1.0, // top-left of left
								0.0, 0.0, // bottom-left of left
								1.0, 0.0, // bottom-right of left

								// top
								1.0, 1.0, // top-right of top
								0.0, 1.0, // top-left of top
								0.0, 0.0, // bottom-left of top
								1.0, 0.0, // bottom-right of top

								// bottom
								1.0, 1.0, // top-right o bottom
								0.0, 1.0, // top-left o bottom
								0.0, 0.0, // bottom-let of bottom
								1.0, 0.0 // bottom-riht of bottom

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

    //vbo texcoord
    vbo_cube_texcoord = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_texcoord);

    gl.bufferData(gl.ARRAY_BUFFER, cube_texcoord, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_TEXCOORD);
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
         
    var pyramid_texcoord = new Float32Array([
                                            // front
                                            1.0, 1.0,
                                            0.0, 0.0,
                                            2.0, 0.0,

                                            // right
                                            1.0, 1.0,
                                            0.0, 0.0,
                                            2.0, 0.0,

                                            // rear
                                            1.0, 1.0,
                                            2.0, 0.0,
                                            0.0, 0.0,

                                            // left
                                            1.0, 1.0,
                                            2.0, 0.0,
                                            0.0, 0.0
                                        ]);
                                            
    //vao
    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

    //vbo position
    vbo_pyramid_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_position);

    gl.bufferData(gl.ARRAY_BUFFER, pyramid_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //vbo texcoord
    vbo_pyramid_texcoord = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_pyramid_texcoord);

    gl.bufferData(gl.ARRAY_BUFFER, pyramid_texcoord, gl.STATIC_DRAW);
    gl.vertexAttribPointer(VertexAtrributEnum.AMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(VertexAtrributEnum.AMC_ATTRIBUTE_TEXCOORD);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);
   
    //Load Texture Stone 
    texture_stone = loadGLTexture("stone.png");

    if (texture_stone == null) {
        alert("Texture Stone Loading Failed");
        uninitialise();
    } else {
        console.log("Texture Stone Loading Successfull");
    }
    
    //Load Texture Kundali
    texture_kundali = loadGLTexture("Vijay_Kundali.png");

    if (texture_stone == null) {
        alert("Texture Kundali Loading Failed");
        uninitialise();
    } else {
        console.log("Texture Kundali Loading Successfull");
    }

    //DEPTH                             
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    //Set WebGL co
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

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

	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, texture_kundali);
	gl.uniform1i(textureSamplerUniform, 0);
    
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

	//for texture
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, texture_stone);
	gl.uniform1i(textureSamplerUniform, 0);
    
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
    if(vbo_cube_texcoord)
        {
            gl.deleteBuffer(vbo_cube_texcoord);
            vbo_cube_texcoord = null;
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
    if(vbo_pyramid_texcoord)
        {
            gl.deleteBuffer(vbo_pyramid_texcoord);
            vbo_pyramid_texcoord = null;
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
    
        if (texture_kundali)
            {
                gl.deleteTextures(1, texture_kundali);
                texture_kundali = 0;
            }
            
        if (texture_stone)
        {
            gl.deleteTextures(1, texture_stone);
            texture_stone = 0;
        }    
}