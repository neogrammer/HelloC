#ifndef HELLOC_INPUT_UTIL_HPP
#define HELLOC_INPUT_UTIL_HPP

#include <jni.h>
#include <android/input.h>

// "Cooked" event types. This is our own enumeration.
enum {
    COOKED_EVENT_TYPE_JOY,
    COOKED_EVENT_TYPE_POINTER_DOWN,
    COOKED_EVENT_TYPE_POINTER_UP,
    COOKED_EVENT_TYPE_POINTER_MOVE,
    COOKED_EVENT_TYPE_KEY_DOWN,
    COOKED_EVENT_TYPE_KEY_UP,
    COOKED_EVENT_TYPE_BACK
};

// "Cooked" event. This is our own event structure.
struct CookedEvent {
    int type;

    // for JOY events
    float joyX, joyY;

    // for POINTER events
    int motionPointerId;
    bool motionIsOnScreen;
    float motionX, motionY;
    float motionMinX, motionMaxX, motionMinY, motionMaxY;

    // for KEY events
    int keyCode;
};

// Callback to be called to deliver a "cooked" event.
typedef bool (*CookedEventCallback)(struct CookedEvent *event);

// "Cooks" a raw event and delivers it to the given callback.
bool CookEvent(AInputEvent *event, CookedEventCallback callback);

#endif //HELLOC_INPUT_UTIL_HPP
