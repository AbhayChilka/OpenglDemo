var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width;
var canvas_original_height;

//Webgl related variables
const VertexAtrributEnum = {
	AMC_ATTRIBUTE_POSITION : 0,
	AMC_ATTRIBUTE_COLOR : 1,
	AMC_ATTRIBUTE_NORMAL : 2,
	AMC_ATTRIBUTE_TEXCOORD : 3
  };

var shaderProgramObject = null;

var mvpMatrixUniform;

var sphere = null;

//uniform
var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var uKeyPressUniform;

//light
var lightambientUniform = new Array(3);		
var lightDefuseUniform = new Array(3);		
var lightSpecularUniform = new Array(3);		
var lightPositionUniform = new Array(3);	

//material
var materialAmbienrUniform;	
var materialDefuseUniform;	
var materialSpecularUniform;	
var materialshininessUniform;

var Angle = 1.0;


var lightPosition = new Array(4);

var x, y, z;

var bLight;

var perspectiveProjectionMatrix;

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
			if (bLight == 0) {
				bLight = 1;
			}
			else {
				bLight = 0;
			}
			break;

		case 88:
			x = 1;
			y = 0;
			z = 0;
			break;

		case 89:
			x = 0;
			y = 1;
			z = 0;
			break;

		case 90:
			x = 0;
			y = 0;
			z = 1;
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

    var vertexShader = 
		"#version 300 es\n" +
		"precision highp int;" +
		"precision highp float;" +
		"in vec4 aPosition;" +
		"in vec3 aNormal;" +
		"uniform mat4 uModelViewMatrix;" +
		"uniform mat4 uViewMatrix;" +
		"uniform mat4 uProjectionMatrix;" +
		"uniform vec4 uLightPosition;" +
		"\n" +
		"uniform int uKeyPress;" +
		"out vec3 oTransformedNormals;" +
		"out vec3 oLightDirection;" +
		"out vec3 oViewerVector;" +
		"void main(void)" +
		"{" +
		"if(uKeyPress == 1)" +
		"{" +
		"vec4 iCoordinates = uViewMatrix * uModelViewMatrix * aPosition;" +
		"oTransformedNormals = mat3(uViewMatrix * uModelViewMatrix) * aNormal;" +
		"oLightDirection = vec3(uLightPosition - iCoordinates);" +
		"oViewerVector = -iCoordinates.xyz;" +
		"}" +
		"else" +
		"{" +
		"oTransformedNormals = vec3(0.0, 0.0, 0.0);" +
		"oLightDirection = vec3(0.0, 0.0, 0.0);" +
		"oViewerVector = vec3(0.0, 0.0, 0.0);" +
		"}" +
		"gl_Position = uProjectionMatrix * uViewMatrix * uModelViewMatrix * aPosition;" +
		"}";

    
    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShader);
    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0) {
            var log = "Vertex shader compilation error: " + error;
            alert(log);
            uninitialise();
        }
    }
    else {
        console.log("Vertex shader compile done!\n");
    }

    var fragmentShader =
        "#version 300 es" +
        "\n" +
        "precision highp int;" +
        "precision highp float;" +
        "in vec3 oTransformedNormals;" +
        "in vec3 oLightDirection;" +
        "in vec3 oViewerVector; " +
        "uniform vec3 uLightAmbient;" +
        "uniform vec3 uLightDefuse;" +
        "uniform vec3 uLightSpecular;" +
        "\n" +
        "uniform vec3 uMaterialAmbient;" +
        "uniform vec3 uMaterialDefuse;" +
        "uniform vec3 uMaterialSpecular;" +
        "uniform float uMaterialShininess;" +
        "uniform int uKeyPress;" +
        "out vec4 FragColor;" +
        "void main(void)" +
        "{" +
        "vec3 phongADSLight;" +
        "if(uKeyPress == 1)" +
        "{" +
        "	vec3 normalizedTransformedNormal = normalize(oTransformedNormals);" +
        "	vec3 normalizedLightDirection = normalize(oLightDirection);" +
        "	vec3 normalizedViewerVector = normalize(oViewerVector);" +
        "	vec3 ambientLight = uLightAmbient * uMaterialAmbient;" +
        "	vec3 diffuseLight = uLightDefuse * uMaterialDefuse * max(dot(normalizedLightDirection, normalizedTransformedNormal), 0.0);" +
        "	vec3 reflectionVector = reflect(-normalizedLightDirection, normalizedTransformedNormal);" +
        "	vec3 lightSpecular = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector, normalizedViewerVector), 0.0), uMaterialShininess);" +
        "	phongADSLight = ambientLight + diffuseLight + lightSpecular;" +
        "}" +
        "else" +
        "{" +
        "	phongADSLight = vec3(1.0, 1.0, 1.0);" +
        "}" +
        "	FragColor = vec4(phongADSLight, 1.0);" +
        "}";
    
    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShader);
    gl.compileShader(fragmentShaderObject);

    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0) {
            var log = "Fragment shader compilation error: " + error;
            alert(log);
            uninitialise();
        }
    }
    else {
        console.log("Fragment shader compile done!\n");
    }

    //shader program object
    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);
    gl.bindAttribLocation(shaderProgramObject, VertexAtrributEnum.AMC_ATTRIBUTE_POSITION, "aPosition");
    gl.bindAttribLocation(shaderProgramObject, VertexAtrributEnum.AMC_ATTRIBUTE_NORMAL, "aNormal");
    gl.linkProgram(shaderProgramObject);

    if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            var log = "Shader program linking error: " + error;
            alert(log);
            uninitialise();
        }
    }
    else {
        console.log("Shader program linking done!\n");
    }

    //mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject,"uMvpMatrix");
    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelViewMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");
    uKeyPressUniform = gl.getUniformLocation(shaderProgramObject, "uKeyPress"); 

    //lights
    lightambientUniform = gl.getUniformLocation(shaderProgramObject, "uLightAmbient");		
    lightDefuseUniform = gl.getUniformLocation(shaderProgramObject, "uLightDefuse");		
    lightSpecularUniform = gl.getUniformLocation(shaderProgramObject, "uLightSpecular");		
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "uLightPosition");	

    //material
    materialAmbienrUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialAmbient");	
    materialDefuseUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialDefuse");	
    materialSpecularUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialSpecular");	
    materialshininessUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialShininess");

	
    //sphere
    sphere = new Mesh();
    makeSphere(sphere, 2.0, 50, 50);
    
    //depth
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);


    //set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    perspectiveProjectionMatrix = mat4.create();

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
    gl.viewport(0, 0, canvas.width, canvas.height);

    //set perspective position
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);

}

function display()
{
    //code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    var factor = 200.0;

	gl.useProgram(shaderProgramObject);

    
	if (bLight == 1) {
		gl.uniform1i(uKeyPressUniform, 1);

		var lightAmbient = [0.0, 0.0, 0.0];
		gl.uniform3fv(lightambientUniform, lightAmbient);

		var lightDiffuse = [1.0, 1.0, 1.0];
		gl.uniform3fv(lightDefuseUniform, lightDiffuse);

		var lightSpecular = [1.0, 1.0, 1.0];
		gl.uniform3fv(lightSpecularUniform, lightSpecular);


		if (x == 1) {
			lightPosition[0] = 0.0;
			lightPosition[1] = factor * Math.sin(Angle);
			lightPosition[2] = factor * Math.cos(Angle);
			lightPosition[3] = 0.0;
		}
		else if (y == 1) {
			lightPosition[0] = factor * Math.sin(Angle);
			lightPosition[1] = 0.0;
			lightPosition[2] = factor * Math.cos(Angle);
			lightPosition[3] = 0.0;
		}
		else if (z == 1) {
			lightPosition[0] = factor * Math.sin(Angle);
			lightPosition[1] = factor * Math.cos(Angle);
			lightPosition[2] = 0.0;
			lightPosition[3] = 0.0;
		}

		gl.uniform4fv(lightPositionUniform, lightPosition);

	}
	else {
		gl.uniform1i(uKeyPressUniform, 0);
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//					1st Col
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//1st Sphere
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-12.00, 15.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//emerald material
	var materialAmbient = [1.0215, 0.1745, 0.0215];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	var materialDiffuse = [1.07568, 0.61424, 0.07568];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	var materialSpecular = [0.633, 0.727811, 0.633];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	var materialShininess = 0.6 * 128.0;
	gl.uniform1f(materialshininessUniform, materialShininess);


	//Sphere
	sphere.draw();


	//2nd Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-12.00, 10.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//jade material
	materialAmbient = [0.135, 0.2225, 0.1575];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.54, 0.89, 0.63];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.316228, 0.316228, 0.316228];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.1 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//3rd Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-12.00, 5.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//jade material
	materialAmbient = [0.05375, 0.05, 0.06625];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.18275, 0.17, 0.22525];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.332741, 0.328634, 0.346435];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.3 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//4th Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-12.00, 0.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//Peral material
	materialAmbient = [0.25, 0.20725, 0.20725];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [1.0, 0.829, 0.829];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.296648, 0.296648, 0.296648];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.088 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//5th Sphere
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-12.00, -5.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//ruby
	var materialAmbient = [0.1745, 0.01175, 0.01175];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	var materialDiffuse = [0.61424, 0.04136, 0.04136];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	var materialSpecular = [0.727811, 0.626959, 0.626959];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	var materialShininess = 0.6 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);


	//Sphere
	sphere.draw();


	//6th Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-12.00, -10.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	////turquoise
	materialAmbient = [0.1, 0.18725, 0.1745];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.396, 0.74151, 0.69102];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.297254, 0.30829, 0.306678];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.1 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();



	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//					2nd Col
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//1st Sphere
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-5.00, 15.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//brass 
	materialAmbient = [0.329412, 0.223529, 0.027451];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.780392, 0.568627, 0.113725];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.992157, 0.941176, 0.807843];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.21794872 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);


	//Sphere
	sphere.draw();


	//2nd Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-5.00, 10.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//bronze
	materialAmbient = [0.2125, 0.1275, 0.054];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.714, 0.4284, 0.18144];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.393548, 0.271906, 0.166721];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.2 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//3rd Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-5.00, 5.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	////chrome material
	materialAmbient = [0.25, 0.25, 0.25];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.4, 0.4, 0.4];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.774597, 0.774597, 0.774597];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.6 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//4th Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-5.00, 0.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//copper material
	materialAmbient = [0.19125, 0.0735, 0.0225];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.7038, 0.27048, 0.0828];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.256777, 0.137622, 0.086014];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.1 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//5th Sphere
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-5.00, -5.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//gold
	var materialAmbient = [0.24725, 0.1995, 0.0745];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	var materialDiffuse = [0.75164, 0.60648, 0.22648];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	var materialSpecular = [0.628281, 0.555802, 0.366065];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	var materialShininess = 0.4 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);


	//Sphere
	sphere.draw();


	//6th Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [-5.00, -10.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//silver
	materialAmbient = [0.19225, 0.19225, 0.19225];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.50754, 0.50754, 0.50754];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.508273, 0.508273, 0.508273];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.4 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//					3rd Col
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//1st Sphere
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [3.00, 15.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//emerald material
	var materialAmbient = [0.0, 0.0, 0.0];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	var materialDiffuse = [0.01, 0.01, 0.01];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	var materialSpecular = [0.50, 0.50, 0.50];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	var materialShininess = 0.25 * 128.0;
	gl.uniform1f(materialshininessUniform, materialShininess);


	//Sphere
	sphere.draw();


	//2nd Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [3.00, 10.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	////cyan material
	materialAmbient = [0.0, 0.1, 0.06];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.0, 0.50980392, 0.50980392];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.50196078, 0.50196078, 0.50196078];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.25 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//3rd Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [3.00, 5.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//jade material
	materialAmbient = [0.0, 0.0, 0.0];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.1, 0.35, 0.1];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.45, 0.45, 0.45];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.25 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//4th Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [3.00, 0.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//Peral material
	materialAmbient = [0.0, 0.0, 0.0];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.5, 0.0, 0.0];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.7, 0.6, 0.6];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.25 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//5th Sphere
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [3, -5.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//ruby
	var materialAmbient = [0.0, 0.0, 0.0];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	var materialDiffuse = [0.55, 0.55, 0.55];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	var materialSpecular = [0.70, 0.70, 0.70];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	var materialShininess = 0.25 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);


	//Sphere
	sphere.draw();


	//6th Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [3, -10.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	////turquoise
	materialAmbient = [0.0, 0.0, 0.0];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.5, 0.5, 0.0];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.6, 0.6, 0.5];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.4 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();



	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//					4th Col
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//1st Sphere
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [10.00, 15.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//brass 
	materialAmbient = [0.02, 0.02, 0.02];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.1, 0.1, 0.1];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.4, 0.4, 0.4];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.078125 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);


	//Sphere
	sphere.draw();


	//2nd Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [10.00, 10.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//bronze
	materialAmbient = [0.0, 0.05, 0.05];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.04, 0.5, 0.5];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.04, 0.7, 0.7];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.078125 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//3rd Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [10.00, 5.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	////chrome material
	materialAmbient = [0.0, 0.05, 0.0];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.4, 0.5, 0.4];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.04, 0.7, 0.04];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.078125 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//4th Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [10.00, 0.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//copper material
	materialAmbient = [0.05, 0.0, 0.0];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.5, 0.4, 0.4];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.7, 0.04, 0.04];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.078125 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();

	//5th Sphere
	var modelMatrix = mat4.create();
	var viewMatrix = mat4.create();
	var projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [10.00, -5.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//gold
	var materialAmbient = [0.05, 0.05, 0.05];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	var materialDiffuse = [0.5, 0.5, 0.5];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	var materialSpecular = [0.70, 0.70, 0.70];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	var materialShininess = 0.4 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);


	//Sphere
	sphere.draw();


	//6th Sphere
	modelMatrix = mat4.create();
	viewMatrix = mat4.create();
	projectionMatrix = mat4.create();

	mat4.translate(modelMatrix, modelMatrix, [10.00, -10.0, -32.0]);

	//mat4.scale(modelMatrix,modelMatrix,[0.75,0.75,0.75]);		
	projectionMatrix = perspectiveProjectionMatrix;

	gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);

	//silver
	materialAmbient = [0.05, 0.05, 0.0];
	gl.uniform3fv(materialAmbienrUniform, materialAmbient);

	materialDiffuse = [0.5, 0.5, 0.4];
	gl.uniform3fv(materialDefuseUniform, materialDiffuse);

	materialSpecular = [0.7, 0.7, 0.04];
	gl.uniform3fv(materialSpecularUniform, materialSpecular);

	materialShininess = 0.078125 * 128;
	gl.uniform1f(materialshininessUniform, materialShininess);

	//Sphere
	sphere.draw();



    gl.useProgram(null);
	update();

    //set double buffering
    requestAnimationFrame(display, canvas);
}

function update()
{
    Angle = Angle + 0.05;
}

function uninitialise() {
    //code
    if (shaderProgramObject) {
        // !shaderProgramObject
        gl.useProgram(shaderProgramObject);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject);
        if (shaderObjects || shaderObjects.length > 0) {
            for (let i = 0; i < shaderObjects.length; ++i) {
                gl.detachShader(shaderProgramObject, shaderObjects[i]);
                gl.deleteShader(shaderObjects[i]);
                shaderObjects[i] = null;
            }
        }

        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }

    if (sphere) {
        sphere.deallocate();
        sphere = null;
    }
}
