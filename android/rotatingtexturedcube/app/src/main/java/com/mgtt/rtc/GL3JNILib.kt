package com.mgtt.rtc

object GL3JNILib {
    external fun Init()
    external fun Render()
    external fun UpdateGlViewPort(width: Int, height: Int)

    init {
        System.loadLibrary("rotating_textured_cube")
    }
}