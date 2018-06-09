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

namespace Tral
{

	VisibleRowsModel::VisibleRowsModel( DataSource* data_source )
		: _row_count( 0 )
		, _begin( nullptr )
		, _end( nullptr )
		, _cached_rows( DefaultCacheSize, nullptr )
		, _cached_rows_first_index( 0 )
		, _conteiner( data_source )
	{
		_row_count = 3; // debug value
	}


	VisibleRowsModel::~VisibleRowsModel()
	{
	}


	std::string VisibleRowsModel::get_row( int index ) const
	{
		return "<font style='color:#00A900;background-color:#FF0000'>Mar 26 06:52:59 <font style='color:#6495ED;background-color:#FFA500'>marked fragment</font> 2639.118 1677 filtered fragment</font>";
	}


	int VisibleRowsModel::get_row_count() const
	{
		return _row_count;
	}

} // namespace Tral
