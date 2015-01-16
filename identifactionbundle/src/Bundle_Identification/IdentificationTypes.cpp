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

#include "_2RealBundle.h"
#include "IdentificationProviderBlock.h"

void getBundleMetainfo( _2Real::bundle::BundleMetainfo &info )
{
	info.setAuthor( "AEF" );
	info.setDescription( "Bundle providing identification data types and a global identification provider" );
	info.setCategory( "ID" );
	info.setContact( "research@aec.at" );
	info.setVersion( 1, 0, 0 );

	info.exportsType("ID", { _2Real::declareField("unique_id", "ulong"), _2Real::declareField("error", "float")});
	info.exportsType("globalID", { _2Real::declareField("identificator", "string"), _2Real::declareField("id", "ID") });
	info.exportsBlock("IdentificationProvider", { _2Real::declareInlet("identificator"), _2Real::declareInlet("localID") }, { _2Real::declareOutlet("globalID") }, {} );
}

void getTypeMetainfo( _2Real::bundle::CustomTypeMetainfo &info, _2Real::bundle::TypeMetainfoCollection const& existingTypes )
{
	if (info.getName() == "ID")
	{
		info.setDescription("ID encapsuling data type");
		info.setInitialFieldValue("unique_id", (uint64_t)0); // unique id
		info.setInitialFieldValue("error", 0.0f); // possible error of identification (0.0 [none] .. 1.0 [worst])
	}
	else if (info.getName() == "globalID")
	{
		auto IDInfo = std::static_pointer_cast< const _2Real::bundle::CustomTypeMetainfo >(existingTypes.getTypeMetainfo("ID"));

		info.setDescription("testing complex types, that is, types where at least one field is a custom type");
		info.setInitialFieldValue("identificator", std::string("n/a")); // string describing identificator
		info.setInitialFieldValue("id", IDInfo->makeData());
	}
}

void getBlockMetainfo( _2Real::bundle::BlockMetainfo &info, _2Real::bundle::TypeMetainfoCollection const& types )
{
	if ( info.getName() == "IdentificationProvider" )
		IdentificationProvider::getBlockMetainfo( info, types );
}