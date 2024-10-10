#ifndef WESTON_WRAPPER_H
#define WESTON_WRAPPER_H

#include <pixman.h>
#include "input_ops.h"

#ifndef EXPORT
#define EXPORT __attribute__((visibility("default")))
#endif

EXPORT void wrapper_notify_android_output_create(void* jni);

EXPORT void wrapper_notify_android_output_destroy(void* jni);

EXPORT void wrapper_notify_android_output_set_size(void* jni, int width, int height);

EXPORT void wrapper_notify_android_repaint_output_pixman(void* jni, pixman_image_t* image);

EXPORT void wrapper_notify_android_destroy(void* jni);

EXPORT void wrapper_func_touch(void* jni, android_touch func);

EXPORT void wrapper_func_key(void* jni, android_keyboard func);

EXPORT bool update_xkb_rules(void* jni, const char** rule, const char** model, const char** layout);

#endif //WESTON_WRAPPER_H
