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

    public void performTouch(int touchId, int touchType, float x, float y) {
        if (nativePtr == NullPtr)
            return;

        performTouch(nativePtr, touchId, touchType, x, y);
    }

    public void performKey(int key, int keyState) {
        if (nativePtr == NullPtr)
            return;

        performKey(nativePtr, key, keyState);
    }

    public void performPointer(int pointerType, float x, float y) {
        if (nativePtr == NullPtr)
            return;

        performPointer(nativePtr, pointerType, x, y);
    }

    public void performButton(int button, int buttonState) {
        if (nativePtr == NullPtr)
            return;

        performButton(nativePtr, button, buttonState);
    }

    public void performAxis(int axisType, float value, boolean hasDiscrete, int discrete) {
        if (nativePtr == NullPtr)
            return;

        performAxis(nativePtr, axisType, value, hasDiscrete, discrete);
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
        public String socketPath;
        public String xdgConfigPath;
        public String xdgRuntimePath;
        public String xkbRule = "evdev";
        public String xkbModel = "pc105";
        public String xkbLayout = "us";
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
    private native void performKey(long ptr, int key, int keyState);
    private native void performPointer(long ptr, int pointerType, float x, float y);
    private native void performButton(long ptr, int button, int buttonState);
    private native void performAxis(long ptr, int axisType, float value, boolean hasDiscrete, int discrete);
}
