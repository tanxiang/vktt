#include <iostream>
#define VK_USE_PLATFORM_WAYLAND_KHR
#include "vk.hh"

namespace tt{

vk::Device PhysicalDevice::createDeviceHelper(vk::SurfaceKHR& surface){
	auto surfaceFormats = getSurfaceFormatsKHR(surface);
	for(auto& surfaceFormat:surfaceFormats){
		std::cout<<vk::to_string(surfaceFormat.format)<<':'<<vk::to_string(surfaceFormat.colorSpace)<<std::endl;
		float queue_priorities[1] = { 0.0 };
		vk::DeviceQueueCreateInfo dc_q_info;
		dc_q_info.setQueueFamilyIndex(1).setQueueCount(1).setPQueuePriorities(queue_priorities);
		std::array<const char *,1> extname{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		vk::DeviceCreateInfo dc_info;
		dc_info.setQueueCreateInfoCount(1).setPQueueCreateInfos(&dc_q_info).setEnabledExtensionCount(extname.size()).setPpEnabledExtensionNames(extname.data());
		vk::SwapchainCreateInfoKHR scc_info;
		return createDevice(dc_info);
	}
	throw std::logic_error( "createDevice not found surfaceFormat!" );
}

vk::SurfaceKHR Instance::createSurface(wl_display *display,wl_surface *surface){
	vk::WaylandSurfaceCreateInfoKHR surfaceCreateInfo{
		vk::WaylandSurfaceCreateFlagsKHR{},
		display,
		surface
	};
	return createWaylandSurfaceKHR(surfaceCreateInfo);
}

PhysicalDevice Instance::findSupportPhysicalDevices(vk::SurfaceKHR& surface){
	auto physicalDevices = enumeratePhysicalDevices();
	for(auto& physicalDevice:physicalDevices){
		auto physicalDeviceProperties=physicalDevice.getProperties();
		std::cout<<"name:"<<physicalDeviceProperties.deviceName<<
			"\n\ttype:"<<vk::to_string(physicalDeviceProperties.deviceType)<<std::endl;
		auto physicalDeviceQueueFamilyProperties = physicalDevice.getQueueFamilyProperties();
		uint32_t i = 0;
		for (; i < physicalDeviceQueueFamilyProperties.size(); ++i) {
			std::cout<<vk::to_string(physicalDeviceQueueFamilyProperties[i].queueFlags)<<std::endl;
			if(physicalDeviceQueueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics){
				if(physicalDevice.getSurfaceSupportKHR(i,surface)){
					std::cout<<"getSurfaceSupportKHR true"<<std::endl;
					break;
				}
			}
		}
		return PhysicalDevice{std::move(physicalDevice)};
	}
	throw std::logic_error( "Support PhysicalDevice not found!" );
}

Instance createInstance(){
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
}//namespace tt

