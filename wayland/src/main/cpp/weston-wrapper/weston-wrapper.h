#ifndef WESTON_WRAPPER_H
#define WESTON_WRAPPER_H

#include "pixman.h"

#ifndef EXPORT
#define EXPORT __attribute__((visibility("default")))
#endif

EXPORT void wrapper_notify_android_output_create();

EXPORT void wrapper_notify_android_output_destroy();

EXPORT void wrapper_notify_android_output_set_size(int width, int height);

EXPORT void wrapper_notify_android_repaint_output_pixman(pixman_image_t* image);

EXPORT void wrapper_notify_android_destroy();


#endif //WESTON_WRAPPER_H
