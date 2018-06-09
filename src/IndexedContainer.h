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


#ifndef __TRAL_INDEXEDCONTAINER_H__
#define __TRAL_INDEXEDCONTAINER_H__


#include "src/IndexedString.h"
#include <list>


namespace Tral
{
	class DataSource;

	class IndexedContainer
	{
	public:
		IndexedContainer( DataSource* data_source );
		~IndexedContainer();

	private:
		typedef std::list<IndexedString> StringList;

		DataSource* const _data_source;
		StringList        _string_list;
	};

} // namespace Tral

#endif // __TRAL_INDEXEDCONTAINER_H__