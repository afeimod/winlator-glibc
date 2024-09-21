#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <libweston/libweston.h>
#include <libweston/windowed-output-api.h>
#include "weston-jni.h"

#define ANDROID_LOG(msg...) __android_log_print(ANDROID_LOG_ERROR, "weston-jni", msg)

extern struct WestonJni* westonJniPtr;

static void handle_repaint_output_pixman(pixman_image_t*);
static void handle_output_set_size(int, int);
static int handle_log(const char*, va_list);
//static int signal_sigchld_handler(int, void*);
static int signal_sigterm_handler(int, void*);
static int signal_sigusr2_handler(int, void*);


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
    struct WestonConfig* westonConfig;

    if (westonJniPtr)
        throwJavaException(env, "Only one westonJni should be created.");

    if (!(westonJni = calloc(1, sizeof(struct WestonJni)))) {
        return 0;
    }

    if (!(westonConfig = calloc(1, sizeof(struct WestonConfig)))) {
        free(westonJni);
        return 0;
    }

    westonJni->config = westonConfig;
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
        if (westonJni->display && westonJni->display_running)
            wl_display_terminate(westonJni->display);

        if (westonJni->compositor)
            weston_compositor_destroy(westonJni->compositor);

        if (westonJni->logCtx)
            weston_log_ctx_destroy(westonJni->logCtx);

        if (westonJni->display)
            wl_display_destroy(westonJni->display);

        free(westonJni->config);
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
    struct WestonConfig* westonConfig = NULL;
    struct wl_display* display = NULL;
    struct wl_event_loop* loop = NULL;
    struct weston_log_context* logCtx = NULL;
    struct weston_compositor* compositor = NULL;
    struct weston_android_backend_config* backendConfig = NULL;
    struct weston_backend* backend = NULL;
    struct weston_head* head = NULL;
    struct weston_output* output = NULL;
    struct wl_shell* shell = NULL;
    struct wl_event_source *signals[3];
    const struct weston_windowed_output_api *api = NULL;

    if (!westonJni || !westonJni->config || westonJni->compositor)
        return JNI_FALSE;

    westonConfig = westonJni->config;

    // create display
    if (!(display = wl_display_create())) {
        ANDROID_LOG("Failed to create display.");
        goto error_free;
    }

    // signal handler
    loop = wl_display_get_event_loop(display);
    signals[0] = wl_event_loop_add_signal(loop, SIGTERM, signal_sigterm_handler, display);
    signals[1] = wl_event_loop_add_signal(loop, SIGUSR2, signal_sigusr2_handler, display);

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
    /* FIXME: Is it possible to implement log_continue via android_log ? */
    weston_log_set_handler(handle_log, handle_log);

    // create compositor
    if(!(compositor = weston_compositor_create(display, logCtx, NULL, NULL))) {
        ANDROID_LOG("Failed to create compositor.");
        goto error_free;
    }

    // create config
    if (!(backendConfig = calloc(1, sizeof(struct weston_android_backend_config)))) {
        ANDROID_LOG("Failed to allocate memory for backend config.");
        goto error_free;
    }

    backendConfig->base.struct_version = WESTON_ANDROID_BACKEND_CONFIG_VERSION;
    backendConfig->base.struct_size = sizeof(struct weston_android_backend_config);
    backendConfig->refresh = westonConfig->screenRefreshRate * 1000;

    switch (westonConfig->rendererType) {
        case RENDERER_PIXMAN:
            backendConfig->renderer = WESTON_RENDERER_PIXMAN;
            break;
        case RENDERER_GL:
            backendConfig->renderer = WESTON_RENDERER_GL;
            break;
        default:
            backendConfig->renderer = WESTON_RENDERER_NOOP;
    }

    // create backend
    if (!(backend = weston_compositor_load_backend(compositor, WESTON_BACKEND_ANDROID,
                                                   (struct weston_backend_config *)backendConfig))) {
        ANDROID_LOG("Failed to create android backend.");
        goto error_free;
    }

    if(weston_compositor_backends_loaded(compositor)) {
        ANDROID_LOG("Failed to call backends loaded.");
        goto error_free;
    }

    // create head
    if (!(api = weston_windowed_output_get_api(compositor, WESTON_WINDOWED_OUTPUT_ANDROID))) {
        ANDROID_LOG("Failed to use weston_windowed_output_api.");
        goto error_free;
    }

    if (api->create_head(backend, "android")) {
        ANDROID_LOG("Failed to create android head.");
        goto error_free;
    }

    if (!(head = weston_compositor_iterate_heads(compositor, head))) {
        ANDROID_LOG("Failed get android head.");
        goto error_free;
    }

    if (head->compositor_link.next != &(compositor->head_list)) {
        ANDROID_LOG("Only one head is allowed.");
        goto error_free;
    }

    if (weston_head_is_connected(head) && !weston_head_is_enabled(head) &&! weston_head_is_non_desktop(head)) {

    } else {
        ANDROID_LOG("Get a bad head.");
        goto error_free;
    }

    //create output
    if(!(output = weston_compositor_create_output(compositor, head, head->name))) {
        ANDROID_LOG("Failed to create an output for android head.");
        goto error_free;
    }

    output->pos.c = weston_coord(0, 0);
    weston_output_set_scale(output, 1);
    weston_output_set_transform(output, WL_OUTPUT_TRANSFORM_NORMAL);
    api->output_set_size(output, westonConfig->screenWidth, westonConfig->screenHeight);

    if (weston_output_enable(output)) {
        ANDROID_LOG("Failed to enable output.");
        goto error_free;
    }

    weston_head_reset_device_changed(head);
    weston_compositor_flush_heads_changed(compositor);
    weston_compositor_wake(compositor);

    // load shell
    int (*shell_init)(struct weston_compositor* ,int*, char**);
    if (!(shell_init = weston_load_module("desktop-shell.so", "wet_shell_init", NULL))) {
        ANDROID_LOG("Failed to load shell module.");
        goto error_free;
    }

    if (shell_init(compositor, NULL, NULL)) {
        ANDROID_LOG("Failed to init shell.");
        goto error_free;
    }

    wl_log_set_handler_server((wl_log_func_t) handle_log);

    westonJni->javaObject = thiz;
    westonJni->display = display;
    westonJni->logCtx = logCtx;
    westonJni->compositor = compositor;
    westonJni->backendConfig = backendConfig;
    westonJni->backend = backend;
    westonJni->head = head;
    westonJni->output = output;
    westonJni->shell = shell;

    return JNI_TRUE;

error_free:
    if (display)
        wl_display_destroy(display);
    if (compositor)
        weston_compositor_destroy(compositor);
    if (logCtx)
        weston_log_ctx_destroy(logCtx);
    free(backendConfig);
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

    if (!westonJni || !westonJni->display || !westonJni->output || !westonJni->display_running)
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

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_setScreenSize(JNIEnv *env, jobject thiz, jlong ptr,
                                                     jint width, jint height) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->config)
        return JNI_FALSE;

    westonJni->config->screenWidth = width;
    westonJni->config->screenHeight = height;
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_setRenderer(JNIEnv *env, jobject thiz, jlong ptr,
                                                   jint renderer) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->config)
        return JNI_FALSE;

    westonJni->config->rendererType = renderer;
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_setRefreshRate(JNIEnv *env, jobject thiz, jlong ptr,
                                                      jint refresh_rate) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->config)
        return JNI_FALSE;

    westonJniPtr->config->screenRefreshRate = refresh_rate;
    return JNI_TRUE;
}

static void handle_repaint_output_pixman(pixman_image_t* pixmanImage) {
    ANativeWindow* window = NULL;
    ANativeWindow_Buffer* buffer = NULL;

    if (!westonJniPtr || !westonJniPtr->window || !westonJniPtr->buffer || !pixmanImage)
        return;

    window = westonJniPtr->window;
    buffer = westonJniPtr->buffer;

    int width = pixman_image_get_width(pixmanImage);
    int height = pixman_image_get_height(pixmanImage);
    int stride = pixman_image_get_stride(pixmanImage);
    uint32_t* src = pixman_image_get_data(pixmanImage) - stride / sizeof(uint32_t);

    if (ANativeWindow_lock(window, westonJniPtr->buffer, NULL) == 0) {
        uint32_t* dst = (uint32_t*)buffer->bits;

        for (int y = 0; y < height; y++) {
            src += stride / sizeof(uint32_t);
            memcpy(dst + y * buffer->stride, src, width * sizeof(uint32_t));
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
    return __android_log_print(ANDROID_LOG_ERROR, "weston", "%s", logBuffer);
}

static int signal_sigterm_handler(int signal, void* data) {
    if (data) {
        wl_display_terminate(data);
        return 1;
    }
    return 0;
}

static int signal_sigusr2_handler(int signal, void* data) {
    if (data) {
        wl_display_terminate(data);
        return 1;
    }
    return 0;
}
