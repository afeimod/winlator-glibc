#include "rect.h"

uint32_t ARectGetWidth(ARect* rect) {
    return rect->right - rect->left;
}

uint32_t ARectGetHeight(ARect* rect){
    return rect->bottom - rect->top;
}