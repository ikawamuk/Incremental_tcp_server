#include "EpollPoller.hpp"
#include <sys/epoll.h>
#include <system_error>

namespace {
	int	get_epoll_event_flag(int poll_event_flag) {
		
	}

	int	get_poller_event_flag(int epoll_event_flag) {
		
	}
}


EpollPoller::EpollPoller(void)
{
	epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd_ < 0) throw std::system_error(errno, std::generic_category(), "epoll_create1");
}

void EpollPoller::register_target(IPollable* target, int events)
{
	struct epoll_event	ev;
	ev.events = get_epoll_event_flag(events);
	ev.data.fd = target->handle();
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, target->handle(), &ev) < 0)
		throw std::system_error(errno, std::generic_category(), "epoll_ctl");
}

void EpollPoller::unregister_target(IPollable* target)
{
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, target->handle(), NULL) < 0)
		throw std::system_error(errno, std::generic_category(), "epoll_ctl");
}

void EpollPoller::modify_target(IPollable* target, int events)
{
	struct epoll_event	ev;
	ev.events = get_epoll_event_flag(events);
	ev.data.fd = target->handle();
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, target->handle(), &ev) < 0)
		throw std::system_error(errno, std::generic_category(), "epoll_ctl");
}

std::vector<Event>	EpollPoller::poll(int timeout_ms)
{
	std::vector<Event>	poll_result;
	struct epoll_event	events[MAX_EVENTS];

	int	nfds = epoll_wait(epoll_fd_, events, MAX_EVENTS, timeout_ms);
	if (nfds == -1) {
		if (errno == EINTR)
			return (std::vector<Event>());
		throw std::system_error(errno, std::generic_category(), "epoll_wait");
	}
	for (int i = 0; i < nfds; ++i)
		poll_result.push_back(Event(events[i].data.fd, get_poller_event_flag(events[i].events)));
	return (poll_result);
}
