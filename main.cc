#include "waylandsuf.hh"
#define VK_USE_PLATFORM_WAYLAND_KHR
#include "vk.hh"
int main(int argc, char **argv) {
	toolkit::Display display;
	auto window{display.CreateWindow()};

	auto instance=tt::createInstance();
	auto surface = instance.createSurface(display.get(),window.get());

	std::cout<<"vk surface get:"<<surface<<std::endl;
	auto physicalDevice = instance.findSupportPhysicalDevices(surface);
	//auto formats = physicalDevice.getSurfaceFormatsKHR(surface);
	auto device = physicalDevice.createDeviceHelper(surface);

	window.set_toplevel();
	display.run();
	exit(0);
}
