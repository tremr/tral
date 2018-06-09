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
#include "src/IndexedContainer.h"
#include "src/DataSource.h"


namespace Tral
{

	class List::ListImpl
	{
	public:
		ListImpl() : _data_source(), _conteiner( &_data_source ) {}

		std::string get_row( int index ) const;
		int         get_row_count() const;

	private:
		DataSource       _data_source;
		IndexedContainer _conteiner;
	};


	std::string List::ListImpl::get_row( int index ) const
	{
		return "<font style='color:#00A900;background-color:#FF0000'>Mar 26 06:52:59 <font style='color:#6495ED;background-color:#FFA500'>marked fragment</font> 2639.118 1677 filtered fragment</font>";
	}


	int List::ListImpl::get_row_count() const
	{
		return 3;
	}


	List::List()
		: _impl( new ListImpl )
	{}


	List::~List()
	{
		delete _impl;
	}


	std::string List::get_row( int index ) const
	{
		return _impl->get_row( index );
	}


	int List::get_row_count() const
	{
		return _impl->get_row_count();
	}


} // namespace Tral
