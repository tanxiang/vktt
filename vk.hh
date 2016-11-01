#pragma once
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.hpp>
#include <utility>
namespace tt{
class Swapchain:public vk::SwapchainKHR{
public:
	Swapchain(vk::SwapchainKHR&& swapchain);
	~Swapchain(){
	}
};

class Device:public vk::Device{
	Swapchain swapchain;
	vk::RenderPass renderPass;
	std::vector<vk::ImageView> imageViews;
	std::vector<vk::Framebuffer> framebuffers;
	vk::RenderPass createRenderPasshelper(vk::SurfaceFormatKHR& surfaceFormat);
public:
	void createBufferHelper();
	void createGraphicsPipelineHelper();
	Device(vk::Device&& device,vk::SurfaceKHR& surface,vk::SurfaceFormatKHR surfaceFormat);
	~Device(){
		destroySwapchainKHR(swapchain);
		destroyRenderPass(renderPass);
		for(auto &imageView:imageViews)
			destroyImageView(imageView);
		for(auto &framebuffer:framebuffers)
			destroyFramebuffer(framebuffer);
		destroy();
	}
};

class PhysicalDevice:public vk::PhysicalDevice{
public:
	PhysicalDevice(vk::PhysicalDevice&& physicalDevice):vk::PhysicalDevice{std::move(physicalDevice)}{
	}
	Device createDeviceHelper(vk::SurfaceKHR& surface);
	~PhysicalDevice(){
		//destroy();
	}
};

class Instance:public vk::Instance{
public:
	Instance(vk::Instance&& instance):vk::Instance{std::move(instance)}{
	}
	vk::SurfaceKHR createSurface(wl_display *display,wl_surface *surface);
	PhysicalDevice findSupportPhysicalDevices(vk::SurfaceKHR& surface);
	~Instance(){
		destroy();
	}
};

Instance createInstance();

}
