#ifndef EPOLLPOLLER_HPP_
# define EPOLLPOLLER_HPP_

# include <vector>
# include "Poller.hpp"

class EpollPoller: public IPoller {
	public:
		EpollPoller(void);
		~EpollPoller() override;
		void register_target(IPollable* target, int events) override;
		void unregister_target(IPollable* target) override;
		void modify_target(IPollable* target, int events) override;
		std::vector<Event>	poll(int timeout_ms) override;
		EpollPoller(const EpollPoller&) = delete;
		EpollPoller& operator=(const EpollPoller&) = delete;
	private:
		int	epoll_fd_;
		static const int MAX_EVENTS = 1024;
};

#endif
