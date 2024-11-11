package com.abc.window;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import android.graphics.Color;

//Packages for FullScreen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;

import android.content.pm.ActivityInfo;

public class MainActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        //FullScreen
        //get and hide action bar
        getSupportActionBar().hide();

        //do fullscreen
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);

        //get window insets controller object
        WindowInsetsControllerCompat windowInsetsControllerCompat = WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());

        //tell above objects to hide system bars means status bar, naviation bar,caption bar and IME(input method editor)
        windowInsetsControllerCompat.hide(WindowInsetsCompat.Type.systemBars() | WindowInsetsCompat.Type.ime());
        
        //do landscape
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
     
        //setting background color black
        MyTextView myTextView = new MyTextView(this);        
        getWindow().getDecorView().setBackgroundColor(Color.BLACK);

        setContentView(myTextView);
    }
}