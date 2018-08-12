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
#include "include/List.h"
#include <cassert>
#include <iostream>

namespace Tral
{

	IndexedContainer::IndexedContainer( DataSource* data_source,  Callback* callback, CachedContainer* cache )
		: Log( "IndexedContainer" )
		, _data_source( data_source )
		, _string_list()
		, _list_mutex()
		, _thread_mutex()
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


	IndexedContainer::ConstIterator IndexedContainer::begin()
	{
//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		ReadOnlyLock lock(_list_mutex);
		return _string_list.empty() ? invalid_iterator() : _string_list.begin();
	}


	IndexedContainer::ConstIterator IndexedContainer::invalid_iterator() const
	{
		assert( [this]()->bool { static ConstIterator const invalid_it = _string_list.end(); return invalid_it == _string_list.end(); }() );
		return _string_list.end();
	}


	IndexedContainer::ConstIterator IndexedContainer::get_previous( ConstIterator string )
	{
		assert( string != invalid_iterator() );

//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		ReadOnlyLock lock(_list_mutex);
		if (string == _string_list.begin())
			return invalid_iterator();

		return --string;
	}


	IndexedContainer::ConstIterator IndexedContainer::get_next( ConstIterator string )
	{
		assert( string != invalid_iterator() );

//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		ReadOnlyLock lock(_list_mutex);
		return ++string;
	}


	unsigned IndexedContainer::get_size() const
	{
//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		ReadOnlyLock lock(_list_mutex);
		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		return _string_list.size();
	}


	void IndexedContainer::reload( ConstIterator first_cached, ConstIterator last_cached )
	{
		int const first_offset = first_cached != invalid_iterator() ? first_cached->get_offset() : 0;
		int const last_offset  = last_cached != invalid_iterator() ? last_cached->get_offset() : 0;

		_thread = std::thread( &IndexedContainer::reload_thread_function, this );
	}


	void IndexedContainer::lock()
	{
//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		_thread_mutex.lock_shared();
	}


	void IndexedContainer::unlock()
	{
//		log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
		_thread_mutex.unlock_shared();
	}


	void IndexedContainer::reload_thread_function()
	{
		assert( _data_source != nullptr );
		{
			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			ReadWriteLock lock(_list_mutex);
			_string_list.clear();
		}

		unsigned offset = 0;
		std::string string;
		do
		{
			unsigned const string_offset = offset;
			offset = _data_source->get_string( string_offset, string );

//			std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

			_callback->on_insert_rows_begin( _string_list.size(), _string_list.size() );
			_thread_mutex.lock();
			_thread_mutex.unlock();
//			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			{
				ReadWriteLock lock(_list_mutex);
				_string_list.push_back( IndexedString( string_offset, string ) );
			}
//			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			_callback->on_insert_rows_end( _string_list.size() - 1, _string_list.size() - 1 );
			_thread_mutex.lock();
			_thread_mutex.unlock();
		}
		while (offset != 0);

		ConstIterator it = _string_list.begin();
		int i = 0;
		while (it != _string_list.end())
		{
			assert( i < _string_list.size() );

//			std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

			_callback->on_remove_rows_begin( i, i );
			_thread_mutex.lock();
			_thread_mutex.unlock();
//			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			{
				it = _string_list.erase( it );
				ReadWriteLock lock(_list_mutex);
				_cache->remove_row( it );
				++it;
				++i;
			}
//			log() << __FUNCTION__ << "::" << _string_list.size() << std::endl;
			_callback->on_remove_rows_end( i, i );
			_thread_mutex.lock();
			_thread_mutex.unlock();
		}
	}

} /* namespace Tral */

