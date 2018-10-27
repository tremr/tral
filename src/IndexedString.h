// Copyright 2018 Roman Tremaskin. All Rights Reserved.

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



#ifndef __TRAL_INDEXEDSTRING_H__
#define __TRAL_INDEXEDSTRING_H__


#include <vector>
#include <string>


namespace Tral
{

	class IndexedString
	{
	public:
		IndexedString( unsigned offset, std::string value );
		IndexedString( IndexedString&& other );
		~IndexedString();

		std::string get_value() const;
		unsigned    get_offset() const { return _offset; }

	private:
		typedef int FilterLink;
		typedef std::vector<FilterLink> FilterLinkList;

		IndexedString( IndexedString const& other );

		unsigned       _offset;
		std::string    _value;
		FilterLinkList _filter_links;
	};

} // namespace Tral

#endif // __TRAL_INDEXEDSTRING_H__
