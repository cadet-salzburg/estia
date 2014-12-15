#pragma once

#include <Eigen/Core>
#include <map>
#include <memory>
#include <list>
#include <deque>
#include <vector>

#include <boost/variant.hpp>

class Feature;

class Human
{
public:
	struct Pattern
	{
		int label;
		std::vector<double> features;
	};

	Human(int id);
	~Human();

	void setup();

	/// engaged can be [0, 1, 2]
	void update(const Eigen::Vector2d &pos, double rot, double facerot, int engaged);
	void setLabel(int label);

	std::list< Pattern > labelledStfPatterns() const;

private:
	int m_id;
	Eigen::Vector2d m_pos;
	Eigen::Vector2d m_initialPos;
	Eigen::Vector2d m_velVec;

	/// total distance travelled
	double m_totalDistanceTravelled;
	
	bool m_initialPosIsSet;

	std::map<std::string, std::shared_ptr<Feature> > m_features;
	std::deque<int> m_labels;
	int m_currentLabel;

};

