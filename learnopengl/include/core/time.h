#pragma once

#include <GLFW/glfw3.h>

namespace Time {

inline double GetTime() {
    return glfwGetTime();
}

inline void SetTime(double time) {
    return glfwSetTime(time);
}

}  // namespace Time
