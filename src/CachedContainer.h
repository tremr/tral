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


#ifndef __TRAL_CACHEDCONTAINER_H__
#define __TRAL_CACHEDCONTAINER_H__


#include "src/IndexedContainer.h"
#include "include/list.h"


namespace Tral
{

	class Callback;


	class CachedContainer : private Log
	{
	public:
		CachedContainer( DataSource* data_source,  Callback* callback );
		~CachedContainer();

		std::string get_row( int index );
		int         get_row_count() const;
		void        reset();
		bool        is_fit_in_cache( int index ) const;
		int         get_not_cached_row_potential_index( IndexedList::const_iterator indexed_it ) const;
		int         get_cached_row_index( IndexedContainer::ConstIterator& indexed_it ) const;
		void        add_row( int index, IndexedContainer::ConstIterator indexed_it );
		void        remove_row( int index );
		void        remove_row( IndexedContainer::ConstIterator& indexed_it );

		void move_cached_rows( int index );

	private:
		enum
		{
			DefaultUICacheSize    = 320,
			NativeCacheRedundancy = 3,
		};

		IndexedContainer                             _conteiner;
		std::vector<IndexedContainer::ConstIterator> _cached_rows;
		int                                          _cached_rows_first_index;
	};

} // namespace Tral


#endif // __TRAL_CACHEDCONTAINER_H__
