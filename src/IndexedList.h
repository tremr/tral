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


#ifndef __TRAL_INDEXEDLIST_H__
#define __TRAL_INDEXEDLIST_H__


#include "src/IndexedString.h"
#include <list>
#include <cassert>


namespace Tral
{

	class IndexedList
	{
		struct Node
		{
			typedef std::list<Node> base_list;

			Node( base_list& owner, IndexedString&& value ) : prev( owner.end() ), next( owner.end() ), value( std::move( value ) ) {}

			bool is_visible( base_list& owner, base_list::const_iterator first_visible ) const { return prev != owner.end() || next != owner.end() || this == &*first_visible; }

			mutable base_list::iterator prev;
			mutable base_list::iterator next;
			IndexedString               value;
		};

		typedef Node::base_list base_list;

	public:
		typedef base_list::size_type size_type;

		class visible_iterator;
		class iterator
		{
			iterator( base_list::iterator const& iterator ) : _iterator( iterator ) {}

		public:
			iterator( visible_iterator const& iterator );
			IndexedString& operator*() const  { return _iterator->value; }
			IndexedString* operator->() const { return &_iterator->value; }
			iterator&      operator++()       { ++_iterator; return *this; }
			iterator       operator++( int )  { iterator tmp = *this; _iterator++; return tmp; }
			iterator&      operator--()       { --_iterator; return *this; }
			iterator       operator--( int )  { iterator tmp = *this; _iterator--; return tmp; }
			bool           operator==( iterator const& other ) const { return _iterator == other._iterator; }
			bool           operator!=( iterator const& other ) const { return !operator==( other ); }

		private:
			friend class IndexedList;

			base_list::iterator to_list_iterator() { return _iterator; }
			base_list::const_iterator to_list_iterator() const { return _iterator; }

			base_list::iterator _iterator;
		};

		class const_visible_iterator;
		class const_iterator
		{
			const_iterator( base_list::iterator const& iterator ) : _iterator( iterator ) {}
			const_iterator( base_list::const_iterator const& iterator ) : _iterator( iterator ) {}

		public:
			const_iterator( iterator const& iterator ) : _iterator( iterator.to_list_iterator() ) {}
			const_iterator( visible_iterator const& iterator );
			const_iterator( const_visible_iterator const& iterator );
			IndexedString const& operator*() const        { return _iterator->value; }
			IndexedString const* operator->() const       { return &_iterator->value; }
			const_iterator&      operator++()             { ++_iterator; return *this; }
			const_iterator       operator++( int )        { const_iterator tmp = *this; _iterator++; return tmp; }
			const_iterator&      operator--()             { --_iterator; return *this; }
			const_iterator       operator--( int )        { const_iterator tmp = *this; _iterator--; return tmp; }
			bool                 operator==( const_iterator const& other ) const { return _iterator == other._iterator; }
			bool                 operator!=( const_iterator const& other ) const { return !operator==( other ); }

		private:
			friend class IndexedList;

			base_list::const_iterator to_list_iterator() const { return _iterator; }

			base_list::const_iterator _iterator;
		};

		class visible_iterator
		{
			visible_iterator( base_list::iterator it ) : _iterator( it ) {}
			visible_iterator( iterator it ) : _iterator( it._iterator ) {}

		public:
			visible_iterator() : _iterator() {}
			IndexedString&    operator*() const  { return _iterator->value; }
			IndexedString*    operator->() const { return &_iterator->value; }
			visible_iterator& operator++()       { _iterator = _iterator->next; return *this; }
			visible_iterator  operator++( int )  { visible_iterator tmp = *this; _iterator = _iterator->next; return tmp; }
			visible_iterator& operator--()       { _iterator = _iterator->prev; return *this; }
			visible_iterator  operator--( int )  { visible_iterator tmp = *this; _iterator = _iterator->prev; return tmp; }
			iterator          base() const       { return _iterator; }
			bool              operator==( visible_iterator const& other ) const { return _iterator == other._iterator; }
			bool              operator!=( visible_iterator const& other ) const { return !operator==( other ); }

		private:
			friend class IndexedList;
			friend class const_visible_iterator;

			base_list::iterator _iterator;
		};

		class const_visible_iterator
		{
			const_visible_iterator( base_list::iterator const& it ) : _iterator( it ) {}
			const_visible_iterator( iterator const& iterator ) : _iterator( iterator.to_list_iterator() ) {}
			const_visible_iterator( base_list::const_iterator const& it ) : _iterator( it ) {}

		public:
			const_visible_iterator() : _iterator() {}
			const_visible_iterator( visible_iterator const& iterator ) : _iterator( iterator._iterator ) {}
			IndexedString const&    operator*() const  { return _iterator->value; }
			IndexedString const*    operator->() const { return &_iterator->value; }
			const_visible_iterator& operator++()       { _iterator = _iterator->next; return *this; }
			const_visible_iterator  operator++( int )  { const_visible_iterator tmp = *this; _iterator = _iterator->next; return tmp; }
			const_visible_iterator& operator--()       { _iterator = _iterator->prev; return *this; }
			const_visible_iterator  operator--( int )  { const_visible_iterator tmp = *this; _iterator = _iterator->prev; return tmp; }
			const_iterator          base() const       { return _iterator; }
			bool                    operator==( const_visible_iterator const& other ) const { return _iterator == other._iterator; }
			bool                    operator!=( const_visible_iterator const& other ) const { return !operator==( other ); }

		private:
			friend class IndexedList;

			base_list::const_iterator _iterator;
		};

		IndexedList();
		~IndexedList();

		bool           empty() const;
		iterator       begin();
		const_iterator begin() const;
		iterator       end();
		const_iterator end() const;
		size_type      size() const;
		void           clear();
//		void           push_back( const IndexedString& value ); // IndexedString copy constructor forbidden.
		void           push_back( IndexedString&& value );
		iterator       erase( iterator pos );
		iterator       insert( const_iterator pos, IndexedString&& value );
		iterator       emplace( const_iterator pos, unsigned offset, std::string&& value );

		bool                   visible_empty() const;
		visible_iterator       visible_begin();
		const_visible_iterator visible_begin() const;
		visible_iterator       visible_end();
		const_visible_iterator visible_end() const;
		size_type              visible_size() const;
		visible_iterator       erase_visible( const_visible_iterator pos );
//		void                   push_back_as_visible( const_iterator nearest_visible, const IndexedString& value ); // IndexedString copy constructor forbidden.
		void                   push_back_as_visible( const_visible_iterator nearest_visible, IndexedString&& value );
		visible_iterator       visible_enable( const_visible_iterator nearest_visible, iterator pos );
		visible_iterator       visible_disable( const_visible_iterator pos );

	private:
		void assert_offsets( base_list::const_iterator previous, base_list::iterator current ) const;
		void assert_visibility() const;

		base_list _string_list;
		size_type _visible_count;
		base_list::iterator _first_visible;
		base_list::iterator _last_visible;
	};

} // namespace Tral

#endif // __TRAL_INDEXEDLIST_H__
