#define VK_USE_PLATFORM_WAYLAND_KHR
#include <algorithm>
#include "vulkan.hpp"
#include <memory>
#include <iostream>
#include <exception>
//#define VK_Validation

namespace toolkit{

	template<class T, class... Args>
		struct Wrapper {
			template<void (T::*F)(Args...)>
				static void forward(void *data, Args... args) {
					(static_cast<T *>(data)->*F)(std::forward<Args>(args)...);
				}
		};

	template<class T, class... Args>
		constexpr static auto createWrapper(void (T::*)(Args...)) -> Wrapper<T, Args...> {
			return Wrapper<T, Args...>();
		}

#define wrapInterface(method) createWrapper(method).forward<method>

	class Window{
		wl_display *m_display;
		wl_surface *m_surface;
		wl_shell_surface *m_shell_surface;
		wl_callback *m_frame_callback;
		void ping(struct wl_shell_surface *shell_surface,uint32_t serial){
			wl_shell_surface_pong(shell_surface, serial);
			std::cout<<"Pinged and ponged\n";
		}

		void configure(struct wl_shell_surface *shell_surface,
				uint32_t edges, int32_t width, int32_t height){
			std::cout<<"configure and w x h=:"<<width<<'x'<<height<<std::endl;
		}

		void popup_done(struct wl_shell_surface *shell_surface){
			std::cout<<"popup_done\n";
		}

		void frame_callback(wl_callback *, uint32_t time){
			std::cout<<"frame_callback"<<std::endl;
			wl_callback_destroy(m_frame_callback);
			m_frame_callback = nullptr;
		}
		public:
		explicit Window(wl_display *display,wl_surface *surface,wl_shell_surface *shell_surface):
			m_display{display},
			m_surface{surface},
			m_shell_surface{shell_surface},
			m_frame_callback{wl_surface_frame(m_surface)}{
				std::cout<<"window display:"<<display<<"\twindow surface:"<<surface<<"\tshell surface:"<<shell_surface<<std::endl;
				static const struct wl_shell_surface_listener shell_surface_listener = {
					wrapInterface(&Window::ping),
					wrapInterface(&Window::configure),
					wrapInterface(&Window::popup_done)
				};
				wl_shell_surface_add_listener(m_shell_surface, &shell_surface_listener,this);
				wl_surface_add_listener(m_surface,nullptr,this);
				static const wl_callback_listener frame_listener = {
					wrapInterface(&Window::frame_callback),
				};
				wl_callback_add_listener(m_frame_callback, &frame_listener, this);
			}
		Window(const Window &) = delete;
		Window(const Window &&w):
			m_display{w.m_display},
			m_surface{w.m_surface},
			m_shell_surface{w.m_shell_surface}{
				wl_surface_set_user_data(m_surface, this);
			}
		auto set_toplevel(){
			return wl_shell_surface_set_toplevel(m_shell_surface);
		}
		
		auto get(){
			return m_surface;
		}
/*
		auto create_vk_surface(const VulkanInstance &instance){
			VkSurfaceKHR surface = 0;
			VkWaylandSurfaceCreateInfoKHR info = {
				VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR, //type
				nullptr, //next
				0, //flags
				m_display, //display
				m_surface, //surface
			};
			if(vkCreateWaylandSurfaceKHR(instance.get_handle(), &info, nullptr, &surface)!= VK_SUCCESS)
				throw vk_exception("Failed to create_vk_surface\n");
			return surface;
		}
*/
		
	};

	class Display{
		struct Deleter{void operator()(wl_display* d) { wl_display_disconnect(d);}};
		std::unique_ptr<wl_display,Deleter> display{wl_display_connect(NULL)};
		wl_compositor *m_compositor;
		wl_shell *m_shell;
		wl_seat *m_seat;
		protected:
		auto dispatch(){
			return wl_display_dispatch(&*display);
		}
		auto roundtrip(){
			return wl_display_roundtrip(&*display);
		}
		auto get_registry(){
			return wl_display_get_registry(&*display);
		}

		void global(wl_registry *reg, uint32_t id, const char *interface, uint32_t version)
		{
#define registry_bind(reg, type, ver) \
			reinterpret_cast<type *>(wl_registry_bind(reg, id, &type##_interface, std::min(version, (uint32_t)ver)));

			auto iface = std::string(interface);
			if (iface == "wl_compositor") {
				m_compositor = registry_bind(reg, wl_compositor, version);
			} else if (iface == "wl_shell") {
				m_shell = registry_bind(reg, wl_shell, version);
			} else if (iface == "wl_seat") {
				m_seat = registry_bind(reg, wl_seat, version);
				//new seat(s);
			}
		}

		void global_remove(wl_registry *reg, uint32_t id)
		{
		}
		public:
		Display(const Display &) = delete;

		Display(){
			if(display==nullptr)
				throw std::runtime_error{"cannot connect wayland server"};
			wl_registry_listener registry_listener = {
				wrapInterface(&Display::global),
				wrapInterface(&Display::global_remove)
			};
			wl_registry_add_listener(get_registry(),&registry_listener,this);
			dispatch();
			roundtrip();
		}

		auto CreateWindow(){
			auto surface = wl_compositor_create_surface(m_compositor);
			return Window{&*display,surface,wl_shell_get_shell_surface(m_shell,surface)};
		}

		auto run(){
			while( dispatch()!=-1)
				;
		}
		
		auto get(){
			return display.get();
		}

	};

}//namespace vkwl

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

