#pragma once
#include <algorithm>
#include <memory>
#include <iostream>
#include <exception>
#include <wayland-client.h>
//#define VK_Validation

namespace toolkit{

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
		Window(wl_display *display,wl_surface *surface,wl_shell_surface *shell_surface);
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
			return wl_display_dispatch(display.get());
		}
		auto dispatch_pending(){
		wl_display_flush(display.get());
			return wl_display_dispatch_pending(display.get());
		}

		auto roundtrip(){
			return wl_display_roundtrip(display.get());
		}
		auto get_registry(){
			return wl_display_get_registry(display.get());
		}
		auto read_events()
		{
			if (wl_display_prepare_read(display.get()) != -1) {
				wl_display_read_events(display.get());
			}
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


		Display();
		Display(const Display &) = delete;
		auto CreateWindow(){
			auto surface = wl_compositor_create_surface(m_compositor);
			return Window{display.get(),surface,wl_shell_get_shell_surface(m_shell,surface)};
		}

		auto run(){
			do{
				dispatch_pending();
				read_events();
			}while(true);
		}
		
		auto get(){
			return display.get();
		}

	};

}//namespace vkwl


