#ifndef WESTON_JNI_H
#define WESTON_JNI_H

#include <jni.h>
#include <pixman.h>
#include <libweston/backend-android.h>
#include <android/native_window_jni.h>

struct WestonJni {
    jobject javaObject;
    ANativeWindow* window;
    ANativeWindow_Buffer* buffer;

    struct wl_display* display;
    struct weston_log_context* logCtx;
    struct weston_compositor* compositor;
    struct weston_android_backend_config* backendConfig;
    struct weston_backend* backend;
    bool display_running;

    void (*output_create)();
    void (*output_destroy)();
    void (*output_set_size)(int, int);
    void (*repaint_output_pixman)(pixman_image_t*);
    void (*destroy)();
};

struct WestonJni* westonJniPtr = NULL;

#endif //WESTON_JNI_H