#pragma once

#include "_2RealBundle.h"

#include <memory>

class HumanListener;
class UdpListeningReceiveSocket;

class Tracking : public _2Real::bundle::Block
{
public:
	static void getBlockMetaInfo(_2Real::bundle::BlockMetainfo &,
		_2Real::bundle::TypeMetainfoCollection const&);

	Tracking(_2Real::bundle::BlockIo const&,
		std::vector < std::shared_ptr < _2Real::bundle::Block>> const&);
	~Tracking();

	void setup();
	void update();
	void shutdown();

private:

	

	std::shared_ptr<HumanListener> m_oscListener;
	std::shared_ptr<UdpListeningReceiveSocket> m_udpSocket;
};

