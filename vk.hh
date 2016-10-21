#pragma once
#include "vulkan.hpp"
#include <utility>
namespace tt{
class Device:public vk::Device{
	vk::SwapchainKHR swapchain;
	vk::RenderPass renderPass;
	vk::ImageView ImageView;
	vk::Framebuffer framebuffer;
	vk::RenderPass createRenderPasshelper(vk::SurfaceFormatKHR& surfaceFormat);
public:
	Device(vk::Device&& device,vk::SurfaceKHR& surface,vk::SurfaceFormatKHR surfaceFormat);
};

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

Instance createInstance();

}
