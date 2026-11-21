#ifndef REACTOR_HPP_
# define REACTOR_HPP_

# include <functional>
# include <map>
# include "Transport.hpp"
# include "Poller.hpp"

using EventHandler = std::function<void(Event&)>;
using ShouldStop = std::function<bool(void)>;

class Reactor {
	public:
		explicit Reactor(std::unique_ptr<IPoller> poller);
		void	run(const ShouldStop& should_stop);
		void	register_handler(IPollable* target, int interests, EventHandler handler);
		void	unregister_handler(IPollable* target);
		void	modify_handler(IPollable* target, int interests);
	private:
		void	handle_event(Event& event);
		std::unique_ptr<IPoller>		poller_;
		std::map<Handle, EventHandler>	event_handlers_;
};


#endif
