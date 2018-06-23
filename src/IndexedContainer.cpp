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


#include "src/IndexedContainer.h"
#include "src/DataSource.h"
#include <cassert>


namespace Tral
{

	IndexedContainer::IndexedContainer( DataSource* data_source )
		: _data_source( data_source )
		, _string_list()
	{
		assert( _data_source != nullptr );
		reload();
	}


	IndexedContainer::~IndexedContainer()
	{
	}


	IndexedContainer::ConstIterator IndexedContainer::begin()
	{
		if (!_string_list.empty())
			return _string_list.begin();

		return invalid_iterator();
	}


	IndexedContainer::ConstIterator IndexedContainer::invalid_iterator() const
	{
		assert( [this]()->bool { static ConstIterator const invalid_it = _string_list.end(); return invalid_it == _string_list.end(); }() );
		return _string_list.end();
	}


	IndexedContainer::ConstIterator IndexedContainer::get_previous( ConstIterator string )
	{
		assert( string != invalid_iterator() );

		if (string == _string_list.begin())
			return invalid_iterator();

		return --string;
	}


	IndexedContainer::ConstIterator IndexedContainer::get_next( ConstIterator string )
	{
		assert( string != invalid_iterator() );

		return ++string;
	}


	void IndexedContainer::reload()
	{
		_string_list.clear();
		assert( _data_source != nullptr );

		unsigned offset = 0;
		std::string string;
		do
		{
			unsigned const string_offset = offset;
			offset = _data_source->get_string( string_offset, string );

			_string_list.push_back( IndexedString( string_offset, string ) );
		}
		while( offset != 0 );
	}

} /* namespace Tral */

