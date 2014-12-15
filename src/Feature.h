#pragma once
#include <deque>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

namespace acc = boost::accumulators;

class Feature
{
public:

	static const int MAX_HISTORY = 15;

	Feature();
	~Feature();
	void updateWithValue(double value, bool computeStats = true, int numFrames = 3);

	const std::deque<double>& stfMeans() const { return m_means; }
	const std::deque<double>& stfStds() const { return m_stds; }
	double ltfMean() const;
	double ltfStd() const;

	double value() const;

private:
	typedef acc::accumulator_set<double, acc::features<acc::tag::mean, acc::tag::variance> > 
		doubleAcc;
	doubleAcc m_accStf, m_accLtf;
	std::deque<double> m_history;
	std::deque<double> m_means, m_stds;

	void pushValue(double value);
};

