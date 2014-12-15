#include "Human.h"

#include <list>
#include <boost/fusion/algorithm/transformation/zip.hpp>
#include <boost/fusion/include/zip.hpp>

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

	m_labels.push_front(m_currentLabel);
	
}

void Human::setLabel(const std::string &label)
{
	m_currentLabel = label;
}

std::list< std::list< double > > Human::labelledStfPatterns() const
{
	// making a list with each feature's mean and std histories
	// --> list will contain [vel_mag.means, vel_mag.stds, rot.means, rot.stds, ...]
	std::list< std::deque<double> > featureParams;
	
	for (auto &featureTuple : m_features)
	{
		auto key = featureTuple.first;
		auto feature = featureTuple.second;
		featureParams.push_back(feature->stfMeans());
		featureParams.push_back(feature->stfStds());
	}

	// the following is equivalent to the following 2 lines in python:
	//   zipped = zip(*self.stf_features())
	//   zipped = [list(z) for z in zipped]
	// will result in a list of patterns, each pattern a vector of values at a point in time

	std::list< std::vector<double> > patterns;
	while (!featureParams.front().empty())
	{
		std::vector<double> pattern;

		for (auto paramList : featureParams)
		{
			if (paramList.empty())
			{
				std::cerr << "feature parameter list is empty (unexpected)!" << std::endl;
				pattern.push_back(0.0);
			}
			else
			{
				double curParam = paramList.front();
				pattern.push_back(curParam);
				paramList.pop_front();
			}
		}

		patterns.push_back(pattern);

	}
}