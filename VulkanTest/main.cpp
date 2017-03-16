
#include "glfwmanager.h"
#include "CmdLineArgs.h"
#include "Demo.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    int argc;
    char **argv;

    Demo demo;

    CmdLineArgs::begin(argc, argv);

    demo.init(argc, argv);


    demo.connection = hInstance;
    strncpy(demo.name, "cube", APP_NAME_STR_LEN);

    int w = 512;
    int h = 512;

    GlfwManager glfw;
    if (!glfw.initilize()) {
        return 1;
    }
    if (!glfw.createWindow(w, h, "Hello vulkan")) {
        return 2;
    }
    demo.window = glfw.getWindow();

    demo.init_vk_swapchain();

    demo.prepare();

    while (glfw.runLoop())
    {
        demo.run();
    }
    demo.cleanup();


    CmdLineArgs::end(argc, argv);

    return 0;
}
