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
#include "include/List.h"
#include <list>
#include <shared_mutex>
#include <thread>


namespace Tral
{
	class DataSource;
	class Callback;

	class IndexedContainer : private Log
	{
	private:
		typedef std::list<IndexedString> StringList;

	public:
		typedef StringList::const_iterator ConstIterator;

		IndexedContainer( DataSource* data_source,  Callback* callback );
		~IndexedContainer();

		ConstIterator begin();
		ConstIterator invalid_iterator() const;
		ConstIterator get_previous( ConstIterator string );
		ConstIterator get_next( ConstIterator string );
		unsigned      get_size() const;
		void          reload( ConstIterator first_cached, ConstIterator last_cached );

	private:
		typedef std::shared_timed_mutex       SharedMutex;
		typedef std::shared_lock<SharedMutex> ReadOnlyLock;
		typedef std::unique_lock<SharedMutex> ReadWriteLock;

		void reload_thread_function();

		DataSource* const   _data_source;
		StringList          _string_list;
		mutable SharedMutex _mutex;
		std::thread         _thread;
		Callback*           _callback;
	};

} // namespace Tral

#endif // __TRAL_INDEXEDCONTAINER_H__
