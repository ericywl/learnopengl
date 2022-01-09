#pragma once

#include <core/window.h>

#include <sstream>
#include <vector>

class Application {
   private:
    std::vector<Window *> m_Windows;

   public:
    Application(const int width, const int height, const std::string &name);
    ~Application();

    Window &NewWindow(const int width, const int height, const std::string &name);
    Window &GetWindow(unsigned int i = 0);
};
