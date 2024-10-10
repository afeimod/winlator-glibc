#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <libweston/libweston.h>
#include <libweston/windowed-output-api.h>
#include "weston-jni.h"
#include "rect.h"

#define epsilon 0.00001f

#define ANDROID_LOG(msg...) __android_log_print(ANDROID_LOG_ERROR, "weston-jni", msg)

static void handle_repaint_output_pixman(struct WestonJni*, pixman_image_t*);
static void handle_output_set_size(struct WestonJni*, int, int);
static int handle_log(const char*, va_list);
static void handle_xdg_log(struct xkb_context*, enum xkb_log_level, const char*, va_list);
//static int signal_sigchld_handler(int, void*);
static int signal_sigterm_handler(int, void*);
static int signal_sigusr2_handler(int, void*);
static void copyRect(JNIEnv*, ARect*, jobject);
static void copyString(JNIEnv*, char*, size_t, jstring);
static void updateBuffersGeometry(JNIEnv*, long);
static void updateOutputStatus(JNIEnv*, long);
static void updateEnvVars(struct WestonConfig*);
static inline void scaleOutputToDisplay(struct WestonConfig*, pixman_image_t*);

static inline void throwJavaException(JNIEnv* env, const char* msg) {
    jclass runtimeExceptionClass = (*env)->FindClass(env, "java/lang/RuntimeException");
    if (!runtimeExceptionClass) {
        return;
    }
    (*env)->ThrowNew(env, runtimeExceptionClass, msg);
}

static inline struct WestonJni* getWestonJniFromPtr(JNIEnv* env, jlong ptr) {
    if (!ptr) {
        throwJavaException(env, "Not called create() yet.");
        return NULL;
    }
    return (struct WestonJni*) ptr;
}

JNIEXPORT jlong JNICALL
Java_org_freedesktop_wayland_WestonJni_create(JNIEnv* env, jobject thiz) {
    struct WestonJni* westonJni;

    if (!(westonJni = calloc(1, sizeof(struct WestonJni))))
        return (jlong) NULL;

    westonJni->javaObject = thiz;
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
        if (westonJni->display && westonJni->display_running)
            wl_display_terminate(westonJni->display);

        if (westonJni->compositor)
            weston_compositor_destroy(westonJni->compositor);

        if (westonJni->logCtx)
            weston_log_ctx_destroy(westonJni->logCtx);

        if (westonJni->display)
            wl_display_destroy(westonJni->display);

        free(westonJni->backendConfig);
        free(westonJni);
    }
}

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_setSurface(JNIEnv* env, jobject thiz, jlong ptr,
                                                     jobject surface) {
    ANativeWindow* window = NULL;
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni)
        return JNI_FALSE;

    if (!surface) {
        westonJni->window = NULL;
        return JNI_TRUE;
    }

    if (!(window = ANativeWindow_fromSurface(env, surface)))
        return JNI_FALSE;

    westonJni->window = window;
    updateBuffersGeometry(env, ptr);

    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_org_freedesktop_wayland_WestonJni_updateConfig(JNIEnv* env, jobject thiz, jlong ptr,
                                                    jobject config) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !config)
        return JNI_FALSE;

    struct WestonConfig* westonConfig = &westonJni->config;
    jclass objClass = (*env)->GetObjectClass(env, config);

    // convert config from java to c
    jfieldID rendererTypeFieId = (*env)->GetFieldID(env, objClass, "rendererType", "I");
    jfieldID renderRefreshRateFieId = (*env)->GetFieldID(env, objClass, "renderRefreshRate", "I");
    jfieldID screenRectFieId = (*env)->GetFieldID(env, objClass, "screenRect", "Landroid/graphics/Rect;");
    jfieldID displayRectFieId = (*env)->GetFieldID(env, objClass, "displayRect", "Landroid/graphics/Rect;");
    jfieldID renderRectFieId = (*env)->GetFieldID(env, objClass, "renderRect", "Landroid/graphics/Rect;");
    jfieldID socketPathFieId = (*env)->GetFieldID(env, objClass, "socketPath", "Ljava/lang/String;");
    jfieldID xdgConfigPathFieId = (*env)->GetFieldID(env, objClass, "xdgConfigPath", "Ljava/lang/String;");
    jfieldID xdgRuntimePathFieId = (*env)->GetFieldID(env, objClass, "xdgRuntimePath", "Ljava/lang/String;");
    jfieldID xkbRuleFieId = (*env)->GetFieldID(env, objClass, "xkbRule", "Ljava/lang/String;");
    jfieldID xkbModelFieId = (*env)->GetFieldID(env, objClass, "xkbModel", "Ljava/lang/String;");
    jfieldID xkbLayoutFieId = (*env)->GetFieldID(env, objClass, "xkbLayout", "Ljava/lang/String;");

    westonConfig->rendererType = (*env)->GetIntField(env, config, rendererTypeFieId);
    westonConfig->renderRefreshRate = (*env)->GetIntField(env, config, renderRefreshRateFieId);
    copyRect(env, &westonConfig->screenRect, (*env)->GetObjectField(env, config, screenRectFieId));
    copyRect(env, &westonConfig->displayRect, (*env)->GetObjectField(env, config, displayRectFieId));
    copyRect(env, &westonConfig->renderRect, (*env)->GetObjectField(env, config, renderRectFieId));
    copyString(env, westonConfig->socketPath, sizeof(westonConfig->socketPath),
               (*env)->GetObjectField(env, config, socketPathFieId));
    copyString(env, westonConfig->xdgConfigPath, sizeof(westonConfig->xdgConfigPath),
               (*env)->GetObjectField(env, config, xdgConfigPathFieId));
    copyString(env, westonConfig->xdgRuntimePath, sizeof(westonConfig->xdgRuntimePath),
               (*env)->GetObjectField(env, config, xdgRuntimePathFieId));
    copyString(env, westonConfig->xkbRules[0], XKB_STR_MAX,
               (*env)->GetObjectField(env, config, xkbRuleFieId));
    copyString(env, westonConfig->xkbRules[1], XKB_STR_MAX,
               (*env)->GetObjectField(env, config, xkbModelFieId));
    copyString(env, westonConfig->xkbRules[2], XKB_STR_MAX,
               (*env)->GetObjectField(env, config, xkbLayoutFieId));

    updateBuffersGeometry(env, ptr);
    updateOutputStatus(env, ptr);
    updateEnvVars(westonConfig);
    return JNI_TRUE;
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
    struct xkb_rule_names xkb_names;

    if (!westonJni || westonJni->compositor)
        return JNI_FALSE;

    westonConfig = &westonJni->config;

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
    if (wl_display_add_socket(display, westonConfig->socketPath)) {
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

    // create xkb
    if (weston_compositor_set_xkb_rule_names(compositor, &xkb_names)) {
        ANDROID_LOG("Failed to create xkb rules.");
        goto error_free;
    }

    xkb_context_set_log_fn(compositor->xkb_context, handle_xdg_log);

    // create config
    if (!(backendConfig = calloc(1, sizeof(struct weston_android_backend_config)))) {
        ANDROID_LOG("Failed to allocate memory for backend config.");
        goto error_free;
    }

    backendConfig->base.struct_version = WESTON_ANDROID_BACKEND_CONFIG_VERSION;
    backendConfig->base.struct_size = sizeof(struct weston_android_backend_config);
    backendConfig->refresh = westonConfig->renderRefreshRate * 1000;
    backendConfig->jni = westonJni;

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
    api->output_set_size(output, ARectGetWidth(&westonConfig->renderRect),
                         ARectGetHeight(&westonConfig->renderRect));

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

JNIEXPORT void JNICALL
Java_org_freedesktop_wayland_WestonJni_performTouch(JNIEnv *env, jobject thiz, jlong ptr,
                                                    jint touch_id, jint touch_type, jfloat x,
                                                    jfloat y) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->func_android_touch)
        return;

    westonJni->func_android_touch(westonJni->backend, touch_id, touch_type, x, y);
}

JNIEXPORT void JNICALL
Java_org_freedesktop_wayland_WestonJni_performKey(JNIEnv *env, jobject thiz, jlong ptr, jint key,
                                                  jint key_state) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->func_android_touch)
        return;

    westonJni->func_android_keyboard(westonJni->backend, key, key_state);
}

static void handle_repaint_output_pixman(struct WestonJni* westonJni, pixman_image_t* srcImg) {
    ANativeWindow* window = NULL;
    ANativeWindow_Buffer* buffer = NULL;
    pixman_image_t* dstImg = NULL;
    struct WestonConfig* config = NULL;

    if (!westonJni || !westonJni->window || !srcImg)
        return;

    config = &westonJni->config;
    window = westonJni->window;
    buffer = &westonJni->buffer;

    if (config->isScaled) {
        dstImg = config->compositeImg;
        scaleOutputToDisplay(config, srcImg);
    } else
        dstImg = srcImg;

    int width = pixman_image_get_width(dstImg);
    int height = pixman_image_get_height(dstImg);
    int stride = pixman_image_get_stride(dstImg);
    uint32_t* src = pixman_image_get_data(dstImg) - stride / sizeof(uint32_t);

    if (ANativeWindow_lock(window, buffer, NULL) == 0) {
        uint32_t* dst = (uint32_t*)buffer->bits;

        for (int y = 0; y < height; y++) {
            src += stride / sizeof(uint32_t);
            memcpy(dst + y * buffer->stride, src, width * sizeof(uint32_t));
        }

        ANativeWindow_unlockAndPost(window);
    }
}

static void handle_output_set_size(struct WestonJni* westonJni, int width, int height) {

}

static int handle_log(const char *fmt, va_list ap) {
    static char logBuffer[256];
    vsnprintf(logBuffer, sizeof(logBuffer), fmt, ap);
    logBuffer[255] = '\0';
    return __android_log_print(ANDROID_LOG_DEBUG, "weston", "%s", logBuffer);
}

static void handle_xdg_log(struct xkb_context* ctx, enum xkb_log_level level, const char* fmt, va_list args) {
    handle_log(fmt, args);
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

static void copyRect(JNIEnv* env, ARect* aRect, jobject jRect) {
    jclass rectClass = (*env)->GetObjectClass(env, jRect);

    jfieldID leftField = (*env)->GetFieldID(env, rectClass, "left", "I");
    jfieldID topField = (*env)->GetFieldID(env, rectClass, "top", "I");
    jfieldID rightField = (*env)->GetFieldID(env, rectClass, "right", "I");
    jfieldID bottomField = (*env)->GetFieldID(env, rectClass, "bottom", "I");

    aRect->left = (*env)->GetIntField(env, jRect, leftField);
    aRect->top = (*env)->GetIntField(env, jRect, topField);
    aRect->right = (*env)->GetIntField(env, jRect, rightField);
    aRect->bottom = (*env)->GetIntField(env, jRect, bottomField);
}

static void copyString(JNIEnv* env, char* dest, size_t len, jstring jString) {
    const char* nString = (*env)->GetStringUTFChars(env, jString, 0);
    size_t nLen = strlen(nString);

    if (nLen + 1 > len)
        goto free;

    strncpy(dest, nString, nLen + 1);

free:
    (*env)->ReleaseStringUTFChars(env, jString, nString);
}

static void updateBuffersGeometry(JNIEnv* env, long ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni || !westonJni->window)
        return;

    ANativeWindow_setBuffersGeometry(westonJni->window,
                                     ARectGetWidth(&westonJni->config.screenRect),
                                     ARectGetHeight(&westonJni->config.screenRect),
                                     WINDOW_FORMAT_RGBX_8888);
}

static void updateOutputStatus(JNIEnv* env, long ptr) {
    struct WestonJni* westonJni = getWestonJniFromPtr(env, ptr);

    if (!westonJni)
        return;

    struct WestonConfig* config = &westonJni->config;
    struct ARect* screenRect = &config->screenRect;
    struct ARect* displayRect = &config->displayRect;
    struct ARect* renderRect = &config->renderRect;

    config->displayWidth = ARectGetWidth(displayRect);
    config->displayHeight = ARectGetHeight(displayRect);
    config->outputScaleX = (float) config->displayWidth / ARectGetWidth(renderRect);
    config->outputScaleY = (float) config->displayHeight / ARectGetHeight(renderRect);
    config->outputStartX = displayRect->left;
    config->outputStartY = displayRect->top;
    config->isScaled = !(fabsf(config->outputScaleX - 1) < epsilon && fabsf(config->outputScaleY - 1) < epsilon);

    if (config->compositeImg) {
        pixman_image_unref(config->compositeImg);
        config->compositeImg = NULL;
    }

    if (config->isScaled) {
        config->compositeImg = pixman_image_create_bits(
                PIXMAN_a8r8g8b8,
                ARectGetWidth(screenRect),
                ARectGetHeight(screenRect),
                NULL,
                0);
    }
}

static void updateEnvVars(struct WestonConfig* config) {
    setenv("XDG_CONFIG_HOME", config->xdgConfigPath, 1);
    setenv("XDG_RUNTIME_DIR", config->xdgRuntimePath, 1);
}

static void scaleOutputToDisplay(struct WestonConfig* config, pixman_image_t* in) {
    pixman_transform_t transform;
    pixman_transform_init_identity(&transform);
    pixman_transform_scale(NULL, &transform,
                           pixman_double_to_fixed(config->outputScaleX),
                           pixman_double_to_fixed(config->outputScaleY));
    pixman_image_set_transform(in, &transform);

    pixman_image_composite(
            PIXMAN_OP_SRC,
            in,
            NULL,
            config->compositeImg,
            0, 0,
            0, 0,
            config->outputStartX, config->outputStartY,
            config->displayWidth,
            config->displayHeight
    );
}