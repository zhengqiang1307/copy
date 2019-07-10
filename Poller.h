#ifndef POLLER
#define POLLER

#include <map>
#include <vector>

namespace muduo
{
namespace net
{
	class channel;
	
class Poller:noncapyable
{
	public:
	typedef std::vector<Channel*> ChannelList;
	
	Poller(EventLoop* loop);
	virtual ~Poller();
	
	virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels)=0;
	
	virtual void updateChannel(Channel* channel)=0;
	
	virtual void removeChannel(Channel* channel)=0;
	
	virtual bool hasChannel(Channel* channel) const ;
	
	static Poller* newDefaultPoller(EventLoop* loop);
	
	void assertInLoopThread() const {
		ownerLoop_->assertInLoopThread();
	}
	
	protected:
	typedef std::map<int, Channel*> ChannelMap;
	ChannelMap channels_;
	private:
	EventLoop* ownerLoop_;
}
}
}


#endif