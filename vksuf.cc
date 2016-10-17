#include "waylandsuf.hh"

int main(int argc, char **argv) {

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
		
	vk::ApplicationInfo appinfo{
		"vktest",
		VK_MAKE_VERSION(0, 1, 5),
		"Vulkan EG",
		VK_MAKE_VERSION(0, 1, 5),
		VK_MAKE_VERSION(1, 0, 5)
	};
	vk::InstanceCreateInfo instace_createinfo{
		vk::InstanceCreateFlags(),
		&appinfo,
		0,
		nullptr,
		sizeof(extension_name)/sizeof(const char *),
		extension_name
	};
	auto instance=vk::createInstance(instace_createinfo);
	toolkit::Display display;

	auto window{display.CreateWindow()};
	vk::WaylandSurfaceCreateInfoKHR surface_createinfo{
		vk::WaylandSurfaceCreateFlagsKHR{},
		display.get(),
		window.get()
	};
	auto surface = instance.createWaylandSurfaceKHR(surface_createinfo);
	std::cout<<"vk surface get:"<<surface<<std::endl;
	auto pdevs = instance.enumeratePhysicalDevices();
	for(auto& pdev:pdevs){
		auto devps = pdev.getProperties();
		std::cout<<"name:"<<devps.deviceName<<"\n\ttype:"<<vk::to_string(devps.deviceType)<<std::endl;	
		auto devqfps = pdev.getQueueFamilyProperties();
		int32_t i = 0;
		for (; i < devqfps.size(); ++i) {
			std::cout<<vk::to_string(devqfps[i].queueFlags)<<std::endl;
			if(devqfps[i].queueFlags & vk::QueueFlagBits::eGraphics){
				if(pdev.getSurfaceSupportKHR(i,surface)){
					std::cout<<"getSurfaceSupportKHR true"<<std::endl;
					break;
				}
			}
		}
		auto formats = pdev.getSurfaceFormatsKHR(surface);
		for(auto& format:formats){
			std::cout<<vk::to_string(format.format)<<':'<<vk::to_string(format.colorSpace)<<std::endl;

			float queue_priorities[1] = { 0.0 };
			vk::DeviceQueueCreateInfo dc_q_info;
			dc_q_info.setQueueFamilyIndex(i).setQueueCount(1).setPQueuePriorities(queue_priorities);
			std::array<const char *,1> extname{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
			vk::DeviceCreateInfo dc_info;
			dc_info.setQueueCreateInfoCount(1).setPQueueCreateInfos(&dc_q_info).setEnabledExtensionCount(extname.size()).setPpEnabledExtensionNames(extname.data());
			vk::SwapchainCreateInfoKHR scc_info;
			auto dev = pdev.createDevice(dc_info);
			vk::SwapchainCreateInfoKHR sc_info;
			sc_info.setSurface(surface).setMinImageCount(1).setImageFormat(format.format).setImageColorSpace(format.colorSpace)
				.setImageExtent(vk::Extent2D{1024,768}).setImageArrayLayers(1).setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
			auto swapChain = dev.createSwapchainKHR(sc_info);
			vk::AttachmentDescription att_des;
			vk::AttachmentReference att_ref;
			vk::SubpassDescription subpass_des;
			vk::RenderPassCreateInfo renderpass_info;
			  // -----------------------------------------------------------------
  /* Create render pass
  VkAttachmentDescription attachmentDescriptions{
      .format = swapchain.displayFormat_,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
      .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
      .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
      .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
      .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
  };

  VkAttachmentReference colourReference = {
      .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

  VkSubpassDescription subpassDescription {
      .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
      .flags = 0,
      .inputAttachmentCount = 0,
      .pInputAttachments = nullptr,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colourReference,
      .pResolveAttachments = nullptr,
      .pDepthStencilAttachment = nullptr,
      .preserveAttachmentCount = 0,
      .pPreserveAttachments = nullptr,
  };
  VkRenderPassCreateInfo renderPassCreateInfo {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
      .pNext = nullptr,
      .attachmentCount = 1,
      .pAttachments = &attachmentDescriptions,
      .subpassCount = 1,
      .pSubpasses = &subpassDescription,
      .dependencyCount = 0,
      .pDependencies = nullptr,
  };*/
			auto renderPass = dev.createRenderPass(renderpass_info);
			
			auto swapchainImages = dev.getSwapchainImagesKHR(swapChain);
			vk::ImageViewCreateInfo iv_create_info;
			auto ImageView = dev.createImageView(iv_create_info);
			vk::FramebufferCreateInfo fbCreateInfo;
			auto fb = dev.createFramebuffer(fbCreateInfo);
			
		}

	}
	

	
	//auto formats = instance.get_format(surface);

	//for(auto& format:formats){
	//	std::cout<<"format get"<<format.format<<std::endl;
	//}
	
	window.set_toplevel();
	display.run();
	exit(0);
}
