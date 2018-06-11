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


#include "src/CachedContainer.h"
#include <algorithm>
#include <cassert>

namespace Tral
{

	CachedContainer::CachedContainer( DataSource* data_source )
		: _conteiner( data_source )
		, _cached_rows( DefaultCacheSize )
		, _cached_rows_first_index( 0 )
	{
	}


	CachedContainer::~CachedContainer()
	{
	}


	void CachedContainer::move_cached_rows( int index )
	{
		int const reserved = _cached_rows.capacity();
		int const size     = _cached_rows.size();
		assert( reserved >= 0 && size >= 0 );

		int const move_to_left  = std::max( _cached_rows_first_index - index, 0 );
		int const move_to_right = std::min( index - _cached_rows_first_index + size, 0 );

		// Moving to the left:
		// a. if cache not full
		for (int i = size - move_to_left; i >= 0 && i < size && _cached_rows.size() < reserved; ++i)
			_cached_rows.push_back( _cached_rows[i] );
		// b. shift cache elements
		for (int i = size - move_to_left; i < size && i >= move_to_left; --i)
			_cached_rows[i] = _cached_rows[i - move_to_left];
		// c. add new cache elements
		for (int i = move_to_left - 1; i >= 0; --i)
		{
			_cached_rows[i] = _conteiner.get_previous( _cached_rows[i] );
			assert( _cached_rows[i] != nullptr );
		}

		// Moving to the right.


		assert( _cached_rows.capacity() == reserved );
	}

} // namespace Tral
