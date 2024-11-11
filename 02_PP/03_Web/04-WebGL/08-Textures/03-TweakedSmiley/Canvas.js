var canvas = null;
var gl = null;
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;

//WebGL Related Variables
const vertexAttributeEnum =
{
    AMC_ATTRIBUTE_POSITION: 0,
    AMC_ATTRIBUTE_COLOR: 1,
    AMC_ATTRIBUTE_TEXCOORDS: 2,
    AMC_ATTRIBUTE_NORMALS: 3
};

var shaderProgramObject = null;

var vao = null;
var vboPosition = null;
var vboTexCoords = null;

var mvpMatrxiUniform;
var textureSamplerUniform;

var perspectiveProjectionMatrix = null;

var smiley_texture = null;

var keyPressUniform;

var gb1 = 1;

var requestAnimationFrame =
    window.requestAnimationFrame ||  //for chrome
    window.webkitRequestAnimationFrame ||  //for safari
    window.mozRequestAnimationFrame || //for mozilla
    window.oRequestAnimationFrame || //for opera
    window.msRequestAnimationFrame; //for msedge/ie



//our main function
function main() {
    //Get Canvas
    canvas = document.getElementById("ABC");

    if (canvas == null) {
        console.log("Getting canvas failed\n");
    }
    else
        console.log("Getting canvas Succedded\n");

    //Set Canvas Width and Height for future use
    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    //Register For Keyboard Events
    window.addEventListener("keydown", keyDown, false);

    //Register For Mouse Events
    window.addEventListener("click", mouseDown, false);

    window.addEventListener("resize", resize, false);

    initialise();

    resize();

    display();
}

function keyDown(event) {

    //code
    switch (event.keyCode) 
    {
        //Exit
        case 81: //Q
        case 113://q

            uninitialise();
            window.close();

            break;


        //FullScreen
        case 70://F
        case 102://f
            toggleFullScreen();

            break;

        case 49:
            gb1 = 1;
            break;

        case 50:
            gb1 = 2;
            break;

        case 51:
            gb1 = 3;
            break;

        case 52:
            gb1 = 4;
            break;

        default :
            gb1 = 0;
            break;
        
    }
}

function mouseDown() 
{

}

function toggleFullScreen() {

    var fullScreen_element =
        document.fullscreenElement ||
        document.webkitFullscreenElement ||
        document.mozFullScreenElement ||
        document.msFullscreenElement ||
        null;

    //if not Fullscreen
    if (fullScreen_element == null) {

        bFullScreen = true;
        if (canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if (canvas.webkitRequestFullscreen)
            canvas.webkitRequestFullscreen();
        else if (canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if (canvas.msRequestFullscreen)
            canvas.msRequestFullscreen();

    }
    else {

        bFullScreen = false;
        if (document.exitFullscreen)
            document.exitFullscreen();
        else if (document.webkitExitFullscreen)
            document.webkitExitFullscreen();
        else if (document.mozCancelFullScreen)
            document.mozCancelFullScreen();
        else if (document.msExitFullscreen)
            document.msExitFullscreen();
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
    //Get WebGL-Context from above canvas
    gl = canvas.getContext("webgl2");
    if (gl == null) {
        console.log("Getting webGL2 Context Failed\n");
    }
    else
        console.log("Getting webGL2 Context Successeded");

    //Vertex Shader
    var vertexShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "in vec4 aPosition;" +
        "in vec2 aTexCoords;" +
        "uniform mat4 uMVPMatrix;" +
        "out vec2 oTexCoords;" +
        "void main(void)" +
        "{" +
        "   gl_Position=aPosition;" +
        "   gl_Position = uMVPMatrix * aPosition;" +
        "   oTexCoords = aTexCoords;" +
        "}";

    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);

    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {

        var error = gl.getShaderInfoLog(vertexShaderObject);

        if (error.length > 0) {
            var log = "Vertex Shader Compilation Error : " + error;
            //MessageBox
            alert(log);    //console.log(log);

        }
        uninitialise();
    }
    else {
        console.log("Vertex Shader Compiled Successfully");
    }

    var fragmentShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "in vec2 oTexCoords;" +
        "uniform sampler2D uTextureObj;" +
        "uniform int uKeyPress;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
            "if(uKeyPress == 0)" +
            "{"+
            "FragColor = vec4(1.0f,1.0f,1.0f,1.0f);"+
            "}"+
            "else"+
            "{"+
            "FragColor=texture(uTextureObj, oTexCoords);"+
            "}"+
        "}";

    //Fragment Shader
    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);

    gl.compileShader(fragmentShaderObject);

    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {

        var error = gl.getShaderInfoLog(fragmentShaderObject);

        if (error.length > 0) {
            var log = "Fragment Shader Compilation Error : " + error;
            //MessageBox
            alert(log);    //console.log(log);

        }
        uninitialise();
    }
    else {
        console.log("Fragment Shader Compiled Successfully");
    }

    //Program object
    shaderProgramObject = gl.createProgram();

    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    gl.bindAttribLocation(shaderProgramObject, vertexAttributeEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
    gl.bindAttribLocation(shaderProgramObject, vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS, "aTexCoords");


    gl.linkProgram(shaderProgramObject);

    if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {

        var error = gl.getProgramInfoLog(shaderProgramObject);

        if (error.length > 0) {
            var log = "Shader Program Link Error : " + error;
            //MessageBox
            alert(log);    //console.log(log);

        }
        uninitialise();
    }
    else {
        console.log("Shader Program Link Successfully");
    }

    mvpMatrxiUniform = gl.getUniformLocation(shaderProgramObject, "uMVPMatrix");
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "uTextureObj");
	keyPressUniform = gl.getUniformLocation(shaderProgramObject, "uKeyPress");	

    //Square data
    var square_position = new Float32Array([
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0
    ]);

    var square_texcoords = new Float32Array(
        [1.0, 1.0,
            0.0, 1.0,
            0.0, 0.0,
            1.0, 0.0
        ]
    );

    //Create Vao
    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    //Create vboPosition
    vboPosition = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboPosition);
    gl.bufferData(gl.ARRAY_BUFFER, square_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_POSITION);

    //vbo texcoord
    vboTexCoords = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboTexCoords);
    gl.bufferData(gl.ARRAY_BUFFER, square_texcoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(vertexAttributeEnum.AMC_ATTRIBUTE_TEXCOORDS);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);


    //Load Texture
    smiley_texture = loadGLTexture("smiley.bmp");

    if (smiley_texture == null) {
        alert("Texture Loading Failed");
        uninitialise();
    } else {
        console.log("Texture Loading Successfull");
    }

    //Depth Initialisation
    gl.enable(gl.DEPTH_TEST);
    gl.clearDepth(1.0);
    gl.depthFunc(gl.LEQUAL);

    //Set WebGL co
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    //initialise perspective projection matrix
    perspectiveProjectionMatrix = mat4.create();

}

function resize() {

    //code
    if (bFullScreen == true) {

        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    //Set Viewport
    gl.viewport(0, 0, canvas.width, canvas.height);

    perspectiveProjectionMatrix = mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);

}

function display() 
{
    //code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    //Transformations
    var modelViewMatrix = mat4.create();
    var mvpMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);

    mat4.multiply(mvpMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpMatrxiUniform, false, mvpMatrix);

   var square_texcoord;

    
    if(gb1 == 1)
    {
        square_texcoord = new Float32Array([
            0.5, 0.5,
            0.0, 0.5,
            0.0, 0.0,
            0.5, 0.0
        ]);
        gl.uniform1i(keyPressUniform, 1);
    }
    else if(gb1 == 2)
    {
        square_texcoord = new Float32Array([
            1.0, 1.0,
            0.0, 1.0,
            0.0, 0.0,
            1.0, 0.0
        ]);
        gl.uniform1i(keyPressUniform, 1);
    }
    else if(gb1 ==3)
    {
        square_texcoord = new Float32Array([
            2.0, 2.0,
            0.0, 2.0,
            0.0, 0.0,
            2.0, 0.0
        ]);
        gl.uniform1i(keyPressUniform, 1);
    }
    else if(gb1 ==4)
    {
        square_texcoord = new Float32Array([
            0.5, 0.5,
            0.5, 0.5,
            0.5, 0.5,
            0.5, 0.5
        ]);
        gl.uniform1i(keyPressUniform, 1);
    }
    else
    {
        gl.uniform1i(keyPressUniform, 0);
    }

    gl.bindVertexArray(vao);
    //Texture Binding
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, smiley_texture);
    gl.uniform1i(textureSamplerUniform, 0);
    
	gl.bindBuffer(gl.ARRAY_BUFFER, vboTexCoords);
		
	gl.bufferData(gl.ARRAY_BUFFER, square_texcoord, gl.DYNAMIC_DRAW);

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    gl.bindVertexArray(null);

    gl.useProgram(null);

    update();

    //Double Buffering
    requestAnimationFrame(display, canvas);
}


function update() {

}

function uninitialise() {

    if (shaderProgramObject) { //shaderProgramObject != null

        gl.useProgram(shaderProgramObject);

        var shaderObjects = gl.getAttachedShaders(shaderProgramObject);

        if (shaderObjects && shaderObjects.length > 0) {

            for (let i = 0; i < shaderObjects.length; i++) {

                gl.detachShader(shaderProgramObject, shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;
            }

        }

        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }


    //Delete Texture
    if (smiley_texture != null) {

        gl.deleteTexture(smiley_texture);
        smiley_texture = null;

    }

    //Delete vbo and vao
    if (vboPosition != null) {

        gl.deleteBuffer(vboPosition);
        vboPosition = null;

    }

    if (vboTexCoords != null) {

        gl.deleteBuffer(vboTexCoords);
        vboTexCoords = null;

    }

    if (vao) {

        gl.deleteVertexArray(vao);
        vao = null;
    }
}
