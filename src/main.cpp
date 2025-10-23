#include "App.hpp"

int main()
{
    const auto app = App::create({
        .name       = "Luna::Metal",
        .windowSize = { 1280, 720 },
    });

    app->run();

    return 0;
}
