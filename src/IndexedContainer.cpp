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
#include "src/CachedContainer.h"
#include "include/list.h"
#include <cassert>
#include <iostream>

namespace Tral
{

	IndexedContainer::IndexedContainer( DataSource* data_source,  Callback* callback, CachedContainer* cache )
		: Log( "IndexedContainer" )
		, _data_source( data_source )
		, _string_list()
		, _thread()
		, _callback( callback )
		, _cache( cache )
	{
		assert( _data_source != nullptr );
		reload( invalid_iterator(), invalid_iterator() );
	}


	IndexedContainer::~IndexedContainer()
	{
		_thread.join();
	}


	IndexedContainer::ConstIterator IndexedContainer::begin() const
	{
//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		return _string_list.visible_begin();
	}


	IndexedContainer::ConstIterator IndexedContainer::invalid_iterator() const
	{
		assert( [this]()->bool { static ConstIterator const invalid_it = _string_list.visible_end(); return invalid_it == _string_list.visible_end(); }() );
		return _string_list.visible_end();
	}


	IndexedContainer::ConstIterator IndexedContainer::get_previous( ConstIterator string )
	{
		assert( string != invalid_iterator() );

//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		if (string == begin())
			return invalid_iterator();

		return --string;
	}


	IndexedContainer::ConstIterator IndexedContainer::get_next( ConstIterator string )
	{
		assert( string != invalid_iterator() );

//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		return ++string;
	}


	unsigned IndexedContainer::get_size() const
	{
//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		return _string_list.size();
	}


	void IndexedContainer::reload( ConstIterator first_cached, ConstIterator last_cached )
	{
		(void) first_cached;
		(void) last_cached;
//		int const first_offset = first_cached != invalid_iterator() ? first_cached->get_offset() : 0;
//		int const last_offset  = last_cached != invalid_iterator() ? last_cached->get_offset() : 0;

		_thread = std::thread( &IndexedContainer::reload_thread_function, this );
	}


	IndexedContainer::ConstIterator IndexedContainer::make_visible( ConstIterator nearest_visible_it, IndexedList::iterator it )
	{
		ConstIterator visible_it = _string_list.get_visible( it );
		if (visible_it != _string_list.visible_end())
			return visible_it;

		int const index = _cache->get_not_cached_row_potential_index( it );

		if (_cache->is_fit_in_cache( index ))
		{
			_callback->on_insert_rows_begin( index, index );
			log() << __FUNCTION__ << ":: element will got " << index << " index of " << _string_list.visible_size() << "visible elements" << std::endl;
			nearest_visible_it = _string_list.visible_enable( nearest_visible_it, it );
			_cache->add_row( index, nearest_visible_it );
			_callback->on_insert_rows_end( index, index );
		}
		else
		{
			_callback->on_critical_section_begin();
			log() << __FUNCTION__ << ":: element does not fit in cache . Total " << _string_list.visible_size() << "visible elements" << std::endl;
			nearest_visible_it = _string_list.visible_enable( nearest_visible_it, it );
			_callback->on_critical_section_end();
		}

		return nearest_visible_it;
	}


	IndexedContainer::ConstIterator IndexedContainer::make_invisible( ConstIterator it )
	{
		int const index = _cache->get_cached_row_index( it );

		if (index >= 0)
		{
			_callback->on_remove_rows_begin( index, index );
			log() << __FUNCTION__ << ":: element " << index << " of " << _string_list.visible_size() << "visible elements" << std::endl;
			_cache->remove_row( index );
			it = _string_list.visible_disable( it );
			_callback->on_remove_rows_end( index, index );
		}
		else
		{
			_callback->on_critical_section_begin();
			log() << __FUNCTION__ << ":: not cached element of " << _string_list.visible_size() << "visible elements" << std::endl;
			it = _string_list.visible_disable( it );
			_callback->on_critical_section_end();
		}

		return it;
	}


	void IndexedContainer::reload_thread_function()
	{
		assert( _data_source != nullptr );
		{
			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			_string_list.clear();
		}

		log() << __FUNCTION__ << "::Test fill indexed_container!!! elements count " << _string_list.size() << std::endl;
		unsigned offset = 0;
		std::string string;
		ConstIterator last_visible_it = _string_list.visible_begin();
		do
		{
			unsigned const string_offset = offset;
			offset = _data_source->get_string( string_offset, string );

//			std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

			_callback->on_insert_rows_begin( _string_list.size(), _string_list.size() );
//			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			IndexedList::iterator it = _string_list.emplace( _string_list.end(), string_offset, std::move( string ) );
			_string_list.visible_enable( last_visible_it, it );

//			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			_callback->on_insert_rows_end( _string_list.size() - 1, _string_list.size() - 1 );
		}
		while (offset != 0);

		log() << __FUNCTION__ << "::Test make_invisible!!! elements count " << _string_list.size() << std::endl;
		{
			ConstIterator it( _string_list.visible_begin() );
			while (it != _string_list.visible_end())
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );
				it = make_invisible( it );

				++it;
			}
		}

		log() << __FUNCTION__ << "::Test make_visible!!! elements count " << _string_list.size() << std::endl;
		{
			IndexedList::iterator it( _string_list.begin() );
			ConstIterator last_visible_it( _string_list.visible_begin() );
			while (it != _string_list.end())
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );
				last_visible_it = make_visible( last_visible_it, it );

				++it;
			}
		}

		log() << __FUNCTION__ << "::Test clearing container!!! elements count " << _string_list.size() << std::endl;
		ConstIterator it( _string_list.visible_begin() );
		unsigned i = 0;
		while (it != _string_list.visible_end())
		{
			assert( i < _string_list.size() );

			std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );

			_callback->on_remove_rows_begin( i, i );
			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;

			_cache->remove_row( it );
			it = _string_list.erase_visible( it );

			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			_callback->on_remove_rows_end( i, i );
		}
	}

} /* namespace Tral */
