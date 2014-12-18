#pragma once

#include "osc/OscPacketListener.h"
#include "osc/OscReceivedElements.h"
#include "ip/UdpSocket.h"

#include <queue>

class HumanListener :
	public osc::OscPacketListener
{
public:
	struct Data
	{
		osc::int32 id;
		float base_roll, midspline_roll, midshoulder_roll, neck_roll;
		float px, py, pz, frx, fry, frz;
		osc::int32 engaged;
	};

	bool hasNewData() { return m_dataQueue.size() > 0; }
	Data nextData();

protected:
	virtual void ProcessMessage(const osc::ReceivedMessage &m,
		const IpEndpointName &remoteEndpoint);
	
private:
	std::queue<Data> m_dataQueue;
};

