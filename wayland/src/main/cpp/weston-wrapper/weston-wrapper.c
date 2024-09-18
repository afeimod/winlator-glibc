#include <jni.h>
#include "weston-wrapper.h"
#include "weston-jni.h"

#define callJniFunction(func, ...) \
    if (westonJniPtr != NULL) westonJniPtr->func(__VA_ARGS__)

extern struct WestonJni* westonJniPtr;

void wrapper_notify_android_output_create() {
    callJniFunction(output_create);
}

void wrapper_notify_android_output_destroy() {
    callJniFunction(output_destroy);
}

void wrapper_notify_android_output_set_size(int width, int height) {
    callJniFunction(output_set_size, width, height);
}

void wrapper_notify_android_repaint_output_pixman(pixman_image_t* image) {
    callJniFunction(repaint_output_pixman, image);
}

void wrapper_notify_android_destroy() {
    callJniFunction(destroy);
}