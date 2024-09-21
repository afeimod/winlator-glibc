#include <jni.h>
#include <android/log.h>
#include "weston-wrapper.h"
#include "weston-jni.h"

#define callJniFunction(func, ...) \
    if (westonJniPtr && westonJniPtr->func) westonJniPtr->func(__VA_ARGS__)

#define ANDROID_LOG(msg...) __android_log_print(ANDROID_LOG_ERROR, "weston-wrapper", msg)

struct WestonJni* westonJniPtr = NULL;

void wrapper_notify_android_output_create() {
    ANDROID_LOG("notify android_output_create");
    callJniFunction(output_create);
}

void wrapper_notify_android_output_destroy() {
    ANDROID_LOG("notify android_output_destroy");
    callJniFunction(output_destroy);
}

void wrapper_notify_android_output_set_size(int width, int height) {
    ANDROID_LOG("notify android_output_set_size width:%d height:%d", width, height);
    callJniFunction(output_set_size, width, height);
}

void wrapper_notify_android_repaint_output_pixman(pixman_image_t* image) {
    callJniFunction(repaint_output_pixman, image);
}

void wrapper_notify_android_destroy() {
    ANDROID_LOG("notify android_destroy");
    callJniFunction(destroy);
}