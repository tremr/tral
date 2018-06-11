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


#ifndef TRAL_SRC_CACHEDCONTAINER_H_
#define TRAL_SRC_CACHEDCONTAINER_H_


#include "src/IndexedContainer.h"


namespace Tral
{

	class CachedContainer
	{
	public:
		CachedContainer( DataSource* data_source );
		~CachedContainer();

		void move_cached_rows( int index );

	private:
		enum { DefaultCacheSize = 500 };

		IndexedContainer _conteiner;
		std::vector<IndexedString const*> _cached_rows;
		int                               _cached_rows_first_index;
	};

} // namespace Tral


#endif /* TRAL_SRC_CACHEDCONTAINER_H_ */
