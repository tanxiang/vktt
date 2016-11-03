#pragma once
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.hpp>
#include <utility>
namespace tt{
class Device;

class Swapchain:public vk::SwapchainKHR{
	//Device& Device;
public:
	Swapchain(vk::SwapchainKHR&& swapchain);
	~Swapchain(){
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

class RenderPass:public vk::RenderPass{
	//Device& Device;
public:
	RenderPass(vk::RenderPass&& renderPass):vk::RenderPass{std::move(renderPass)}{
	};
};

class ImageView:public vk::ImageView{
	//Device& Device;
public:
	ImageView(vk::ImageView&& imageView):vk::ImageView{std::move(imageView)}{
	};
};

class Framebuffer:public vk::Framebuffer{
	//Device& Device;
public:
	Framebuffer(vk::Framebuffer&& framebuffer):vk::Framebuffer{std::move(framebuffer)}{
	};
};

class Device:public vk::Device{
	PhysicalDevice& physicalDevice;
	Swapchain swapchain;
	vk::RenderPass renderPass;
	std::vector<ImageView> imageViews;
	std::vector<Framebuffer> framebuffers;
	vk::RenderPass createRenderPasshelper(vk::SurfaceFormatKHR& surfaceFormat);
public:
	void createBufferHelper();
	void createGraphicsPipelineHelper();
	bool crawFrame(){return false;}
	Device(PhysicalDevice& physicalDevice,vk::Device&& device,vk::SurfaceKHR& surface,vk::SurfaceFormatKHR surfaceFormat);
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
