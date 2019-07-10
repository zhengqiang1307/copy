#ifndef EPOLLPOLLER
#define EPOLLPOLLER

#include "muduo/net/Poller.h"

#include <vector>

struct epoll_event;

namespace muduo
{
namespace net
{
	
class EPollPoller: public Poller
{
	public:
	EPollPoller(EventLoop* loop);
	~EPollPoller() override;
	
	Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
	void updateChannel(Channel* channel) override;
	void removeChannel(Channel* channel) override;
	
	private:
	static const int kInitEventListSize=16;
	static const char* operationToString(int op);
	void fillActiveChannels(int numEvents, channelList* activeChannels) const;
	void update(int operation, Channel* channel);
	typedef std::vector<struct epoll_event> EventList;
	int pollfd_;
	EventList events_;
}
	
}
}

#endif