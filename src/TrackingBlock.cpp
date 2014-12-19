#include "TrackingBlock.h"

#include "HumanListener.h"

void Tracking::getBlockMetaInfo(_2Real::bundle::BlockMetainfo &info,
	_2Real::bundle::TypeMetainfoCollection const &types)
{
	_2Real::bundle::BlockMetainfo &trackingInfo =
		dynamic_cast<_2Real::bundle::BlockMetainfo&>(info);

	auto humanInfo = std::static_pointer_cast<const _2Real::bundle::CustomTypeMetainfo>
		(types.getTypeMetainfo("human"));

	auto configInfo = std::static_pointer_cast<const _2Real::bundle::CustomTypeMetainfo>
		(types.getTypeMetainfo("trackingConfig"));

	trackingInfo.setBlockClass<Tracking>();
	trackingInfo.setDescription("tracking");

	_2Real::bundle::UpdatePolicyMetainfo policy = trackingInfo.getUpdatePolicyMetainfo();
	policy.set(_2Real::DefaultUpdatePolicy::ANY);

	_2Real::bundle::OutletMetainfo out = trackingInfo.getOutletMetainfo("humans");
	out.setDescription("humans coming out");
	out.setDatatypeAndInitialValue(humanInfo->makeData());

	_2Real::bundle::ParameterMetainfo config = trackingInfo.getParameterMetainfo("config");
	config.setDatatypeAndInitialValue(configInfo->makeData());
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
	std::thread t(&UdpListeningReceiveSocket::Run, m_udpSocket);
	t.detach();


	std::cout << "tracking set up" << std::endl;
}

void Tracking::update()
{
//	std::cout << "TRACKING UPD " << std::endl;

	if (m_oscListener->hasNewData())
	{
		std::cout << "NEW DATA " << std::endl;
		HumanListener::Data data = m_oscListener->nextData();

		std::cout << "ID " << data.id << std::endl;
		
		_2Real::DataItem &outlval = mIo.mOutlets[0]->getValue();
		_2Real::CustomDataItem &outItem = boost::get<_2Real::CustomDataItem>(outlval);

		auto gidOut = outItem.getValue<_2Real::CustomDataItem>("id");

		auto idOut = gidOut.getValue<_2Real::CustomDataItem>("id");
		idOut.getValue<uint64_t>("unique_id") = data.id;
		gidOut.getValue<std::string>("identificator") = "k4w2streamer01";
		gidOut.getValue<_2Real::CustomDataItem>("id") = idOut;

		if (m_frameNumbers.find(data.id) == m_frameNumbers.end())
		{
			m_frameNumbers[data.id] = 0;
		}

		m_frameNumbers[data.id]++;
		
		outItem.getValue<uint64_t>("frame") = m_frameNumbers[data.id];
		
		auto pos = outItem.getValue<_2Real::CustomDataItem>("pos");
		pos.getValue<double>("x") = data.px;
		pos.getValue<double>("y") = data.pz;

		outItem.getValue<_2Real::CustomDataItem>("pos") = pos;
		outItem.getValue<_2Real::CustomDataItem>("id") = gidOut;

		outItem.getValue<double>("rot") = data.midshoulder_roll;
		outItem.getValue<double>("facerot") = data.frz;
		outItem.getValue<uint8_t>("engaged") = data.engaged;

		std::cout << "gid " << gidOut << std::endl;
		std::cout << "id " << idOut << std::endl;


		std::cout << "tack/out:" << std::endl;
		std::cout << outItem << std::endl;

		
		//{
		//	_2Real::DataItem &outlval_ = mIo.mOutlets[0]->getValue();
		//	_2Real::CustomDataItem &outItem_ = boost::get<_2Real::CustomDataItem>(outlval_);
		//	std::cout << "OUT REAL " << outItem_ << std::endl;
		//}


		//std::cout << "track/out: " << pos.getValue<double>("x") << " / " << pos.getValue<double>("x")
		//	<< std::endl;
	}
	else
	{
		mIo.mOutlets[0]->discard();
	}
}

void Tracking::shutdown()
{

}