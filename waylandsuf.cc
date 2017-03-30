#include "waylandsuf.hh"

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

Window::Window(wl_display *display,wl_surface *surface,wl_shell_surface *shell_surface):
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

Display::Display(){
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

}//namespace
