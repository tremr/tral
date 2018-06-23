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


#include "src/DataSource.h"


namespace Tral
{

	DataSource::DataSource()
	{
		for (size_t i = 0; i < 10000; ++i)
		{
			_data.push_back( std::string("-") + std::to_string( i ) + "-" );
		}
	}


	DataSource::~DataSource()
	{
	}


	unsigned DataSource::get_string( unsigned offset, std::string& out_string ) const
	{
		unsigned next_offset = 0;
		if (offset < _data.size())
		{
			out_string = _data.at( offset );
			next_offset = offset + 1;
		}
		else
		{
			out_string = "";
		}

//		out_string = "<font style='color:#00A900;background-color:#FF0000'>Mar 26 06:52:59 <font style='color:#6495ED;background-color:#FFA500'>marked fragment</font> 2639.118 1677 filtered fragment</font>";
		return next_offset < _data.size() ? next_offset : 0;
	}


	unsigned DataSource::get_size() const
	{
		return _data.size();
	}

} // namespace Tral
