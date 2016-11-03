#include <iostream>
#define VK_USE_PLATFORM_WAYLAND_KHR
#include "vk.hh"

namespace tt{

Swapchain::Swapchain(vk::SwapchainKHR&& swapchain): vk::SwapchainKHR{std::move(swapchain)}{
	//std::cout<<"&&swapchain\n";
}

void Device::createBufferHelper(){
	// Vertex positions
	const float vertexData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};
  // Create a vertex buffer
	uint32_t queueIdx = 0;
	vk::BufferCreateInfo createBufferInfo{
		vk::BufferCreateFlags(),
		sizeof(vertexData),
		vk::BufferUsageFlagBits::eVertexBuffer,
		vk::SharingMode::eExclusive,
		1,
		&queueIdx
	};

	auto buffer = createBuffer(createBufferInfo);

	auto memReq = getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo allocInfo {
		memReq.size,
		0,  // Memory type assigned in the next step
	};

/*
	MapMemoryTypeToIndex(memReq.memoryTypeBits,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                              &allocInfo.memoryTypeIndex);

  // Allocate memory for the buffer
  VkDeviceMemory deviceMemory;
  CALL_VK(vkAllocateMemory(device.device_, &allocInfo, nullptr, &deviceMemory));

  void* data;
  CALL_VK(vkMapMemory(device.device_, deviceMemory, 0, sizeof(vertexData), 0,
                      &data));
  memcpy(data, vertexData, sizeof(vertexData));
  vkUnmapMemory(device.device_, deviceMemory);

  CALL_VK(vkBindBufferMemory(device.device_, buffers.vertexBuf, deviceMemory, 0));
*/}
void Device::createGraphicsPipelineHelper(){

}
	
vk::RenderPass Device::createRenderPasshelper(vk::SurfaceFormatKHR& surfaceFormat){
	vk::AttachmentDescription attachmentDescriptions{
		vk::AttachmentDescriptionFlags(),
		surfaceFormat.format,
		vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::eColorAttachmentOptimal
	};

	vk::AttachmentReference colourReference{0, vk::ImageLayout::eColorAttachmentOptimal};
	vk::SubpassDescription subpassDescription {
		vk::SubpassDescriptionFlags(),
		vk::PipelineBindPoint::eGraphics,
		0,nullptr,1,&colourReference,
		nullptr,nullptr,0,nullptr
	};

	return createRenderPass(
		vk::RenderPassCreateInfo{
			vk::RenderPassCreateFlags(),
			1,&attachmentDescriptions,
			1,&subpassDescription,
			0,nullptr
		}
	);
}

Device::Device(PhysicalDevice& pD,vk::Device&& device,vk::SurfaceKHR& surface,vk::SurfaceFormatKHR surfaceFormat):
	vk::Device{std::move(device)},
	physicalDevice{pD},
	swapchain{createSwapchainKHR(
		vk::SwapchainCreateInfoKHR{
			vk::SwapchainCreateFlagsKHR(),surface,1,
			surfaceFormat.format,surfaceFormat.colorSpace,
			vk::Extent2D{1024,768},1,vk::ImageUsageFlagBits::eColorAttachment
		}
	)},
	renderPass{createRenderPasshelper(surfaceFormat)}
{
	auto swapchainImages{getSwapchainImagesKHR(swapchain)};
	for(auto& swapchainImage:swapchainImages){
		vk::ImageViewCreateInfo imageViewCreateInfo{
			vk::ImageViewCreateFlags(),
			swapchainImage,
			vk::ImageViewType::e2D,
			surfaceFormat.format,
			vk::ComponentMapping{
				vk::ComponentSwizzle::eR,
				vk::ComponentSwizzle::eG,
				vk::ComponentSwizzle::eB,
				vk::ComponentSwizzle::eA
			},
			vk::ImageSubresourceRange{
				vk::ImageAspectFlagBits::eColor,0,1,0,1
			}
		};
		imageViews.emplace_back(createImageView(imageViewCreateInfo));
		
		vk::FramebufferCreateInfo framebufferCreateInfo{
			vk::FramebufferCreateFlags(),renderPass,1,&*imageViews.rbegin(),1024,768,1
		};
		framebuffers.emplace_back(createFramebuffer(framebufferCreateInfo));
	}
}


Device PhysicalDevice::createDeviceHelper(vk::SurfaceKHR& surface){
	auto surfaceFormats{getSurfaceFormatsKHR(surface)};
	for(auto& surfaceFormat:surfaceFormats){
		std::cout<<vk::to_string(surfaceFormat.format)<<':'<<vk::to_string(surfaceFormat.colorSpace)<<std::endl;
		float queuePriorities[1] = { 0.0 };
		vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
		deviceQueueCreateInfo.setQueueFamilyIndex(1).setQueueCount(1).setPQueuePriorities(queuePriorities);
		std::array<const char *,1> extname{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		vk::DeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo.setQueueCreateInfoCount(1).setPQueueCreateInfos(&deviceQueueCreateInfo).setEnabledExtensionCount(extname.size()).setPpEnabledExtensionNames(extname.data());
		return Device{*this,createDevice(deviceCreateInfo),surface,surfaceFormat};
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
	auto physicalDevices{enumeratePhysicalDevices()};
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

