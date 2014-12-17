#pragma once
#include <list>

#include "Human.h"


class Modeller
{
public:
	enum class ATTENTION_TYPE
	{
		STF, 
		LTF
	};

	Modeller();
	~Modeller();

	void trainSvm();
	void savePatterns();
	void readProblem(const std::string &filenameStr, svm_problem &prob, svm_parameter &param,
		svm_node *x_space);
	double predict(const Human::Pattern &pattern);

private:
	std::list<Human::Pattern> m_patternsStf, m_patternsLtf;
	std::string m_filenameBase;
	svm_model *m_modelStf;
	svm_model *m_modelLtf;
};

