package com.abc.window;

import android.content.Context; 

import androidx.appcompat.widget.AppCompatTextView;
import android.graphics.Color;
import android.view.Gravity;

// Event related packages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

public class MyTextView extends AppCompatTextView implements OnDoubleTapListener, OnGestureListener
{
    private GestureDetector gestureDetector;

    public MyTextView(Context context)
    {
        super(context);

        setTextColor(Color.rgb(0, 255, 0));
        setTextSize(48);
        setGravity(Gravity.CENTER);
        setText("Hello World !!!");
        //create and set gesture object
        gestureDetector = new GestureDetector(context, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    //implementation of onTouch event method of viewclass
    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        if(!gestureDetector.onTouchEvent(e))
        {
            super.onTouchEvent(e);
        }
        return true;
    }

    //implementation of 3 methods of on double tap listner interface

    @Override
    public boolean onDoubleTap(MotionEvent e)
    {
        setText("Double Tap");
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent e)
    {
        setText("Single Tap");
        return true;
    }

    //implementation of 6 methods of on gesture listner interface
    @Override
    public boolean onDown(MotionEvent e)
    {
        return true;
    }

    @Override
    public boolean onFling(MotionEvent eOne, MotionEvent eTwo, float velocityX, float velocityY)
    {
        return true;
    }

    @Override
    public void onLongPress(MotionEvent e)
    {
        setText("Long Press");
    }

    @Override
    public boolean onScroll(MotionEvent eOne, MotionEvent eTwo, float distanceX, float distanceY)
    {
        setText("Scrool,Swipe");
        return true;
    }

    @Override
    public void onShowPress(MotionEvent e)
    {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent e)
    {
        return true;
    }
}

