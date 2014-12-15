#include "Modeller.h"

#include "svm.h"
#include <fstream>
//#include <boost/date_time.hpp>
#include <chrono>
#include <sstream>
#include <iomanip>

Modeller::Modeller()
{
}


Modeller::~Modeller()
{
}

void Modeller::trainSvm()
{

	
}

void Modeller::savePatterns()
{
	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);
	std::stringstream filename;
	filename << "data/" << std::put_time(std::localtime(&now_c), "%F_%H-%M-%S") << "_stf.txt";

	std::ofstream svmdata;
	svmdata.open(filename.str());
}