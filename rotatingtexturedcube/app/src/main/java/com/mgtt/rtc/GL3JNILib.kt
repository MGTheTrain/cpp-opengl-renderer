package com.mgtt.rtc

class GL3JNILib {
    external fun Render()
    external fun UpdateGlViewPort()

    init {
        System.loadLibrary("rtc")
    }
}