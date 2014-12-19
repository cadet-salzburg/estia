#include "TrackingBlock.h"

#include "HumanListener.h"

void Tracking::getBlockMetaInfo(_2Real::bundle::BlockMetainfo &info,
	_2Real::bundle::TypeMetainfoCollection const &types)
{
	_2Real::bundle::BlockMetainfo &trackingInfo =
		dynamic_cast<_2Real::bundle::BlockMetainfo&>(info);

	auto humanInfo = std::static_pointer_cast<const _2Real::bundle::CustomTypeMetainfo>
		(types.getTypeMetainfo("human"));

	trackingInfo.setBlockClass<Tracking>();
	trackingInfo.setDescription("tracking");

	_2Real::bundle::UpdatePolicyMetainfo policy = trackingInfo.getUpdatePolicyMetainfo();
	policy.set(_2Real::DefaultUpdatePolicy::ANY);

	_2Real::bundle::OutletMetainfo out = trackingInfo.getOutletMetainfo("humans");
	out.setDescription("humans coming out");
}

Tracking::Tracking(_2Real::bundle::BlockIo const& io,
		std::vector<std::shared_ptr<_2Real::bundle::Block>> const& dependencies) :
		_2Real::bundle::Block(io, dependencies)
{
}


Tracking::~Tracking()
{
}

void Tracking::setup()
{
	auto configParam = mIo.mParameters[0];
	auto config = boost::get<_2Real::CustomDataItem>(configParam->getValue());
	auto port = config.getValue<uint32_t>("oscPort");
	IpEndpointName localEndpoint(IpEndpointName::ANY_ADDRESS, port);

	m_oscListener = std::shared_ptr<HumanListener>(new HumanListener());
	m_udpSocket = std::shared_ptr<UdpListeningReceiveSocket>(
		new UdpListeningReceiveSocket(localEndpoint, m_oscListener.get()));
}

void Tracking::update()
{
	if (m_oscListener->hasNewData())
	{
		HumanListener::Data data = m_oscListener->nextData();
		
		auto outItem = boost::get<_2Real::CustomDataItem>(mIo.mOutlets[0]->getValue());

		auto gidOut = outItem.getValue<_2Real::CustomDataItem>("id");
		auto idOut = gidOut.getValue<_2Real::CustomDataItem>("id");
		idOut.getValue<uint64_t>("unique_id") = data.id;
		gidOut.getValue<std::string>("identificator") = "k4w2streamer01";
		
		if (m_frameNumbers.find(data.id) == m_frameNumbers.end())
		{
			m_frameNumbers[data.id] = 0;
		}

		m_frameNumbers[data.id]++;
		
		outItem.getValue<uint64_t>("frame") = m_frameNumbers[data.id];
		
		auto pos = outItem.getValue<_2Real::CustomDataItem>("pos");
		pos.getValue<double>("x") = data.px;
		pos.getValue<double>("y") = data.pz;
		
		outItem.getValue<double>("rot") = data.midshoulder_roll;
		outItem.getValue<double>("facerot") = data.frz;
		outItem.getValue<uint8_t>("engaged") = data.engaged;
	}
}

void Tracking::shutdown()
{

}