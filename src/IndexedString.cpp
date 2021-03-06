// Copyright 2018 Roman Tremaskin. All Rights Reserved.

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


#include "IndexedString.h"


namespace Tral
{

	IndexedString::IndexedString( unsigned offset, std::string value )
		: _offset( offset )
		, _value( value )
		, _filter_links()
	{}


	IndexedString::IndexedString( IndexedString&& other )
		: _offset( other._offset )
		, _value( std::move( other._value ) )
		, _filter_links( std::move( other._filter_links ) )
	{}


	IndexedString::~IndexedString()
	{}


	std::string Tral::IndexedString::get_value() const
	{
		return _value;
	}

} /* namespace Tral */

