/*	CADET - Center for Advances in Digital Entertainment Technologies
	Copyright 2012 Fachhochschule Salzburg GmbH

		http://www.cadet.at

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef BOOST_ALL_DYN_LINK
	#define BOOST_ALL_DYN_LINK
#endif

#include "_2RealApplication.h"

void printBasicTypeMetainfo( _2Real::app::TypeMetainfo const& );
void printCustomTypeMetainfo( _2Real::app::CustomTypeMetainfo const& );

void printBasicTypeMetainfo( _2Real::app::TypeMetainfo const& meta )
{
	assert( meta.isValid() );
	std::cout << "basic type " << meta.getName() << std::endl;
}

void printCustomTypeMetainfo( _2Real::app::CustomTypeMetainfo const& meta )
{
	assert( meta.isValid() );

	std::cout << "custom type " << meta.getName() << std::endl;
	std::cout << meta.getDescription() << std::endl;

	auto fields = meta.getDataFields();
	for ( auto it : fields )
	{
		std::cout << "has field " << it.first << std::endl;
		if ( it.second->isBasicType() )
			printBasicTypeMetainfo( *( static_cast< const _2Real::app::TypeMetainfo * >
			( it.second.get() ) ) );
		else
			printCustomTypeMetainfo( *( static_cast< const _2Real::app::CustomTypeMetainfo * >
			( it.second.get() ) ) );
	}
}

void handleFuture( std::future< _2Real::BlockResult > &obj, std::string const& info = "" )
{
	obj.wait();
	_2Real::BlockResult val = obj.get();
	switch ( val )
	{
	case _2Real::BlockResult::CARRIED_OUT:
		std::cout << "---- " << info << " was carried out" << std::endl;
		break;
	case _2Real::BlockResult::IGNORED:
		std::cout << "---- " << info << " was ignored" << std::endl;
		break;
	}
}

void infos(std::pair<_2Real::app::BundleHandle, _2Real::app::BundleMetainfo> const& loadedBundle)
{
	auto bundleinfo = loadedBundle.second;

	std::cout << "basic bundle info" << std::endl;
	std::cout << "description " << bundleinfo.getDescription() << std::endl;
		std::cout << "category " << bundleinfo.getCategory() << std::endl;
		std::cout << "author " << bundleinfo.getAuthor() << std::endl;
		std::cout << "contact " << bundleinfo.getContact() << std::endl;
		std::cout << "version " << bundleinfo.getVersion() << std::endl;

		std::cout << std::endl;

		// i know that these are custom types, so no need for casting around
		auto typeinfos = bundleinfo.getExportedTypes();
		std::cout << "number of exported types : " << typeinfos.size() << std::endl;
		if ( !typeinfos.empty() ) std::cout << "--------------------------------" << std::endl;
		for ( auto it : typeinfos )
		{
			printCustomTypeMetainfo( it );
			std::cout << "--------------------------------" << std::endl;
		}

		std::cout << std::endl;

		auto blockinfos = bundleinfo.getExportedBlocks();
		std::cout << "exported blocks " << blockinfos.size() << std::endl;
		for ( auto it : blockinfos )
		{
			std::cout << "\t" << it.getName() << std::endl;
			std::cout << "\t" << it.getDescription() << std::endl;
			auto dependencies = it.getDependenciesByName();
			std::cout << "\tdependencies\t";
			for ( auto it : dependencies ) std::cout << it << " ";
			std::cout << std::endl;

			std::vector< _2Real::app::InletMetainfo > inletinfos = it.getInlets();

			std::cout << "\tinlets " << inletinfos.size() << std::endl;
			for ( auto it : inletinfos )
			{
				std::cout << "\t\tname " << it.getName() << std::endl;
				std::cout << "\t\tdescription " << it.getDescription() << std::endl;
				std::cout << "\t\tdatatype " << it.getTypeMetainfo()->getName() << std::endl;
				std::cout << "\t\tinitial "; 
				boost::apply_visitor< _2Real::PrintOutVisitor >
					( _2Real::PrintOutVisitor( std::cout ), it.getInitialValue() ); 
				std::cout << std::endl;
				std::cout << "\t\tis multi " << std::boolalpha << it.isMulti() << std::endl;
			}

			std::vector< _2Real::app::OutletMetainfo > outletinfos = it.getOutlets();
			std::cout << "\toutlets "  << outletinfos.size() << std::endl;
			for ( auto it : outletinfos )
			{
				std::cout << "\t\tname " << it.getName() << std::endl;
				std::cout << "\t\tdescription " << it.getDescription() << std::endl;
				std::cout << "\t\tdatatype " << it.getTypeMetainfo()->getName() << std::endl;
				std::cout << "\t\tinitial "; 
				boost::apply_visitor< _2Real::PrintOutVisitor >
					( _2Real::PrintOutVisitor( std::cout ), it.getInitialValue() ); 
				std::cout << std::endl;
			}

			std::vector< _2Real::app::ParameterMetainfo > paraminfos = it.getParameters();
			std::cout << "\tparameters " << paraminfos.size() << std::endl;
			for ( auto it : paraminfos )
			{
				std::cout << "\t\tname " << it.getName() << std::endl;
				std::cout << "\t\tdescription " << it.getDescription() << std::endl;
				std::cout << "\t\tdatatype " << it.getTypeMetainfo()->getName() << std::endl;
				std::cout << "\t\tinitial "; 
				boost::apply_visitor< _2Real::PrintOutVisitor >
					( _2Real::PrintOutVisitor( std::cout ), it.getInitialValue() ); 
				std::cout << std::endl;
			}
		}
}

int main( int argc, char *argv[] )
{
	try
	{
		_2Real::app::Engine testEngine;

		auto idBundle = testEngine.loadBundle("Bundle_Identification");

		infos(idBundle);

		auto loadedBundle = testEngine.loadBundle("Bundle_Estia");

		infos(loadedBundle);


		// -------create block instance---------

		_2Real::app::TimerHandle timer = testEngine.createTimer( 30.0 );
		_2Real::app::ThreadpoolHandle threadpool = 
			testEngine.createThreadpool( _2Real::ThreadpoolPolicy::FIFO );


		_2Real::app::BlockHandle blockModelling = 
			loadedBundle.first.createBlock("attentionModelling", 
			threadpool, 
			std::vector< _2Real::app::BlockHandle >());

		_2Real::app::BlockIo mio = blockModelling.getBlockIo();
		auto inletModellingHumans = std::dynamic_pointer_cast< _2Real::app::InletHandle >
			(mio.mInlets[0]);
		auto inletModellingLabels = std::dynamic_pointer_cast<_2Real::app::InletHandle>
			(mio.mInlets[1]);
		auto outletModellingAttentives = std::dynamic_pointer_cast<_2Real::app::OutletHandle>
			(mio.mOutlets[0]);

		_2Real::app::BlockHandle blockTracking = 
			loadedBundle.first.createBlock("attentionTracking", 
			threadpool, 
			std::vector< _2Real::app::BlockHandle >());

		_2Real::app::BlockIo tio = blockTracking.getBlockIo();
		auto outletTrackingHumans = std::dynamic_pointer_cast< _2Real::app::OutletHandle >
			(tio.mOutlets[0]);

		auto cfg = std::shared_ptr<_2Real::DataItem>(new _2Real::DataItem());
		*cfg = loadedBundle.second.getExportedType("trackingConfig").makeData();
		auto param = tio.mParameters[0];
		param->setValue(cfg);

		auto label = std::shared_ptr<_2Real::DataItem>(new _2Real::DataItem());
		*label = loadedBundle.second.getExportedType("humanLabel").makeData();
		inletModellingLabels->setValue(label);

		std::future< _2Real::BlockResult > setupM = blockModelling.setup();
		handleFuture( setupM );

		std::future< _2Real::BlockResult > setupT = blockTracking.setup();
		handleFuture( setupT );

		auto link = testEngine.link(*inletModellingHumans.get(), *outletTrackingHumans.get());

		blockModelling.getUpdatePolicy().set(_2Real::DefaultUpdatePolicy::ANY);

		outletModellingAttentives->registerToNewData([](std::shared_ptr<const _2Real::DataItem> d)
		{
			std::cout << "NEW VALUE ";
			d->apply_visitor<_2Real::PrintOutVisitor>(_2Real::PrintOutVisitor(std::cout));
			std::cout << std::endl;

			std::cout << "attn " 
				<< (uint32_t) boost::get<_2Real::CustomDataItem>(*d).getValue<uint8_t>("attentionStf") 
				<< std::endl;
		});
		
		//outletTrackingHumans->registerToNewData([](std::shared_ptr<const _2Real::DataItem> d)
		//{
		//	std::cout << "NEW HUMAN ";
		//	d->apply_visitor<_2Real::PrintOutVisitor>(_2Real::PrintOutVisitor(std::cout));
		//	std::cout << std::endl;
		//});
		//

		auto s1 = blockTracking.startUpdating(timer);
		auto s2 = blockModelling.startUpdating();

		handleFuture(s1);
		handleFuture(s2);
		
		while (1)
		{
			std::this_thread::yield();

			std::string line;
			char lineEnd = '\n';
			std::getline( std::cin, line, lineEnd );
			if ( line == "q" )
				break;
			else if ( line == "stop" )
			{
				timer.stop();
			}
			else if ( line == "start" )
			{
				timer.start();
			}
		}

		auto stop = blockTracking.stopUpdating();
		handleFuture(stop);
		auto stop2 = blockModelling.stopUpdating();
		handleFuture(stop2);

		auto shut = blockTracking.shutdown();
		handleFuture(shut);
		auto shut2 = blockModelling.shutdown();
		handleFuture(shut2);

		testEngine.clear();

	}
	catch ( std::exception const& e )
	{
		std::cout << "-------------exception caught in main------------" << std::endl;
		std::cout << e.what() << std::endl;
		std::cout << "-------------exception caught in main------------" << std::endl;
	}

	while( 1 )
	{
		std::this_thread::yield();

		std::string line;
		char lineEnd = '\n';
		std::getline( std::cin, line, lineEnd );
		if ( line == "q" )
			break;
	}	

	return 0;
}