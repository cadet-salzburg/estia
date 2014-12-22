#include "Human.h"

#include <list>
#include <deque>

#include "Feature.h"

using namespace Eigen;

Human::Human(uint64_t id) : 
	m_id(id),
	m_initialPosIsSet(false),
	m_lifetime(.0f),
	m_timeSinceLastUpdate(.0f),
	m_mostRecentFrame(0),
	m_totalDistanceTravelled(0.0),
	m_currentLabel(0),
	m_predictionLtf(0),
	m_predictionStf(0)

///	m_pos(Eigen::Vector2d(0.0, 0.0)),
///	m_initialPos(Eigen::Vector2d(0.0, 0.0)),
///	m_velVec(Eigen::Vector2d(0.0, 0.0))
{
	setup();
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
		m_initialPos = m_pos = pos;
		m_totalDistanceTravelled = 0.0;
		m_initialPosIsSet = true;
	}

	m_features["rot"]->updateWithValue(rot);

	Vector2d velVec = pos - m_pos;
	double velMag = velVec.norm();

	double accelMag = velMag - m_features["vel_mag"]->value();

	m_features["vel_mag"]->updateWithValue(velMag);
	m_features["accel_mag"]->updateWithValue(accelMag);

	{
		double totalPosDiff = (pos - m_initialPos).norm();
		m_totalDistanceTravelled += velMag;
		if (m_totalDistanceTravelled != 0.0)
		{
			double directness = totalPosDiff / m_totalDistanceTravelled;
			if (std::isnan(directness) || std::isinf(directness))
				directness = 0.0;
			m_features["directness"]->updateWithValue(directness);
		}
		else
		{
			m_features["directness"]->updateWithValue(0.0);
		}
	}


	double distanceToCam = pos.norm();
	m_features["distance"]->updateWithValue(distanceToCam);

	double phi = std::acos(velVec.dot(pos) / (velMag * distanceToCam));
	if (std::isnan(phi) || std::isinf(phi))
		phi = 0.0;
	m_features["phi"]->updateWithValue(phi);

	m_features["facerot"]->updateWithValue(facerot);
	m_features["engaged"]->updateWithValue(engaged);

	m_labels.push_front(m_currentLabel);

	std::cout << "/human" << std::endl;
	for (auto &kv : m_features)
	{
		std::cout << kv.first << "\t" << kv.second->value() << std::endl;
	}
	
	setTimeSinceLastUpdate(.0f);

	m_pos = pos;
}

void Human::setLabel(uint8_t label)
{
	m_currentLabel = label;
}

Human::Pattern Human::currentStfPattern() const
{
	Pattern pattern;

	for (auto &featureTuple : m_features)
	{
		auto key = featureTuple.first;
		auto feature = featureTuple.second;		
		pattern.features.push_back(feature->stfMeans().back());
		pattern.features.push_back(feature->stfStds().back());
	}

	return pattern;
}

Human::Pattern Human::labelledLtfPattern(uint8_t label) const
{

	Pattern pattern;
	pattern.label = label;

	for (auto &featureTuple : m_features)
	{
		pattern.features.push_back(featureTuple.second->ltfMean());
		pattern.features.push_back(featureTuple.second->ltfStd());
	}

	return pattern;
}

std::list< Human::Pattern > Human::labelledStfPatterns() const
{
	// making a list with each feature's mean and std histories
	// --> list will contain [vel_mag.means, vel_mag.stds, rot.means, rot.stds, ...]
	std::list< std::deque<double> > featureParams;
	std::deque<int> labels = m_labels;
	
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

	std::list< Pattern > patterns;
	while (!featureParams.front().empty())
	{
		Pattern pattern;

		for (auto &paramList : featureParams)
		{
			if (paramList.empty())
			{
				std::cerr << "feature parameter list is empty (unexpected)!" << std::endl;
				pattern.features.push_back(0.0);
			}
			else
			{
				double curParam = paramList.front();
				pattern.features.push_back(curParam);
				paramList.pop_front();
			}
		}

		pattern.label = labels.back();
		labels.pop_back();

		patterns.push_back(pattern);

	}

	return patterns;
}