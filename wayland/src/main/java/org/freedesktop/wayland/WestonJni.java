package org.freedesktop.wayland;

import android.view.Surface;

import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class WestonJni {
    static {
        System.loadLibrary("weston-jni");
    }

    public static final long NullPtr = 0;
    private long nativePtr = NullPtr;
    private boolean needInit = true;
    private Future<?> displayFuture;

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

    public void setRenderSurface(Surface surface) {
        if (nativePtr == NullPtr)
            throw new PtrException("NativePtr is null.");

        if (surface != null && haveSurface(nativePtr))
            throw new PtrException("Have a surface already.");

        if (!renderSurface(nativePtr, surface))
            throw new RenderSurfaceException();
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

    public boolean initWeston() {
        boolean ret;

        if (nativePtr == NullPtr)
            throw new PtrException("NativePtr is null.");

        if (!needInit)
            return false;

        ret = init(nativePtr);
        needInit = !ret;
        return ret;
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

    private native long create();
    private native void destroy(long ptr);
    private native boolean renderSurface(long ptr, Surface surface);
    private native boolean haveSurface(long ptr);
    private native boolean init(long ptr);
    private native void displayRun(long ptr);
    private native void displayTerminate(long ptr);
    private native boolean isDisplayRunning(long ptr);
}
