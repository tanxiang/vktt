#include <iostream>
#define VK_USE_PLATFORM_WAYLAND_KHR
#include "vk.hh"
#include "fmap.hh"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace tt{

auto loadShaderWithDevice(Device& device,const char* fileName){
	fs::path p = fs::current_path() / fileName;
	return true;
}

uint32_t PhysicalDevice::mapMemoryTypeToIndex(uint32_t typeBits,
                              vk::MemoryPropertyFlagBits requirementsMask) {
	auto memoryProperties = getMemoryProperties();
  // Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < 32; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((memoryProperties.memoryTypes[i].propertyFlags &
				requirementsMask) == requirementsMask) {
				return i;
			}
		}
		typeBits >>= 1;
	}
	return 0;//FIXME throw??
}

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
		physicalDevice.mapMemoryTypeToIndex(memReq.memoryTypeBits,vk::MemoryPropertyFlagBits::eHostVisible)
	};
	auto deviceMemory = allocateMemory(allocInfo);
	void *data = mapMemory(deviceMemory,0,sizeof(vertexData));
	memcpy(data, vertexData, sizeof(vertexData));
	unmapMemory(deviceMemory);
	bindBufferMemory(buffer,deviceMemory,0);
	//FIXME need freeMemory(deviceMemory);
}

void Device::createGraphicsPipelineHelper(){
	auto pipelineLayout = createPipelineLayout(	vk::PipelineLayoutCreateInfo{});
	vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
	
	//vk::ShaderModule vertexShader,fragmentShader;
	//auto [vsContext,vsSize] = fileMapBuf("shaders/tri.vert.spv"); //c++1z
	auto vsContext = fileMapBuf("shaders/tri.vert.spv");
	vk::ShaderModuleCreateInfo vertexShaderInfo{
		vk::ShaderModuleCreateFlags(),
		vsContext.second,
		static_cast<const uint32_t*>(vsContext.first.get())
	};

	auto vertexShader = createShaderModule(vertexShaderInfo);
	auto fsContext = fileMapBuf("shaders/tri.frag.spv"); 
	vk::ShaderModuleCreateInfo fragmentShaderInfo{
		vk::ShaderModuleCreateFlags(),
		fsContext.second,
		static_cast<const uint32_t*>(fsContext.first.get())
	};
	
	auto fragmentShader = createShaderModule(fragmentShaderInfo);
	vk::PipelineShaderStageCreateInfo pipelineShaderStageCreateInfo[2]{
		{
			vk::PipelineShaderStageCreateFlags(),
			vk::ShaderStageFlagBits::eVertex,
			vertexShader,
			"main",
			nullptr
		},
		{
			vk::PipelineShaderStageCreateFlags(),
			vk::ShaderStageFlagBits::eFragment,
			fragmentShader,
			"main",
			nullptr
		}
	};

	// Specify viewport info
	vk::Viewport viewport{
		0.0,1.0,
		0,0,
		1024,768
	};
	vk::Rect2D scissor{
		{1024,768},{0,0}
	};
	vk::PipelineViewportStateCreateInfo viewportInfo{
		vk::PipelineViewportStateCreateFlags(),
		1,&viewport,
		1,&scissor
	};
	
	// Specify multisample info
	vk::SampleMask sampleMask=~0u;
	vk::PipelineMultisampleStateCreateInfo multisampleInfo;
	multisampleInfo.setPSampleMask(&sampleMask);
	
	// Specify color blend state
	vk::PipelineColorBlendAttachmentState attachmentStates;
	attachmentStates.setColorWriteMask(vk::ColorComponentFlagBits::eR|vk::ColorComponentFlagBits::eB|vk::ColorComponentFlagBits::eG|vk::ColorComponentFlagBits::eA);
	vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
	colorBlendInfo.setLogicOp(vk::LogicOp::eCopy).setAttachmentCount(1).setPAttachments(&attachmentStates);
	
	// Specify rasterizer info
	vk::PipelineRasterizationStateCreateInfo rasterInfo;
	rasterInfo.setLineWidth(1);
	// Specify input assembler state
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	// Specify vertex input state
	vk::VertexInputBindingDescription vertex_input_bindings{
		0,3*sizeof(float),vk::VertexInputRate::eVertex
	};
	vk::VertexInputAttributeDescription vertex_input_attributes;
	vertex_input_attributes.setFormat(vk::Format::eR32G32B32Sfloat);
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
		vk::PipelineVertexInputStateCreateFlags(),
		1,&vertex_input_bindings,
		1,&vertex_input_attributes
	};
	
	// Create the pipeline cache
	vk::PipelineCacheCreateInfo pipelineCacheInfo;
	auto pipelineCache = createPipelineCache(pipelineCacheInfo);
	
	vk::GraphicsPipelineCreateInfo pipelineCreateInfo{
		vk::PipelineCreateFlags(),
		2,pipelineShaderStageCreateInfo,
		&vertexInputInfo,
		&inputAssemblyInfo,
		nullptr,
		&viewportInfo,
		&rasterInfo,
		&multisampleInfo,
		nullptr,
		&colorBlendInfo,
		&pipelineDynamicStateCreateInfo,
		pipelineLayout,
		renderPass,
		0,
		vk::Pipeline(),
		0
	};
	auto graphicsPipeline = createGraphicsPipelines(pipelineCache,pipelineCreateInfo);
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

