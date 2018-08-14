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


#include "include/List.h"
#include "src/VisibleRowsModel.h"
#include "src/DataSource.h"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace Tral
{

	class List::ListImpl
	{
	public:
		ListImpl( Callback* callback ) : _data_source(), _model( &_data_source, callback ) {}

		std::string get_row( int index ) const;
		int         get_row_count() const;
		void        lock();
		void        unlock();

	private:
		DataSource       _data_source;
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

	std::string List::ListImpl::get_row( int index ) const
	{
		return _model.get_row( index );
	}


	int List::ListImpl::get_row_count() const
	{
		return _model.get_row_count();
	}


	void List::ListImpl::lock()
	{
		_model.lock();
	}


	void List::ListImpl::unlock()
	{
		_model.unlock();
	}


	List::List(  Callback* callback  )
		: Log( "List" )
		, _impl( new ListImpl( callback ) )
	{}


	List::~List()
	{
		delete _impl;
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


	void List::lock()
	{
		_impl->lock();
	}


	void List::unlock()
	{
		_impl->unlock();
	}

} // namespace Tral
