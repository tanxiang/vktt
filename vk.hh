#include "vulkan.hpp"
#include <utility>
namespace tt{
class PhysicalDevice:public vk::PhysicalDevice{
public:
	PhysicalDevice(vk::PhysicalDevice&& physicalDevice):vk::PhysicalDevice{std::move(physicalDevice)}{
	}
	vk::Device createDeviceHelper(vk::SurfaceKHR& surface);
};

class Instance:public vk::Instance{
public:
	Instance(vk::Instance&& instance):vk::Instance{std::move(instance)}{
	}
	vk::SurfaceKHR createSurface(wl_display *display,wl_surface *surface);
	PhysicalDevice findSupportPhysicalDevices(vk::SurfaceKHR& surface);
};

auto createInstance(){
#ifdef VK_Validation
	const char * extension_name[3] 
#else
	const char * extension_name[2] 
#endif
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#ifdef VK_Validation
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
	};
	vk::ApplicationInfo appInfo{
		"vktt",
		VK_MAKE_VERSION(0, 1, 5),
		"vktt",
		VK_MAKE_VERSION(0, 1, 5),
		VK_MAKE_VERSION(1, 0, 5)
	};
	vk::InstanceCreateInfo instaceCreateInfo{
		vk::InstanceCreateFlags(),
		&appInfo,
		0,
		nullptr,
		sizeof(extension_name)/sizeof(const char *),
		extension_name
	};
	return tt::Instance{vk::createInstance(instaceCreateInfo)};
}

}
