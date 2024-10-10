#ifndef WINLATOR_INPUT_OPS_H
#define WINLATOR_INPUT_OPS_H

#include <libweston/libweston.h>

enum PointerType {
    ABSOLUTE_POS = 0,
    RELATIVE_POS
};

enum AxisType {
    VERTICAL_SCROLL = 0,
    HORIZONTAL_SCROLL
};

typedef void (*android_touch)(struct weston_backend* b, int touchId, int touchType, float x, float y);

typedef void (*android_keyboard)(struct weston_backend* b, int key, int keyState);

typedef void (*android_pointer)(struct weston_backend* b, int pointerType, float x, float y);

typedef void (*android_button)(struct weston_backend* b, int button, int buttonState);

typedef void (*android_axis)(struct weston_backend* b, int axisType, float value, bool hasDiscrete, int discrete);

#endif //WINLATOR_INPUT_OPS_H
