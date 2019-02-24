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


#include "src/VisibleRowsModel.h"
#include "src/IndexedString.h"
#include "include/list.h"


namespace Tral
{

	VisibleRowsModel::VisibleRowsModel( DataSource* data_source,  Callback* callback )
		: _cached_container( data_source, callback )
		, _row_count( 0 )
		, _begin( nullptr )
		, _end( nullptr )
	{
		_row_count = 3; // debug value
	}


	VisibleRowsModel::~VisibleRowsModel()
	{
	}


	std::string VisibleRowsModel::get_row( int index ) const
	{
		return _cached_container.get_row( index );
	}


	int VisibleRowsModel::get_row_count() const
	{
		return _cached_container.get_row_count();
	}

} // namespace Tral
