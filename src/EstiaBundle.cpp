// PythiaBundle.cpp : Defines the exported functions for the DLL application.
//

#include "_2RealBundle.h"
#include <Eigen/Core>

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

	info.exportsType("modellingConfig", {
		_2Real::declareField("mode", "uchar"),
		_2Real::declareField("datafile", "string")
	});

	info.exportsBlock("attentionEstimator", 
	{ _2Real::declareInlet("humans") },
	{ _2Real::declareOutlet("attentives") },
	{ _2Real::declareParameter("config") }
	);
}