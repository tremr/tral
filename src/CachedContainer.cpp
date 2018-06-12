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
		, _cached_rows( DefaultCacheSize, nullptr )
		, _cached_rows_first_index( 0 )
		, _size( 0 )
	{
	}


	CachedContainer::~CachedContainer()
	{
	}


	void CachedContainer::move_cached_rows( int index )
	{
		int const reserved = _cached_rows.capacity();
		assert( reserved >= 0 && _size >= 0 && _size <= reserved );

		int const move_to_left  = std::max( _cached_rows_first_index - index, 0 );
		int const move_to_right = std::min( index - _cached_rows_first_index - _size, 0 );

		// Moving to the left:
		if (move_to_left > 0)
		{
			_size = std::min( _size + move_to_left, reserved );
			for (int i = _size - 1; i > move_to_left; --i)
			{
				_cached_rows[i] = _cached_rows[i - move_to_left];
			}

			for (int i = move_to_left, k = i - 1; i > 0; --i, --k)
			{
				_cached_rows[k] = _conteiner.get_previous( _cached_rows[i] );
				assert( _cached_rows[k] != nullptr ); // Unexpected begin of data stream obtained. This should not happen if the stream remains constant and data is only added.

				if (_cached_rows[k] == nullptr)
				{
					_size -= i;
					for (int j = 0; j < _size; ++j)
						_cached_rows[j] = _cached_rows[j + i];

					break;
				}
			}

			_cached_rows_first_index -= move_to_left;
		}

		// Moving to the right:
		if (move_to_right > 0)
		{
			int i = move_to_right;
			for (; i < _size; ++i)
				_cached_rows[i - move_to_right] = _cached_rows[i];

			do
			{
				_cached_rows[i] = _conteiner.get_next( _cached_rows[i - 1] );
			}
			while (i < reserved && _cached_rows[i] != nullptr);

			_size = i;
			_cached_rows_first_index += move_to_right;
		}

		assert( _cached_rows.capacity() == reserved && _size >= 0 && _size <= reserved );
	}

} // namespace Tral
