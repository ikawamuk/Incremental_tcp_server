#ifndef POLLER_HPP_
# define POLLER_HPP_

# include <vector>

using Handle = int;

class IPollable {
	public:
		virtual ~IPollable() = default;
		virtual Handle	handle(void) const noexcept = 0;
};

enum PollEvent {
	POLL_IN = 1 << 0,
	POLL_OUT = 1 << 1,
	POLL_ERROR = 1 << 2
};

struct Event {
	Handle	handle;
	int		event_flag;
};

class IPoller {
	public:
		virtual ~IPoller() = default;
		virtual void register_target(IPollable* target, int events) = 0;
		virtual void unregister_target(IPollable* target) = 0;
		virtual std::vector<Event>	poll(int timeout_ms) = 0;
};

#endif
