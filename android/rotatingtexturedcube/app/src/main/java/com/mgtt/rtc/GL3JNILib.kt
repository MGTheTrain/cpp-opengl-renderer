package com.mgtt.rtc

object GL3JNILib {
    external fun Render()
    external fun UpdateGlViewPort(width: Int, height: Int)

    init {
        System.loadLibrary("rtc")
    }
}