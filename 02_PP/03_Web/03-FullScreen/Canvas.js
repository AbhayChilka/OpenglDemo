
var canvas = null;
var context = null;

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

    //get context from above canvas
    context = canvas.getContext("2d");
    if(context == null)
    {
        console.log("getting 2d context failed");
    }
    else
    {
        console.log("getting 2d context successeded");
    }

    //set background canvas color to black
    context.fillStyle = "black";

    //paint the canvas rectangle with above color
    context.fillRect(0, 0, canvas.width, canvas.height);

    //draw text
    drawText("Hello World !!!");
    
    //register for keyboard events
    window.addEventListener("keydown", keyDown, false);

    //register for mouse events
    window.addEventListener("click", mouseDown, false);
}

function keyDown(event)
{
    switch(event.keyCode)
    {
        case 70:
        case 102:
            toggleFullscreen();
            drawText("Hello World !!!");
            break;

        default:
            break;
    }

}

function mouseDown()
{
}

function drawText(text)
{
    //code
    //set the text in middle of canvas 
    context.textAlign = "center"; //horizontal center
    context.textBaseline = "middle"; //vertical 

    //set font
    context.font = "48px sans-serif";

    //set text color to white
    context.fillStyle = "lime";

    //paint text with above color
    context.fillText(text, canvas.width/2, canvas.height/2);
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
    }
}


