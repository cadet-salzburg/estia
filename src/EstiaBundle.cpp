

#include "_2RealBundle.h"
#include <Eigen/Core>

#include "ModellingBlock.h"
#include "TrackingBlock.h"

void getBundleMetainfo(_2Real::bundle::BundleMetainfo &info)
{
	info.setAuthor("Peter Holzkorn");
	info.setDescription("Frobnicates the Foobaz");
	info.setCategory("machine learning");
	info.setContact("peter.holzkorn@aec.at");
	info.setVersion(0, 1, 0);

	_2Real::HumanReadableNameVisitor name;

	info.exportsType("vec2", {
		_2Real::declareField("x", "double"),
		_2Real::declareField("y", "double")
	});

	info.exportsType("human", {
		_2Real::declareField("frame", "ulong"),
		_2Real::declareField("id", "uint"),
		_2Real::declareField("pos", "vec2"),
		_2Real::declareField("rot", "double"),
		_2Real::declareField("facerot", "double"),
		_2Real::declareField("engaged", "uchar")
	});

	info.exportsType("attentive", {
		_2Real::declareField("id", "uint"),
		_2Real::declareField("pos", "vec2"),
		_2Real::declareField("attentionStf", "uchar"),
		_2Real::declareField("attentionLtf", "uchar")
	});

	info.exportsType("humanLabel", {
		_2Real::declareField("id", "uint"),
		_2Real::declareField("labelStf", "uchar")
	});

	info.exportsType("modellingConfig", {
		_2Real::declareField("mode", "uchar"),
		_2Real::declareField("datafile", "string")
	});

	info.exportsType("trackingConfig", {
		_2Real::declareField("oscPort", "uint")
	});

	info.exportsBlock("attentionModelling", 
		{
			_2Real::declareInlet("humans"),
			_2Real::declareInlet("labels") 
		},
		{ 
			_2Real::declareOutlet("attentives") 
		},
		{ 
			_2Real::declareParameter("config") 
		}
	);

	info.exportsBlock("attentionTracking",
		{ },
		{ 
			_2Real::declareOutlet("humans") 
		},
		{ 
			_2Real::declareParameter("config") 
		}
	);
}

void getTypeMetainfo(_2Real::bundle::CustomTypeMetainfo &info,
	_2Real::bundle::TypeMetainfoCollection const& existingTypes)
{
	if (info.getName() == "vec2")
	{
		info.setDescription("vec2, double");
		info.setInitialFieldValue("x", 0.0);
		info.setInitialFieldValue("y", 0.0);
	}
	else if (info.getName() == "human")
	{
		auto vec2Info = std::static_pointer_cast<const _2Real::bundle::CustomTypeMetainfo>
			(existingTypes.getTypeMetainfo("vec2"));

		info.setInitialFieldValue("frame", (uint64_t) 0);
		info.setInitialFieldValue("id", (uint32_t) 0);
		info.setInitialFieldValue("pos", vec2Info->makeData());
		info.setInitialFieldValue("rot", 0.0);
		info.setInitialFieldValue("facerot", 0.0);
		info.setInitialFieldValue("engaged", (uint8_t) 0);
	}
	else if (info.getName() == "attentive")
	{
		auto vec2Info = std::static_pointer_cast<const _2Real::bundle::CustomTypeMetainfo>
			(existingTypes.getTypeMetainfo("vec2"));

		info.setInitialFieldValue("id", (uint32_t)0);
		info.setInitialFieldValue("pos", vec2Info->makeData());
		info.setInitialFieldValue("attentionStf", (uint8_t)0);
		info.setInitialFieldValue("attentionLtf", (uint8_t)0);
	}
	else if (info.getName() == "humanLabel")
	{
		info.setInitialFieldValue("id", (uint32_t)0);
		info.setInitialFieldValue("labelStf", (uint8_t)0);
	}
	else if (info.getName() == "modellingConfig")
	{
		info.setInitialFieldValue("mode", (uint8_t)0);
		info.setInitialFieldValue("datafile", std::string("none"));
	}
	else if (info.getName() == "trackingConfig")
	{
		info.setInitialFieldValue("oscPort", (uint32_t) 57120);
	}

}

void getBlockMetainfo(_2Real::bundle::BlockMetainfo &info,
	_2Real::bundle::TypeMetainfoCollection const& types)
{
	if (info.getName() == "attentionModelling")
	{
		Modelling::getBlockMetaInfo(info, types);
	}
	else if (info.getName() == "attentionTracking")
	{
		Tracking::getBlockMetaInfo(info, types);
	}

}