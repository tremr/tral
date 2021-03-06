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


#include "include/list.h"
#include "src/VisibleRowsModel.h"
#include "src/DataSource.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cassert>
#include <mutex>

namespace Tral
{

	class LockedMutex
	{
	public:
		LockedMutex() { _mutex.lock(); }
		~LockedMutex() { _mutex.unlock(); }

		void lock() { _mutex.lock(); }
		void unlock() { _mutex.unlock(); }

	private:
		std::recursive_mutex _mutex;
	};

	class List::ListImpl : Callback
	{
	public:
		ListImpl( Callback* callback );
		virtual ~ListImpl() override {}

		// Callback
		virtual void on_critical_section_begin() override;
		virtual void on_critical_section_end() override;
		virtual void on_insert_rows_begin( unsigned first, unsigned last ) override;
		virtual void on_insert_rows_end( unsigned first, unsigned last ) override;
		virtual void on_remove_rows_begin( unsigned first, unsigned last ) override;
		virtual void on_remove_rows_end( unsigned first, unsigned last ) override;


		void critical_section_begin_ok();
		void critical_section_end_ok();
		void insert_rows_begin_ok();
		void insert_rows_end_ok();
		void remove_rows_begin_ok();
		void remove_rows_end_ok();

		std::string get_row( int index ) const;
		int         get_row_count() const;

	private:
		mutable LockedMutex          _begin_sync_holder;
		mutable std::recursive_mutex _end_sync_mutex;

		DataSource       _data_source;
		Callback*        _callback;
		VisibleRowsModel _model;

	};


	std::ostream& Log::log() const
	{
		int const DayInSeconds = 86400;

		auto now          = std::chrono::high_resolution_clock::now();
		auto duration     = now.time_since_epoch();

		duration         %= std::chrono::duration<long long int, std::ratio<DayInSeconds>>( 1 );
		auto hours        = std::chrono::duration_cast<std::chrono::hours>( duration );

		duration         %= std::chrono::hours( 1 );
		auto minutes      = std::chrono::duration_cast<std::chrono::minutes>( duration );

		duration         %= std::chrono::minutes( 1 );
		auto seconds      = std::chrono::duration_cast<std::chrono::seconds>( duration );

		duration         %= std::chrono::seconds( 1 );
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>( duration );

		duration         %= std::chrono::milliseconds( 1 );
		auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>( duration );

		std::cout << std::setw(2) << std::right << std::setfill( '0' ) << hours.count()        << ".";
		std::cout << std::setw(2) << std::right << std::setfill( '0' ) << minutes.count()      << ".";
		std::cout << std::setw(2) << std::right << std::setfill( '0' ) << seconds.count()      << ":";
		std::cout << std::setw(3) << std::right << std::setfill( '0' ) << milliseconds.count() << ".";
		std::cout << std::setw(3) << std::right << std::setfill( '0' ) << microseconds.count() << " ";

		return std::cout << _name << "::";
	}


	List::ListImpl::ListImpl( Callback* callback )
		: _begin_sync_holder()
		, _data_source()
		, _callback( callback )
		, _model( &_data_source, this )
	{
		assert( callback != nullptr );
	}


	/*virtual*/ void List::ListImpl::on_critical_section_begin()
	{
		_callback->on_critical_section_begin();
		_begin_sync_holder.lock();
	}


	/*virtual*/ void List::ListImpl::on_critical_section_end()
	{
		_callback->on_critical_section_end();
		_begin_sync_holder.unlock();
		_end_sync_mutex.lock();
		_end_sync_mutex.unlock();
	}


	/*virtual*/ void List::ListImpl::on_insert_rows_begin( unsigned first, unsigned last )
	{
		_callback->on_insert_rows_begin( first, last );
		_begin_sync_holder.lock();
	}


	/*virtual*/ void List::ListImpl::on_insert_rows_end( unsigned first, unsigned last )
	{
		_callback->on_insert_rows_end( first, last );
		_begin_sync_holder.unlock();
		_end_sync_mutex.lock();
		_end_sync_mutex.unlock();
	}


	/*virtual*/ void List::ListImpl::on_remove_rows_begin( unsigned first, unsigned last )
	{
		_callback->on_remove_rows_begin( first, last );
		_begin_sync_holder.lock();
	}


	/*virtual*/ void List::ListImpl::on_remove_rows_end( unsigned first, unsigned last )
	{
		_callback->on_remove_rows_end( first, last );
		_begin_sync_holder.unlock();
		_end_sync_mutex.lock();
		_end_sync_mutex.unlock();
	}


	void List::ListImpl::critical_section_begin_ok()
	{
		_end_sync_mutex.lock();
		_begin_sync_holder.unlock();
	}


	void List::ListImpl::critical_section_end_ok()
	{
		_begin_sync_holder.lock();
		_end_sync_mutex.unlock();
	}


	void List::ListImpl::insert_rows_begin_ok()
	{
		_end_sync_mutex.lock();
		_begin_sync_holder.unlock();
	}


	void List::ListImpl::insert_rows_end_ok()
	{
		_begin_sync_holder.lock();
		_end_sync_mutex.unlock();
	}


	void List::ListImpl::remove_rows_begin_ok()
	{
		_end_sync_mutex.lock();
		_begin_sync_holder.unlock();
	}


	void List::ListImpl::remove_rows_end_ok()
	{
		_begin_sync_holder.lock();
		_end_sync_mutex.unlock();
	}


	std::string List::ListImpl::get_row( int index ) const
	{
		std::lock_guard<LockedMutex> lock( _begin_sync_holder );
		return _model.get_row( index );
	}


	int List::ListImpl::get_row_count() const
	{
		std::lock_guard<LockedMutex> lock( _begin_sync_holder );
		return _model.get_row_count();
	}


	List::List(  Callback* callback  )
		: Log( "List" )
		, _impl( new ListImpl( callback ) )
	{}


	List::~List()
	{
		delete _impl;
	}


	void List::critical_section_begin_ok()
	{
		_impl->critical_section_begin_ok();
	}


	void List::critical_section_end_ok()
	{
		_impl->critical_section_end_ok();
	}


	void List::insert_rows_begin_ok()
	{
		_impl->insert_rows_begin_ok();
	}


	void List::insert_rows_end_ok()
	{
		_impl->insert_rows_end_ok();
	}


	void List::remove_rows_begin_ok()
	{
		_impl->remove_rows_begin_ok();
	}


	void List::remove_rows_end_ok()
	{
		_impl->remove_rows_end_ok();
	}


	std::string List::get_row( int index ) const
	{
		log() << __FUNCTION__ << ": index " << index << std::endl;
		return _impl->get_row( index );
	}


	int List::get_row_count() const
	{
		int result = _impl->get_row_count();

		log() << __FUNCTION__ << ": count " << result << std::endl;

		return result;
	}

} // namespace Tral
