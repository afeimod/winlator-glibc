#ifndef WINLATOR_INPUT_OPS_H
#define WINLATOR_INPUT_OPS_H

#include <libweston/libweston.h>

typedef void (*android_touch)(struct weston_backend* b, int touchId, int touchType, float x, float y);

typedef void (*android_keyboard)(struct weston_backend*b, int key, int keyState);

#endif //WINLATOR_INPUT_OPS_H
