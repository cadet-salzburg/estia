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

	info.exportsType("vec3", {
		_2Real::declareField("x", "double"),
		_2Real::declareField("y", "double"),
		_2Real::declareField("z", "double")
	});

	info.exportsType("human", {
		_2Real::declareField("id", "int"),
		_2Real::declareField("pos", "vec3"),
		_2Real::declareField("facerot", "double"),
		_2Real::declareField("engaged", "int")
	});

	info.exportsType("attentive", {
		_2Real::declareField("id", "int"),
		_2Real::declareField("pos", "vec3"),
		_2Real::declareField("attentionStf", "int"),
		_2Real::declareField("attentionLtf", "int")
	});

	info.exportsType("humansList", {
		_2Real::declareField("humans", "list of humans")
	});

	info.exportsType("attentivesList", {
		_2Real::declareField("attentives", "list of attentives")
	});

	info.exportsBlock("attentionEstimator", 
	{ _2Real::declareInlet("humans") },
	{ _2Real::declareOutlet("attentives") },
	{ _2Real::declareParameter("init") }
	);
}