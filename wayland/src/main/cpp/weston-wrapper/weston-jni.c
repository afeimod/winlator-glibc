#include <jni.h>
#include <string.h>
#include <android/log.h>
#include "weston-jni.h"

#define ANDROID_LOG(msg) __android_log_print(ANDROID_LOG_ERROR, "weston-jni", msg)

extern struct WestonJni* westonJniPtr;

static void handle_repaint_output_pixman(pixman_image_t*);
static void handle_output_set_size(int, int);
static int handle_log(const char *fmt, va_list ap);

static inline void throwJavaException(JNIEnv* env, const char* msg) {
    jclass runtimeExceptionClass = (*env)->FindClass(env, "java/lang/RuntimeException");
    if (!runtimeExceptionClass) {
        return;
    }
    (*env)->ThrowNew(env, runtimeExceptionClass, msg);
}

static inline struct WestonJni* getWestonJniFromPtr(JNIEnv* env, jlong ptr) {
    if ((jlong) westonJniPtr != ptr)
        throwJavaException(env, "Another westonJniPtr is handled or not called create().");
    return (struct WestonJni*) ptr;
}

JNIEXPORT jlong JNICALL
Java_org_freedesktop_wayland_WestonJni_create(JNIEnv* env, jobject thiz) {
    struct WestonJni* westonJni;

    if (westonJniPtr)
        throwJavaException(env, "Only one westonJni should be created.");

    if (!(westonJni = calloc(1, sizeof(struct WestonJni)))) {
        return 0;
    }

    westonJni->output_create = NULL;
    westonJni->output_destroy = NULL;
    westonJni->output_set_size = handle_output_set_size;
    westonJni->repaint_output_pixman = handle_repaint_output_pixman;
    westonJni->destroy = NULL;

    westonJniPtr = westonJni;

    return (jlong) westonJni;
}

JNIEXPORT void JNICALL
Java_org_freedesktop_wayland_WestonJni_destroy(JNIEnv* env, jobject thiz, jlong ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (westonJni) {
        if (westonJni->display && westonJni->display_running) {
            wl_display_terminate(westonJni->display);
            westonJni->display_running = false;
        }

        if (westonJni->compositor)
            weston_compositor_destroy(westonJni->compositor);

        if (westonJni->logCtx)
            weston_log_ctx_destroy(westonJni->logCtx);

        if (westonJni->display)
            wl_display_destroy(westonJni->display);

        free(westonJni->backendConfig);
        free(westonJni->buffer);
        free(westonJni);

        westonJniPtr = NULL;
    }
}

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_renderSurface(JNIEnv* env, jobject thiz, jlong ptr,
                                                     jobject surface) {
    ANativeWindow* window = NULL;
    ANativeWindow_Buffer* buffer = NULL;
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni)
        return JNI_FALSE;

    if (surface == NULL) {
        westonJni->window = NULL;
        return JNI_TRUE;
    }

    if (!(window = ANativeWindow_fromSurface(env, surface)))
        return JNI_FALSE;

    if (!(buffer = calloc(1, sizeof(ANativeWindow_Buffer))))
        return JNI_FALSE;

    westonJni->window = window;
    westonJni->buffer = buffer;

    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_haveSurface(JNIEnv* env, jobject thiz, jlong ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni)
        return JNI_FALSE;

    return westonJni->window ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_init(JNIEnv *env, jobject thiz, jlong ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);
    struct wl_display* display = NULL;
    struct weston_log_context* logCtx = NULL;
    struct weston_compositor* compositor = NULL;
    struct weston_android_backend_config* config = NULL;
    struct weston_backend* backend = NULL;

    if (!westonJni || westonJni->compositor)
        return JNI_FALSE;

    // create display
    if (!(display = wl_display_create())) {
        ANDROID_LOG("Failed to create display.");
        goto error_free;
    }

    // create socket
    if (wl_display_add_socket(display, "/data/data/com.winlator/files/tmp/wayland-0")) {
        ANDROID_LOG("Failed to add a socket.");
        goto error_free;
    }

    // create logger
    if (!(logCtx = weston_log_ctx_create())) {
        ANDROID_LOG("Failed to create weston logger");
        goto error_free;
    }

    weston_log_set_handler(handle_log, NULL);

    // create compositor
    if(!(compositor = weston_compositor_create(display, logCtx, NULL, NULL))) {
        ANDROID_LOG("Failed to create compositor.");
        goto error_free;
    }

    // create config
    if (!(config = calloc(1, sizeof(struct weston_android_backend_config)))) {
        ANDROID_LOG("Failed to allocate memory for backend config.");
        goto error_free;
    }

    config->base.struct_version = WESTON_ANDROID_BACKEND_CONFIG_VERSION;
    config->base.struct_size = sizeof(struct weston_android_backend_config);
    config->refresh = 60;
    config->renderer = WESTON_RENDERER_PIXMAN;

    // create backend
    if (!(backend = weston_compositor_load_backend(compositor, WESTON_BACKEND_ANDROID,
                                                   (struct weston_backend_config *)config))) {
        ANDROID_LOG("Failed to create android backend.");
        goto error_free;
    }

    westonJni->javaObject = thiz;
    westonJni->display = display;
    westonJni->logCtx = logCtx;
    westonJni->compositor = compositor;
    westonJni->backendConfig = config;
    westonJni->backend = backend;

    return JNI_TRUE;

error_free:
    if (display)
        wl_display_destroy(display);
    if (compositor)
        weston_compositor_destroy(compositor);
    if (logCtx)
        weston_log_ctx_destroy(logCtx);
    free(config);
    return JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_org_freedesktop_wayland_WestonJni_displayRun(JNIEnv *env, jobject thiz, jlong ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->display || westonJni->display_running)
        return;

    westonJni->display_running = true;
    wl_display_run(westonJni->display);
}

JNIEXPORT void JNICALL
Java_org_freedesktop_wayland_WestonJni_displayTerminate(JNIEnv *env, jobject thiz, jlong ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->display || !westonJni->display_running)
        return;

    wl_display_terminate(westonJni->display);
    westonJni->display_running = false;
}

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_isDisplayRunning(JNIEnv *env, jobject thiz, jlong ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->display)
        return JNI_FALSE;

    return westonJni->display_running;
}

static void handle_repaint_output_pixman(pixman_image_t* pixmanImage) {
    ANativeWindow* window = NULL;
    ANativeWindow_Buffer* buffer = NULL;

    if (!westonJniPtr || !westonJniPtr->window || !westonJniPtr->buffer || !pixmanImage)
        return;

    window = westonJniPtr->window;
    buffer = westonJniPtr->buffer;

    if (ANativeWindow_lock(window, westonJniPtr->buffer, NULL) == 0) {
        uint32_t* dst = (uint32_t*)buffer->bits;
        uint32_t* src = (uint32_t*)pixman_image_get_data(pixmanImage);

        int width = pixman_image_get_width(pixmanImage);
        int height = pixman_image_get_height(pixmanImage);

        for (int y = 0; y < height; ++y) {
            memcpy(dst + y * buffer->stride / sizeof(uint32_t), src + y * width,
                   width * sizeof(uint32_t));
        }

        ANativeWindow_unlockAndPost(window);
    }
}

static void handle_output_set_size(int width, int height) {
    if (!westonJniPtr || !westonJniPtr->window)
        return;

    ANativeWindow_setBuffersGeometry(westonJniPtr->window, width, height, WINDOW_FORMAT_RGBX_8888);
}

static int handle_log(const char *fmt, va_list ap) {
    static char logBuffer[256];
    vsnprintf(logBuffer, sizeof(logBuffer), fmt, ap);
    logBuffer[255] = '\0';
    __android_log_print(ANDROID_LOG_ERROR, "weston", "%s", logBuffer);
}