#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <PixieToolboxCore.h>

#include "PixieToolbox.h"

int32_t main(int argc, char** argv) {
    Config::SetApplicationPath(argv[0]);

    MainWindow* mainWindow = PixieToolboxCore::CreateMainWindow("Pixie Toolbox", { 1920, 1080 }, RenderAPI::OpenGL);

    mainWindow->SetApplication(new PixieToolbox());
    mainWindow->Start();

    delete mainWindow;

    return 0;
}
