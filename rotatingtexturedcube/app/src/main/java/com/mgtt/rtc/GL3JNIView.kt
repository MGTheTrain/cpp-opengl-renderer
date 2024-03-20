package com.mgtt.rtc

import android.content.Context
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GL3JNIView(context: Context?): GLSurfaceView(context) {
    private fun init() {
        setEGLContextClientVersion(3)
        setRenderer(Renderer())
    }

    private class Renderer : GLSurfaceView.Renderer {
        override fun onDrawFrame(gl: GL10) {
            GL3JNILib.Render()
        }

        override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
            GL3JNILib.UpdateGlViewPort(width, height)
        }

        override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
            // GL3JNILib.Init()
        }
    }

    init {
        init()
    }
}