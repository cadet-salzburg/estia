 /*
	CADET - Center for Advances in Digital Entertainment Technologies
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

#include "IdentificationProviderBlock.h"
#include "common/_2RealData.h"

#include <Windows.h>

void IdentificationProvider::getBlockMetainfo( _2Real::bundle::BlockMetainfo &info, _2Real::bundle::TypeMetainfoCollection const& types )
{
	_2Real::bundle::BlockMetainfo &counterinfo = dynamic_cast< _2Real::bundle::BlockMetainfo & >( info );

	auto IDInfo = std::static_pointer_cast< const _2Real::bundle::CustomTypeMetainfo >( types.getTypeMetainfo( "ID" ) );
	auto globalIDInfo = std::static_pointer_cast< const _2Real::bundle::CustomTypeMetainfo >(types.getTypeMetainfo("globalID"));

	counterinfo.setBlockClass< IdentificationProvider >();
	counterinfo.setDescription( "IdentificationProvider" );

	_2Real::bundle::UpdatePolicyMetainfo policy = counterinfo.getUpdatePolicyMetainfo();
	policy.set( _2Real::DefaultUpdatePolicy::ANY );

	_2Real::bundle::InletMetainfo stringy = counterinfo.getInletMetainfo("identificator");
	stringy.setDescription("name of identificator");
	stringy.setDatatypeAndInitialValue(std::string("n/a"));

	_2Real::bundle::InletMetainfo in = counterinfo.getInletMetainfo( "localID" );
	in.setDescription( "local ID" );
	in.setDatatypeAndInitialValue(IDInfo->makeData());
		
	_2Real::bundle::OutletMetainfo out = counterinfo.getOutletMetainfo( "globalID" );
	out.setDescription( "global ID" );
	out.setDatatypeAndInitialValue(globalIDInfo->makeData());
}

IdentificationProvider::IdentificationProvider( _2Real::bundle::BlockIo const& io, std::vector< std::shared_ptr< _2Real::bundle::Block > > const& dependencies ) :
_2Real::bundle::Block(io, dependencies), 
m_nNextGlobalID(0)
{
}

IdentificationProvider::~IdentificationProvider()
{
}

void IdentificationProvider::setup()
{	
}

void IdentificationProvider::update()
{
	auto inlet0 = std::dynamic_pointer_cast< _2Real::bundle::InletHandle >(mIo.mInlets[0]);
	auto inlet1 = std::dynamic_pointer_cast< _2Real::bundle::InletHandle >(mIo.mInlets[1]);
	auto outlet0 = std::dynamic_pointer_cast< _2Real::bundle::OutletHandle >(mIo.mOutlets[0]);

	// get const& ref to inlet data
	const std::string &identifier_in = boost::get<const std::string >(inlet0->getValue());

	_2Real::CustomDataItem const& ID_in = boost::get< _2Real::CustomDataItem >(inlet1->getValue());
	const uint64_t &uniqueID_in = ID_in.getValue< const uint64_t >("unique_id");
	const float &error_in = ID_in.getValue< const float >("error");

	// extract a reference to the custom type in the outlet data
	_2Real::CustomDataItem & out0 = boost::get< _2Real::CustomDataItem >(outlet0->getValue());
	std::string &identifier_out = out0.getValue< std::string >("identificator");
	_2Real::CustomDataItem & ID_out = out0.getValue< _2Real::CustomDataItem >("id");
	uint64_t &uniqueID_out = ID_out.getValue< uint64_t >("unique_id");
	float &error_out = ID_out.getValue< float >("error");

	uniqueID_out = uniqueID_in;
	identifier_out = identifier_in;
	error_out = error_in;
}

void IdentificationProvider::shutdown()
{
}