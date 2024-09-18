#include <jni.h>
#include <string.h>
#include "weston-jni.h"

extern struct WestonJni* westonJniPtr;

static void handle_repaint_output_pixman(pixman_image_t*);
static void handle_output_set_size(int, int);

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

    return (jlong) westonJni;
}

JNIEXPORT void JNICALL
Java_org_freedesktop_wayland_WestonJni_destroy(JNIEnv* env, jobject thiz, jlong ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (westonJni) {
        free(westonJni->buffer);
        free(westonJni);
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