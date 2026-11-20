#ifndef EPOLLPOLLER_HPP_
# define EPOLLPOLLER_HPP_

# include <vector>
# include "Poller.hpp"

class EpollPoller: public IPoller {
	public:
		EpollPoller(void);
		void register_target(IPollable* target, int events);
		void unregister_target(IPollable* target);
		void modify_target(IPollable* target, int events);
		std::vector<Event>	poll(int timeout_ms);
	private:
		int	epoll_fd_;
		static const int MAX_EVENTS = 1024;
};

#endif
