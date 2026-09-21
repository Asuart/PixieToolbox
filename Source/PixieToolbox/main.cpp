#include "PixieToolbox.h"

using namespace PixieToolbox;

int main(int /*argc*/, char** /*argv*/) {
	PixieToolboxApp app("Pixie Toolbox", { 1280, 720 }, RenderAPI::Vulkan);

	app.Start();

	return 0;
}
