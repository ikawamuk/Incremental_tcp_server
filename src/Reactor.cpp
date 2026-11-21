#include "Reactor.hpp"

Reactor::Reactor(std::unique_ptr<IPoller> poller)
:poller_(std::move(poller)) {
	if (!poller_) throw std::invalid_argument("Poller is null");
}

void	Reactor::run(const ShouldStop& should_stop) {
	while (!should_stop()) {
		try {
			std::vector<Event>	events = poller_->poll(1000);
			for (std::vector<Event>::iterator ev = events.begin(); ev != events.end(); ++ev)
				handle_event(*ev);
		} catch (const std::system_error& e) {
			if (e.code() == std::errc::interrupted) continue ;
			throw ;
		}
	}
}

void	Reactor::unregister_handler(IPollable* target) {
	poller_->unregister_target(target);
	event_handlers_.erase(target->handle());
}

void Reactor::register_handler(IPollable* target, int interests, EventHandler handler) {
	auto [it, inserted] = event_handlers_.try_emplace(
		target->handle(), std::move(handler));
	if (!inserted)
		throw std::logic_error("Duplicate event handler");
	try {
		poller_->register_target(target, interests);
	} catch (...) {
		event_handlers_.erase(it);
		throw;
	}
}

void	Reactor::handle_event(Event& event) {
	const auto it = event_handlers_.find(event.handle);
	if (it == event_handlers_.end()) return ;
	auto handler = it->second;
	handler(event);
}

void	Reactor::modify_handler(IPollable* target, int interests) {
	poller_->modify_target(target, interests);
}
