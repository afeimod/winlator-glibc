#ifndef WESTON_JNI_H
#define WESTON_JNI_H

#include <jni.h>
#include <pixman.h>
#include <libweston/backend-android.h>
#include <android/native_window_jni.h>

struct WestonJni {
    ANativeWindow* window;
    ANativeWindow_Buffer* buffer;
    struct weston_compositor* westonCompositorPtr;
    struct wl_display* wlDisplayPtr;
    struct weston_android_backend_config* backendConfigPtr;

    void (*output_create)();
    void (*output_destroy)();
    void (*output_set_size)(int, int);
    void (*repaint_output_pixman)(pixman_image_t*);
    void (*destroy)();
};

struct WestonJni* westonJniPtr = NULL;

#endif //WESTON_JNI_H