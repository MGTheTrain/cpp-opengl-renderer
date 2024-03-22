package com.mgtt.rtc

import android.content.Context
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View

class TouchView: View {
    var tag = 0
    private var mX = 0f
    private var mY = 0f

    constructor(context: Context?) : super(context) {}
    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {}

    override fun onSizeChanged(w: Int, h: Int, width: Int, height: Int) {
        super.onSizeChanged(w, h, width, height)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        return true
    }
}