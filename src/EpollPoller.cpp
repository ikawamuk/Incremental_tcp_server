#include "EpollPoller.hpp"
#include <sys/epoll.h>
#include <system_error>
#include <array>

namespace {
	struct EventMapping {
		uint32_t domain;
		uint32_t epoll_flag;
	};
	constexpr std::array<EventMapping, 4> kEventMap = {{
		{IPoller::EVENT_READABLE, EPOLLIN},
		{IPoller::EVENT_WRITABLE, EPOLLOUT},
		{IPoller::EVENT_ERROR,    EPOLLERR},
		{IPoller::EVENT_ERROR,    EPOLLHUP},
	}};

	uint32_t to_epoll_events(int events) noexcept {
		uint32_t raw = static_cast<uint32_t>(events);
		uint32_t mask = 0;
		for (const auto& m : kEventMap) {
			if ((raw & static_cast<uint32_t>(m.domain)) != 0) {
				mask |= m.epoll_flag;
			}
		}
		return mask;
	}

	uint32_t from_epoll_events(uint32_t revents) noexcept {
		uint32_t out = 0;
		for (const auto& m : kEventMap) {
			if ((revents & m.epoll_flag) != 0) {
				out |= static_cast<uint32_t>(m.domain);
			}
		}
		return (out);
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
	ev.events = to_epoll_events(events);
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
	ev.events = to_epoll_events(events);
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
		poll_result.push_back(Event{events[i].data.fd, from_epoll_events(events[i].events)});
	return (poll_result);
}
