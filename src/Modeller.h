#pragma once
#include <list>

#include "Human.h"

class Modeller
{
public:
	Modeller();
	~Modeller();

	void trainSvm();
	void savePatterns();

private:
	std::list<Human::Pattern> m_patternsStf, m_patternsLtf;
};

