package org.freedesktop.wayland;

import android.view.Surface;

public class WestonJni {
    static {
        System.loadLibrary("weston-jni");
    }

    public static final long NullPtr = 0;

    private long nativePtr = NullPtr;

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

        if (haveSurface(nativePtr))
            throw new PtrException("Have a surface already.");

        if (!renderSurface(nativePtr, surface))
            throw new RenderSurfaceException();
    }

    @Override
    protected void finalize() throws Throwable {
        if (nativePtr != NullPtr)
            destroy(nativePtr);
        super.finalize();
    }

    private native long create();
    private native void destroy(long ptr);
    private native boolean renderSurface(long ptr, Surface surface);
    private native boolean haveSurface(long ptr);
}
