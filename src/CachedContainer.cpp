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
#include <iostream>

namespace Tral
{

	CachedContainer::CachedContainer( DataSource* data_source, Callback* callback )
		: _conteiner( data_source, callback )
		, _cached_rows( DefaultUICacheSize * NativeCacheRedundancy, _conteiner.invalid_iterator() )
		, _cached_rows_first_index( -1 )
		, _size( 0 )
	{
		reset();
	}


	CachedContainer::~CachedContainer()
	{
	}


	std::string CachedContainer::get_row( int index )
	{
		int const cache_capacity     = _cached_rows.capacity();
		int const shift_with_reserve = cache_capacity / NativeCacheRedundancy;
		int new_cache_begin          = _cached_rows_first_index;

		assert( shift_with_reserve < cache_capacity );

		if (_size == 0) // TODO: this must be unnecessary.
		{
			reset();
			return "no data";
		}

		if (index < _cached_rows_first_index)
			new_cache_begin = std::max( index - shift_with_reserve, 0 );
		else if (index > _cached_rows_first_index + _size - 1)
			new_cache_begin = std::max( index - cache_capacity + shift_with_reserve, 0 );

		move_cached_rows( new_cache_begin );
		assert( index - _cached_rows_first_index >= 0 );

		std::cout << __FUNCTION__ << ":: _cached_rows_first_index:" << _cached_rows_first_index << " _size:" << _size << " index:" << index;

		assert( index - _cached_rows_first_index < _size );
		assert( _cached_rows[index - _cached_rows_first_index] != _conteiner.invalid_iterator() );

		std::cout << " value:" << _cached_rows[index - _cached_rows_first_index]->get_value() << std::endl;

		return _cached_rows[index - _cached_rows_first_index]->get_value();

//		return "<font style='color:#00A900;background-color:#FF0000'>Mar 26 06:52:59 <font style='color:#6495ED;background-color:#FFA500'>marked fragment</font> 2639.118 1677 filtered fragment</font>";
	}


	int CachedContainer::get_row_count() const
	{
		return _conteiner.get_size();
	}


	void CachedContainer::reset()
	{
		_cached_rows_first_index = -1;
		_size = 0;
		_cached_rows[0] = _conteiner.begin();

		if (_cached_rows[0] != _conteiner.invalid_iterator())
		{
			_cached_rows_first_index = 0;
			_size = 1;
		}
	}


	void CachedContainer::move_cached_rows( int new_cache_begin )
	{
		int const reserved = _cached_rows.capacity();
		assert( reserved > 0 && _size > 0 && _size <= reserved );
		assert( new_cache_begin >= 0 );

		int const move_to_left  = std::max( _cached_rows_first_index - new_cache_begin, 0 );
		int const move_to_right = std::max( new_cache_begin - _cached_rows_first_index, 0 );

		// Moving to the left:
		if (move_to_left > 0)
		{
			assert( move_to_right == 0 );
			_size = std::min( _size + move_to_left, reserved );

			int i = _size - 1;
			for (; i >= move_to_left; --i)                     // Existing cache will be partially saved.
			{
				_cached_rows[i] = _cached_rows[i - move_to_left];
			}

			IndexedContainer::ConstIterator it = _cached_rows[0];
			int const left_jump = move_to_left - _size;
			for (int j = 0; j < left_jump; ++j)                // Cache will be fully updated after jump to the left.
			{
				it = _conteiner.get_previous( it );
				assert( it != _conteiner.invalid_iterator() ); // Unexpected begin of data stream obtained. This should not happen if the stream remains constant and data is only adding.
			}

			for (; i >= 0; --i)                                // Fill the cache if it needed.
			{
				_cached_rows[i] = it = _conteiner.get_previous( it );
				assert( _cached_rows[i] != _conteiner.invalid_iterator() ); // Unexpected begin of data stream obtained. This should not happen if the stream remains constant and data is only adding.
			}
		}

		// Moving to the right:
		if (move_to_right > 0)
		{
			assert( move_to_left == 0 );

			for (int i = move_to_right; i < _size; ++i)        // Existing cache will be partially saved.
			{
				assert( i - move_to_right >= 0 );
				_cached_rows[i - move_to_right] = _cached_rows[i];
			}

			IndexedContainer::ConstIterator it = _cached_rows[_size - 1];
			int const right_jump = move_to_right - _size + 1;
			for (int j = 0; j < right_jump; ++j)               // Cache will be fully updated after jump to the right.
			{
				_cached_rows[0] = it = _conteiner.get_next( it );
				assert( it != _conteiner.invalid_iterator() ); // Unexpected end of data stream obtained. This should not happen if the stream remains constant and data is only adding.
			}

			_size = std::max( _size - move_to_right, 1 );      // Size would equal to 1 if it was a jump to right.
		}

		IndexedContainer::ConstIterator it = _conteiner.get_next( _cached_rows[_size - 1] );
		while (_size < reserved && it != _conteiner.invalid_iterator()) // If cache not full try to fill it.
		{
			_cached_rows[_size++] = it;
			it = _conteiner.get_next( it );
		}

		_cached_rows_first_index = new_cache_begin;
		assert( _cached_rows.capacity() == reserved && _size > 0 && _size <= reserved );
	}

} // namespace Tral
