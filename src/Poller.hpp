#ifndef POLLER_HPP_
# define POLLER_HPP_

# include <vector>
# include <cstdint>

using Handle = int;

struct Event {
	Handle		handle;
	uint32_t	event_flag;
};

class IPollable {
	public:
		virtual ~IPollable() = default;
		virtual Handle	handle(void) const noexcept = 0;
};

class IPoller {
	public:
		enum {
			EVENT_READABLE = 1u << 0,
			EVENT_WRITABLE = 1u << 1,
			EVENT_ERROR = 1u << 2
		};
		virtual ~IPoller() = default;
		virtual void register_target(IPollable* target, int events) = 0;
		virtual void unregister_target(IPollable* target) = 0;
		virtual void modify_target(IPollable* target, int events) = 0;
		virtual std::vector<Event>	poll(int timeout_ms) = 0;
};

#endif
