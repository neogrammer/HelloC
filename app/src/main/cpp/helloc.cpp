
#include "core/native_engine.hpp"

extern "C" {
void android_main(struct android_app* state);
};

void android_main(struct android_app* app) {
    NativeEngine* engine = new NativeEngine(app);
    engine->GameLoop();
    delete engine;
}
