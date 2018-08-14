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
		: Log( "CachedContainer" )
		, _conteiner( data_source, callback, this )
		, _cached_rows()
		, _cached_rows_first_index( -1 )
	{
		_cached_rows.reserve( DefaultUICacheSize * NativeCacheRedundancy );
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

		assert( _cached_rows_first_index >= 0 );
		assert( shift_with_reserve <= cache_capacity );
		assert( _conteiner.get_size() != 0 );

		if (index < _cached_rows_first_index)
			new_cache_begin = std::max( index - shift_with_reserve, 0 );
		else if (index > _cached_rows_first_index + _cached_rows.size() - 1)
			new_cache_begin = std::max( index - cache_capacity + shift_with_reserve, 0 );

		move_cached_rows( new_cache_begin );
		assert( index - _cached_rows_first_index >= 0 );

		log() << __FUNCTION__ << ":: _cached_rows_first_index:" << _cached_rows_first_index << " size:" << _cached_rows.size() << " index:" << index;

		assert( index - _cached_rows_first_index < _cached_rows.size() );
		assert( _cached_rows[index - _cached_rows_first_index] != _conteiner.invalid_iterator() );

		std::cout << " value:" << _cached_rows[index - _cached_rows_first_index]->get_value() << std::endl;

		return _cached_rows[index - _cached_rows_first_index]->get_value();

//		return "<font style='color:#00A900;background-color:#FF0000'>Mar 26 06:52:59 <font style='color:#6495ED;background-color:#FFA500'>marked fragment</font> 2639.118 1677 filtered fragment</font>";
	}


	int CachedContainer::get_row_count() const
	{
		return _conteiner.get_size();
	}


	void CachedContainer::lock()
	{
		_conteiner.lock();
	}


	void CachedContainer::unlock()
	{
		_conteiner.unlock();
	}


	void CachedContainer::reset()
	{
		log() << __FUNCTION__ << std::endl;
		_cached_rows.clear();
		_cached_rows_first_index = 0;
		move_cached_rows( 0 );
	}


	void CachedContainer::remove_row( IndexedContainer::ConstIterator& indexed_it )
	{
		auto it = _cached_rows.begin();
		auto end = _cached_rows.end();
		for (; it != end; ++it)
		{
			if (*it == indexed_it)
				break;
		}

		if (it == end) return;

		_cached_rows.erase( it );
		IndexedContainer::ConstIterator indexed_to_add = _conteiner.get_next( *_cached_rows.rbegin() );
		if (indexed_to_add != _conteiner.invalid_iterator())
			_cached_rows.push_back( indexed_to_add );
	}


	void CachedContainer::move_cached_rows( int new_cache_begin )
	{
		size_t const reserved = _cached_rows.capacity();
		assert( reserved > 0 );
		assert( new_cache_begin >= 0 );
		assert( new_cache_begin == 0 ||  _cached_rows.size() > 0 );

		int const move_to_left  = std::max( _cached_rows_first_index - new_cache_begin, 0 );
		int const move_to_right = std::max( new_cache_begin - _cached_rows_first_index, 0 );

		// Moving to the left:
		if (move_to_left > 0)
		{
			assert( move_to_right == 0 );
			IndexedContainer::ConstIterator indexed_it = _cached_rows[0];

			int new_size = std::min( _cached_rows.size() + move_to_left, reserved );
			_cached_rows.resize( new_size );

			int i = _cached_rows.size() - 1;
			for (; i >= move_to_left; --i)                     // Existing cache will be partially saved.
			{
				_cached_rows[i] = _cached_rows[i - move_to_left];
			}

			int const left_long_jump = move_to_left - _cached_rows.size();
			for (int j = 0; j < left_long_jump; ++j)                // Cache will be fully updated after jump to the left. Lets find indexed iterator at the right border of _cached_rows after jump.
			{
				indexed_it = _conteiner.get_previous( indexed_it );
				assert( indexed_it != _conteiner.invalid_iterator() ); // Unexpected begin of data stream obtained. This should not happen if the stream remains constant and data is only adding.
			}

			for (; i >= 0; --i)                                // Fill the cache if it needed.
			{
				_cached_rows[i] = indexed_it = _conteiner.get_previous( indexed_it );
				assert( _cached_rows[i] != _conteiner.invalid_iterator() ); // Unexpected begin of data stream obtained. This should not happen if the stream remains constant and data is only adding.
			}
		}

		// Moving to the right:
		if (move_to_right > 0)
		{
			assert( move_to_left == 0 );
			IndexedContainer::ConstIterator indexed_it = *_cached_rows.rbegin();

			for (int i = move_to_right; i < _cached_rows.size(); ++i)        // Existing cache will be partially saved.
				_cached_rows[i - move_to_right] = _cached_rows[i];

			int const right_long_jump = move_to_right - _cached_rows.size() + 1;
			for (int j = 0; j < right_long_jump; ++j)               // Cache will be fully updated after jump to the right.
			{
				_cached_rows[0] = indexed_it = _conteiner.get_next( indexed_it );
				assert( indexed_it != _conteiner.invalid_iterator() ); // Unexpected end of data stream obtained. This should not happen if the stream remains constant and data is only adding.
			}

			assert( static_cast<int>( _cached_rows.size() ) >= 0 );
			_cached_rows.resize( std::max( static_cast<int>( _cached_rows.size() ) - move_to_right, 1 ) ); // Size would equal to 1 if it was a jump to right.
		}

		if (_cached_rows.empty())
		{
			log() << __FUNCTION__ << ": empty cache will be filled from begin of indexed list." << std::endl;

			IndexedContainer::ConstIterator indexed_it = _conteiner.begin();
			if (indexed_it == _conteiner.invalid_iterator())
			{
				log() << __FUNCTION__ << ": indexed list empty." << std::endl;
				return;
			}

			_cached_rows.push_back( indexed_it );
		}

		IndexedContainer::ConstIterator indexed_it = _conteiner.get_next( *_cached_rows.rbegin() );
		while (_cached_rows.size() < reserved && indexed_it != _conteiner.invalid_iterator()) // If cache not full try to fill it.
		{
			_cached_rows.push_back( indexed_it );
			indexed_it = _conteiner.get_next( indexed_it );
		}

		_cached_rows_first_index = new_cache_begin;
		assert( _cached_rows.capacity() == reserved && _cached_rows.size() > 0 && _cached_rows.size() <= reserved );
	}

} // namespace Tral
