#include "Human.h"

#include <list>

#include "Feature.h"

using namespace Eigen;

Human::Human(int id) : 
	m_id(id),
	m_initialPosIsSet(false)
{
}


Human::~Human()
{
}

void Human::setup()
{
	std::list<std::string> featureKeys = { 
		"vel_mag",
		"rot",
		"accel_mag",
		"phi",
		"distance",
		"directness",
		"facerot",
		"engaged"
	};

	for (auto key : featureKeys)
	{
		m_features[key] = std::make_shared<Feature>();
	}
}

void Human::update(const Eigen::Vector2d &pos, double rot, double facerot, int engaged)
{
	if (!m_initialPosIsSet)
	{
		m_initialPos = pos;
		m_totalDistanceTravelled = 0.0;
		m_initialPosIsSet = true;
	}

	m_features["rot"]->updateWithValue(rot);

	Vector2d velVec = pos - m_pos;
	double velMag = velVec.norm();
	m_features["vel_mag"]->updateWithValue(velMag);

	double accelMag = velMag - m_features["vel_mag"]->value();
	m_features["accel_mag"]->updateWithValue(accelMag);

	{
		double totalPosDiff = (pos - m_initialPos).norm();
		m_totalDistanceTravelled += velMag;
		double directness = totalPosDiff / m_totalDistanceTravelled;
		m_features["directness"]->updateWithValue(directness);
	}

	double distanceToCam = pos.norm();
	m_features["distance"]->updateWithValue(distanceToCam);

	double phi = std::acos(velVec.dot(pos) / (velMag * distanceToCam));
	m_features["phi"]->updateWithValue(phi);

	m_features["facerot"]->updateWithValue(facerot);
	m_features["engaged"]->updateWithValue(engaged);
	
}