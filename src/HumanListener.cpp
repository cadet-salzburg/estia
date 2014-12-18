#include "HumanListener.h"

#include <iostream>

#include <Eigen/Core>

HumanListener::HumanListener()
{
}


HumanListener::~HumanListener()
{
}

void HumanListener::ProcessMessage(const osc::ReceivedMessage &m,
	const IpEndpointName &remoteEndpoint)
{
	(void)remoteEndpoint;
	
	try
	{
		if (std::strcmp(m.AddressPattern(), "/body") == 0)
		{
			osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
			
			Data d;
			
			args
				>> d.id
				>> d.base_roll >> d.midspline_roll >> d.midshoulder_roll >> d.neck_roll
				>> d.px >> d.py >> d.pz >> d.frx >> d.fry >> d.frz
				>> d.engaged;

			m_dataQueue.push(d);
		}
	}
	catch (osc::Exception &e)
	{
		std::cerr << "error parsing argument: " << m.AddressPattern << ": "
			<< e.what << std::endl;
	}
}

HumanListener::Data HumanListener::nextData()
{
	if (hasNewData())
	{
		Data d = m_dataQueue.front();
		m_dataQueue.pop();
		return d;
	}

	std::cerr << "WARNING: retrieving invalid data from HumanListener Data Queue!" << std::endl;
	return Data();
}