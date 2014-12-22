#include "Feature.h"
#include <algorithm>


Feature::Feature()
{
}


Feature::~Feature()
{
}

double Feature::value() const
{
	if (m_history.size() > 0)
		return m_history.front();
	else
		return 0.0;
}

double Feature::ltfMean() const
{
	return acc::mean(m_accLtf);
}

double Feature::ltfStd() const
{
	return sqrt(acc::variance(m_accLtf));
}

void Feature::pushValue(double value)
{
	if (m_history.size() >= Feature::MAX_HISTORY)
	{
		m_history.pop_back();
	}
	m_history.push_front(value);
}

void Feature::updateWithValue(double value, bool computeStats, int numFrames)
{
	if (std::isnan(value) || std::isinf(value))
		value = 0.0;

	pushValue(value);
	if (computeStats)
	{
		m_accLtf(value);

		m_accStf = doubleAcc();
		for (int idx = 0; idx < std::min<int>(numFrames, m_history.size()); idx++)
		{
			m_accStf(m_history[idx]);
		}

		m_means.push_back(acc::mean(m_accStf));
		m_stds.push_back(sqrt(acc::variance(m_accStf)));
	}
}