#pragma once

#include <Eigen/Core>
#include <map>
#include <memory>
#include <list>

#include <boost/variant.hpp>

class Feature;

class Human
{
public:
	Human(int id);
	~Human();

	void setup();

	/// engaged can be [0, 1, 2]
	void update(const Eigen::Vector2d &pos, double rot, double facerot, int engaged);
	void setLabel(const std::string &label);

	std::list< std::list< double > > labelledStfPatterns() const;

private:
	int m_id;
	Eigen::Vector2d m_pos;
	Eigen::Vector2d m_initialPos;
	Eigen::Vector2d m_velVec;

	/// total distance travelled
	double m_totalDistanceTravelled;
	
	bool m_initialPosIsSet;

	std::map<std::string, std::shared_ptr<Feature> > m_features;
	std::list<std::string> m_labels;
	std::string m_currentLabel;

};

