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


#ifndef __TRAL_LIST_H__
#define __TRAL_LIST_H__


#include <string>


namespace Tral
{

	class List
	{
	public:
		List();
		~List();

		std::string get_string() const;

	private:
		class ListImpl;

		ListImpl* _impl;
	};

} // namespace Tral


#endif // __TRAL_LIST_H__
