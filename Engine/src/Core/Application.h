#pragma once

#include <string>

class Application {
public:
    static void Init(int width = 1280, int height = 720, const std::string &title = "Window");

    static void Run();

    static void Destroy();
};
