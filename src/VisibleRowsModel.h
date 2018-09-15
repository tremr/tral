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


#ifndef __TRAL_VISIBLEROWSMODEL_H__
#define __TRAL_VISIBLEROWSMODEL_H__


#include "src/CachedContainer.h"
#include <string>
#include <vector>


namespace Tral
{
	class IndexedString;
	class DataSource;
	class Callback;

	class VisibleRowsModel
	{
	public:
		VisibleRowsModel( DataSource* data_source, Callback* callback );
		~VisibleRowsModel();

		std::string get_row( int index ) const;
		int         get_row_count() const;

	private:
		mutable CachedContainer _cached_container;
		int             _row_count;
		IndexedString*  _begin;
		IndexedString*  _end;
	};

} // namespace Tral

#endif // __TRAL_VISIBLEROWSMODEL_H__
