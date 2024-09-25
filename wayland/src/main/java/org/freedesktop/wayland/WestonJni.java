package org.freedesktop.wayland;

import static org.freedesktop.wayland.WlTouch.*;

import android.graphics.Rect;
import android.view.MotionEvent;
import android.view.Surface;

import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class WestonJni {
    static {
        System.loadLibrary("weston-jni");
    }

    public static final long NullPtr = 0;
    public static final int RendererPixman = 0;
    public static final int RendererGL = 1;
    private long nativePtr = NullPtr;
    private Future<?> displayFuture;
    private final Config mConfig = new Config();

    public long getNativePtr() {
        return nativePtr;
    }

    public void nativeCreate() {
        if (nativePtr != NullPtr)
            throw new PtrException(String.format("NativePtr: %d seems not released yet.", nativePtr));

        nativePtr = create();
        if (nativePtr == NullPtr)
            throw new PtrException("nativeCreate got an unexpected null ptr.");
    }

    public void nativeDestroy() {
        if (nativePtr == NullPtr)
            throw new PtrException("NativePtr is already null.");

        destroy(nativePtr);
        nativePtr = NullPtr;
    }

    public void init() {
        if (nativePtr == NullPtr)
            throw new PtrException("NativePtr is null.");
        init(nativePtr);
    }

    public void setSurface(Surface surface) {
        if (nativePtr == NullPtr)
            throw new PtrException("NativePtr is null.");
        setSurface(nativePtr, surface);
    }

    public void startDisplay() {
        if (isDisplayRunning(nativePtr))
            throw new DisplayException("Display is already running.");
        displayFuture = Executors.newSingleThreadExecutor().submit(() -> displayRun(nativePtr));
    }

    public void stopDisplay() {
        if (!isDisplayRunning(nativePtr))
            throw new DisplayException("Display is not running.");
        displayTerminate(nativePtr);
        try {
            displayFuture.get();
        } catch (Exception e) {
            e.printStackTrace();
        }
        displayFuture = null;
    }

    public void updateConfig() {
        if (nativePtr == NullPtr)
            throw new PtrException("NativePtr is null.");
        updateConfig(nativePtr, mConfig);
    }

    public Config getConfig() {
        return mConfig;
    }

    public void onTouch(MotionEvent event) {
        if (nativePtr == NullPtr)
            return;

        int touchType = switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN->WL_TOUCH_DOWN;
            case MotionEvent.ACTION_MOVE->WL_TOUCH_MOTION;
            case MotionEvent.ACTION_UP->WL_TOUCH_UP;
            case MotionEvent.ACTION_CANCEL->WL_TOUCH_CANCEL;
            default -> -1;
        };

        if (touchType == -1)
            return;

        performTouch(nativePtr, event.getPointerId(event.getActionIndex()), touchType, event.getX(), event.getY());
    }

    @Override
    protected void finalize() throws Throwable {
        if (nativePtr != NullPtr) {
            if (isDisplayRunning(nativePtr))
                stopDisplay();
            nativeDestroy();
        }
        super.finalize();
    }

    public static class Config {
        public int rendererType = RendererPixman;
        public int renderRefreshRate = 60;
        public Rect screenRect;
        public Rect displayRect;
        public Rect renderRect;
        public String socketPath = "";
    }

    private native long create();
    private native void destroy(long ptr);
    private native boolean init(long ptr);
    private native boolean setSurface(long ptr, Surface surface);
    private native boolean updateConfig(long ptr, Config config);
    private native void displayRun(long ptr);
    private native void displayTerminate(long ptr);
    private native boolean isDisplayRunning(long ptr);
    private native void performTouch(long ptr, int touchId, int touchType, float x, float y);
}
