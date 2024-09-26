#include <jni.h>
#include <android/log.h>
#include "weston-wrapper.h"
#include "weston-jni.h"

#define callJniFunction(jni, func, ...) \
    if (jni && ((struct WestonJni*)jni)->func) ((struct WestonJni*)jni)->func(__VA_ARGS__)

#define ANDROID_LOG(msg...) __android_log_print(ANDROID_LOG_ERROR, "weston-wrapper", msg)

void wrapper_notify_android_output_create(void* jni) {
    ANDROID_LOG("notify android_output_create");
    callJniFunction(jni, output_create, jni);
}

void wrapper_notify_android_output_destroy(void* jni) {
    ANDROID_LOG("notify android_output_destroy");
    callJniFunction(jni, output_destroy, jni);
}

void wrapper_notify_android_output_set_size(void* jni, int width, int height) {
    ANDROID_LOG("notify android_output_set_size width:%d height:%d", width, height);
    callJniFunction(jni, output_set_size, jni, width, height);
}

void wrapper_notify_android_repaint_output_pixman(void* jni, pixman_image_t* image) {
    callJniFunction(jni, repaint_output_pixman, jni, image);
}

void wrapper_notify_android_destroy(void* jni) {
    ANDROID_LOG("notify android_destroy");
    callJniFunction(jni, destroy, jni);
}

void wrapper_func_touch(void* jni, android_touch func) {
    ANDROID_LOG("set func touch");
    if (jni)
        ((struct WestonJni*)jni)->func_android_touch = func;
}