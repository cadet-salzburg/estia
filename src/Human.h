#pragma once

#include <Eigen/Core>
#include <map>
#include <memory>
#include <list>
#include <deque>
#include <vector>

#include <boost/variant.hpp>

#define DISAPPEAR_AFTER 2.0f

class Feature;

class Human
{
public:

	struct Pattern
	{
		int label;
		std::vector<double> features;
	};

	struct HumanFrame
	{
		uint64_t frame;
		uint64_t id;
		Eigen::Vector2d pos;
		double rot;
		double facerot;
		uint8_t engaged;
	};

	Human(uint64_t id);
	~Human();

	void setup();

	/// engaged can be [0, 1, 2]
	void update(const Eigen::Vector2d &pos, double rot, double facerot, int engaged);
	void setLabel(uint8_t label);

	std::list< Pattern > labelledStfPatterns() const;
	Pattern currentStfPattern() const;

	void setLifetime(float f) { m_lifetime = f; }
	float lifetime() const { return m_lifetime; }
	void setTimeSinceLastUpdate(float f) { m_timeSinceLastUpdate = f; }
	float timeSinceLastUpdate() const { return m_timeSinceLastUpdate; }
	uint64_t id() const { return m_id; }

	uint8_t predictionStf() const { return m_predictionStf; }
	uint8_t predictionLtf() const { return m_predictionLtf; }

	void setPredictionStf(uint8_t i) { m_predictionStf = i; }
	void setPredictionLtf(uint8_t i) { m_predictionLtf = i; }

	uint64_t mostRecentFrame() const { return m_mostRecentFrame; }
	void setMostRecentFrame(uint64_t f) { m_mostRecentFrame = f; }

private:
	uint64_t m_id;
	uint64_t m_mostRecentFrame;
	Eigen::Vector2d m_pos;
	Eigen::Vector2d m_initialPos;
	Eigen::Vector2d m_velVec;
	float m_lifetime;
	float m_timeSinceLastUpdate;

	/// total distance travelled
	double m_totalDistanceTravelled;
	
	bool m_initialPosIsSet;

	std::map<std::string, std::shared_ptr<Feature> > m_features;
	std::deque<int> m_labels;
	uint8_t m_currentLabel;

	uint8_t m_predictionStf, m_predictionLtf;
};

