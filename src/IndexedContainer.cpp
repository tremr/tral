// Copyright 2018 Roman Tremaskin. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "IndexedContainer.h"

namespace Tral
{

	IndexedContainer::IndexedContainer( DataSource* data_source )
		: _data_source( data_source )
		, _string_list()
	{
	}


	IndexedContainer::~IndexedContainer()
	{
	}


	const IndexedString* Tral::IndexedContainer::get_previous( const IndexedString* string )
	{
		return nullptr;
	}


	const IndexedString* Tral::IndexedContainer::get_next( const IndexedString* string )
	{
		return nullptr;
	}

} /* namespace Tral */

