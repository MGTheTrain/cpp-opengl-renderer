package com.mgtt.rtc

import android.app.Activity
import android.graphics.Color
import android.os.Bundle
import android.util.DisplayMetrics
import android.widget.RelativeLayout

class MainActivity : Activity() {
    var gl3JniView: GL3JNIView? = null
    override fun onCreate(icicle: Bundle?) {
        super.onCreate(icicle)

        val dm = DisplayMetrics()
        this.window.windowManager.defaultDisplay.getMetrics(dm)
        val width = dm.widthPixels
        val height = dm.heightPixels
        val layout = RelativeLayout(this)
        layout.layoutParams = RelativeLayout.LayoutParams(width, height)
        setContentView(layout)
        gl3JniView = GL3JNIView(application)
        gl3JniView!!.layoutParams = RelativeLayout.LayoutParams(width, height)
        layout.addView(gl3JniView)
    }

    override fun onPause() {
        super.onPause()
        gl3JniView!!.onPause()
    }

    override fun onResume() {
        super.onResume()
        gl3JniView!!.onResume()
    }

    companion object {
        private val TRANSPARENT = Color.argb(0, 1, 1, 1)
    }
}