package com.abc.window;

import androidx.appcompat.app.AppCompatActivity;

import androidx.appcompat.widget.AppCompatTextView;

import android.os.Bundle;

import android.graphics.Color;

import android.view.Gravity;

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
        getWindow().getDecorView().setBackgroundColor(Color.BLACK);
        AppCompatTextView myTextView = new AppCompatTextView(this);        
        myTextView.setTextColor(Color.rgb(0, 255, 0));
        myTextView.setTextSize(60);
        myTextView.setGravity(Gravity.CENTER);
        myTextView.setText("Hello World !!!");

        setContentView(myTextView);
    }
}