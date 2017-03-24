
#include "glfwmanager.h"
#include "CmdLineArgs.h"
#include "Demo.h"

#include "DebugConsole.h"


#define APP_SHORT_NAME "VulkanTest"






int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
    CmdLineArgs args;
    GlfwManager glfw;
    Demo demo;

    DebugConsole::Init();

    demo.init(args.argc(), args.argv(), APP_SHORT_NAME);

    if (!glfw.initilize()) {
        return 1;
    }

    if (!glfw.createWindow(512, 512, APP_SHORT_NAME)) {
        return 2;
    }

    demo.init_vk_swapchain(hInstance, glfw.getWindow());

    demo.prepare();

    while (glfw.runLoop())
    {
        demo.run();
    }
    demo.cleanup();


    DebugConsole::Term();

    glfw.finalize();

    return 0;
}
