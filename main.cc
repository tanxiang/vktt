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
	auto formats = physicalDevice.getSurfaceFormatsKHR(surface);
	for(auto& format:formats){
		std::cout<<vk::to_string(format.format)<<':'<<vk::to_string(format.colorSpace)<<std::endl;

		float queue_priorities[1] = { 0.0 };
		vk::DeviceQueueCreateInfo dc_q_info;
		dc_q_info.setQueueFamilyIndex(1).setQueueCount(1).setPQueuePriorities(queue_priorities);
		std::array<const char *,1> extname{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		vk::DeviceCreateInfo dc_info;
		dc_info.setQueueCreateInfoCount(1).setPQueueCreateInfos(&dc_q_info).setEnabledExtensionCount(extname.size()).setPpEnabledExtensionNames(extname.data());
		vk::SwapchainCreateInfoKHR scc_info;
		auto dev = physicalDevice.createDevice(dc_info);
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
	
	//auto formats = instance.get_format(surface);

	//for(auto& format:formats){
	//	std::cout<<"format get"<<format.format<<std::endl;
	//}
	
	window.set_toplevel();
	display.run();
	exit(0);
}
