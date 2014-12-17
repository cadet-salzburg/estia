#include "Modeller.h"

#include "svm.h"
#include <fstream>
//#include <boost/date_time.hpp>
#include <chrono>
#include <sstream>
#include <iomanip>

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

static char *line = NULL;
static int max_line_len;

static char* readline(FILE *input)
{
	int len;
	
	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}

Modeller::Modeller() : 
	m_modelStf(nullptr),
	m_modelLtf(nullptr)
{
}


Modeller::~Modeller()
{
}

void Modeller::readProblem(const std::string &filenameStr,
	svm_problem &prob, svm_parameter &param, svm_node *x_space)
{

	const char *filename = filenameStr.c_str();

	int max_index, inst_max_index, i;
	size_t elements, j;
	FILE *fp = fopen(filename,"r");
	char *endptr;
	char *idx, *val, *label;

	if(fp == NULL)
	{
		fprintf(stderr,"can't open input file %s\n",filename);
		exit(1);
	}

	prob.l = 0;
	elements = 0;

	int max_line_len = 1024;
	char *line = Malloc(char,max_line_len);
	while(readline(fp)!=NULL)
	{
		char *p = strtok(line," \t"); // label

		// features
		while(1)
		{
			p = strtok(NULL," \t");
			if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
				break;
			++elements;
		}
		++elements;
		++prob.l;
	}
	rewind(fp);

	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct svm_node *,prob.l);
	x_space = Malloc(struct svm_node,elements);

	max_index = 0;
	j=0;
	for(i=0;i<prob.l;i++)
	{
		inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
		readline(fp);
		prob.x[i] = &x_space[j];
		label = strtok(line," \t\n");
		if(label == NULL) // empty line
			exit_input_error(i+1);

		prob.y[i] = strtod(label,&endptr);
		if(endptr == label || *endptr != '\0')
			exit_input_error(i+1);

		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				exit_input_error(i+1);
			else
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			if(endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				exit_input_error(i+1);

			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;
		x_space[j++].index = -1;
	}

	if(param.gamma == 0 && max_index > 0)
		param.gamma = 1.0/max_index;

	if(param.kernel_type == PRECOMPUTED)
		for(i=0;i<prob.l;i++)
		{
			if (prob.x[i][0].index != 0)
			{
				fprintf(stderr,"Wrong input format: first column must be 0:sample_serial_number\n");
				exit(1);
			}
			if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
			{
				fprintf(stderr,"Wrong input format: sample_serial_number out of range\n");
				exit(1);
			}
		}

	fclose(fp);
}

void Modeller::trainSvm()
{
	std::stringstream filenameStf;
	filenameStf << m_filenameBase << "_stf.txt";

	svm_node *xspaceStf;
	svm_problem probStf;
	svm_parameter paramStf;

	readProblem(filenameStf.str(), probStf, paramStf, xspaceStf);
	m_modelStf = svm_train(&probStf, &paramStf);
}

double Modeller::predict(const Human::Pattern &pattern, Modeller::ATTENTION_TYPE type)
{
	const svm_model *model = type == ATTENTION_TYPE::STF ? m_modelStf : m_modelLtf;

	if (model != nullptr)
	{
		svm_node *node = new svm_node[pattern.features.size()];
		int idx = 0;
		for (double val : pattern.features)
		{
			node[idx].value = val;
			node[idx].index = idx++;
		}

		double stfPrediction = svm_predict(model, node);
	}
}

void Modeller::savePatterns()
{
	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);
	std::stringstream filename;
	filename << "data/" << std::put_time(std::localtime(&now_c), "%F_%H-%M-%S") << "_stf.txt";

	std::ofstream svmdata;
	svmdata.open(filename.str());

	for (const Human::Pattern &pattern : m_patternsStf)
	{
		svmdata << pattern.label;

		int idx = 1;
		for (double val : pattern.features)
		{
			svmdata << " " << idx << ":" << val;
		}

		svmdata << std::endl;
	}

	svmdata.close();
}