//our main function
function main()
{
    //get canvas
    var canvas = document.getElementById("ABC");
    if(canvas == null)
    {
        console.log("getting canvas failed");
    }
    else
    {
        console.log("getting canvas successeded");
    }

    //get context from above canvas
    var context = canvas.getContext("2d");
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

    //declare the text
    var text = "Hello World !!!";

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