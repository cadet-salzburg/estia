#pragma once

#include "_2RealBundle.h"

#include <memory>

class Modeller;

class Modelling : public _2Real::bundle::Block
{
public:

	static void getBlockMetaInfo(_2Real::bundle::BlockMetainfo &,
		_2Real::bundle::TypeMetainfoCollection const&);

	Modelling(_2Real::bundle::BlockIo const&,
		std::vector<std::shared_ptr<_2Real::bundle::Block>> const&);
	~Modelling();

	void setup();
	void update();
	void shutdown();

private:
	std::shared_ptr<Modeller> m_modeller;
};

