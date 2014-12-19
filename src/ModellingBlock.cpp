#include "ModellingBlock.h"

#include <Eigen/Core>

#include "Modeller.h"
#include "Human.h"

void Modelling::getBlockMetaInfo(_2Real::bundle::BlockMetainfo &info,
	_2Real::bundle::TypeMetainfoCollection const &types)
{
	_2Real::bundle::BlockMetainfo &modellingInfo =
		dynamic_cast<_2Real::bundle::BlockMetainfo&>(info);

	auto humanInfo = std::static_pointer_cast<const _2Real::bundle::CustomTypeMetainfo>
		(types.getTypeMetainfo("human"));
	auto attentiveInfo = std::static_pointer_cast<const _2Real::bundle::CustomTypeMetainfo>
		(types.getTypeMetainfo("attentive"));

	auto configInfo = std::static_pointer_cast<const _2Real::bundle::CustomTypeMetainfo>
		(types.getTypeMetainfo("modellingConfig"));

	modellingInfo.setBlockClass<Modelling>();
	modellingInfo.setDescription("modelling");

	_2Real::bundle::UpdatePolicyMetainfo policy = modellingInfo.getUpdatePolicyMetainfo();
	policy.set(_2Real::DefaultUpdatePolicy::ANY);

	_2Real::bundle::InletMetainfo in = modellingInfo.getInletMetainfo("humans");
	in.setDescription("humans coming in");
	in.setDatatypeAndInitialValue(humanInfo->makeData());

	_2Real::bundle::OutletMetainfo out = modellingInfo.getOutletMetainfo("attentives");
	out.setDescription("attentives coming out");
	out.setDatatypeAndInitialValue(attentiveInfo->makeData());

	_2Real::bundle::ParameterMetainfo param = modellingInfo.getParameterMetainfo("config");
	param.setDatatypeAndInitialValue(configInfo->makeData());
}

Modelling::Modelling(_2Real::bundle::BlockIo const& io,
		std::vector<std::shared_ptr<_2Real::bundle::Block>> const& dependencies) :
		_2Real::bundle::Block(io, dependencies)
{
}

Modelling::~Modelling()
{
}

void Modelling::setup()
{
	auto configParam = mIo.mParameters[0];
	auto config = boost::get<_2Real::CustomDataItem>(configParam->getValue());
	auto mode = static_cast<Modeller::ApplicationMode>(config.getValue<uint8_t>("mode"));
	auto datafile = config.getValue<std::string>("datafile");

	m_modeller = std::make_shared<Modeller>(mode);
	if (mode == Modeller::ApplicationMode::PREDICT)
	{
		m_modeller->setFilenameBase(datafile);
	}

}

void Modelling::update()
{
	std::cout << "MODELLING UPD " << std::endl;

	auto inlet = std::dynamic_pointer_cast<_2Real::bundle::InletHandle>(mIo.mInlets[0]);
	auto labelsInlet = std::dynamic_pointer_cast<_2Real::bundle::InletHandle>(mIo.mInlets[1]);
	auto outlet = mIo.mOutlets[0];

	const _2Real::CustomDataItem &val_in = boost::get<_2Real::CustomDataItem>(inlet->getValue());
	const _2Real::CustomDataItem &label_in = 
		boost::get<_2Real::CustomDataItem>(labelsInlet->getValue());

	Human::HumanFrame humanFrame;

	humanFrame.frame = val_in.getValue<uint64_t>("frame");

	auto gid = val_in.getValue<_2Real::CustomDataItem>("id");
	auto unique_id = gid.getValue<_2Real::CustomDataItem>("id").getValue<uint64_t>("unique_id");
	humanFrame.id = unique_id;
	auto pos = val_in.getValue<_2Real::CustomDataItem>("pos");
	humanFrame.pos = Eigen::Vector2d(
		pos.getValue<double>("x"),
		pos.getValue<double>("y"));
	humanFrame.rot = val_in.getValue<double>("rot");
	humanFrame.facerot = val_in.getValue<double>("facerot");
	humanFrame.engaged = val_in.getValue<uint8_t>("engaged");

	Modeller::Predictions predictions = m_modeller->updateWithFrame(humanFrame);

	_2Real::CustomDataItem &val_out = boost::get<_2Real::CustomDataItem>(outlet->getValue());
	val_out.getValue<_2Real::CustomDataItem>("id") = gid;
	val_out.getValue<_2Real::CustomDataItem>("pos") = pos;
	val_out.getValue<uint8_t>("attentionStf") = static_cast<uint8_t>(predictions.stf);
	val_out.getValue<uint8_t>("attentionLtf") = static_cast<uint8_t>(predictions.ltf);


	if (m_modeller->applicationMode() == Modeller::ApplicationMode::COLLECT)
	{
		auto id = label_in
			.getValue<_2Real::CustomDataItem>("id")
			.getValue<_2Real::CustomDataItem>("id")
			.getValue<uint64_t>("unique_id");
		auto label = label_in.getValue<uint8_t>("labelStf");

		m_modeller->setLabelStf(id, label);
	}
}

void Modelling::shutdown()
{

}