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


#ifndef __TRAL_LIST_H__
#define __TRAL_LIST_H__


#include <string>


namespace Tral
{

	class Log
	{
	public:
		Log( std::string const& name ) : _name( name ) {}

		std::ostream& log() const;

	private:
		std::string const _name;
	};


	class Callback
	{
	public:
		virtual void on_insert_rows_begin( unsigned first, unsigned last ) = 0;
		virtual void on_insert_rows_end( unsigned first, unsigned last ) = 0;
		virtual void on_remove_rows_begin( unsigned first, unsigned last ) = 0;
		virtual void on_remove_rows_end( unsigned first, unsigned last ) = 0;

	protected:
		virtual ~Callback() {}
	};


	class List : private Log
	{
	public:
		List( Callback* callback );
		~List();

		// The internal thread waits for the corresponding function to be called after each callback.
		void insert_rows_begin_ok();
		void insert_rows_end_ok();
		void remove_rows_begin_ok();
		void remove_rows_end_ok();

		std::string get_row( int index ) const;
		int         get_row_count() const;

	private:
		class ListImpl;

		ListImpl* _impl;
	};

} // namespace Tral


#endif // __TRAL_LIST_H__
