//
// $Id$
//
// Created by Jlobson 6 окт. 2018 г.
//
// Copyright (C) 2018 Zodiac Interactive, LLC
//
#include <src/IndexedList.h>

namespace Tral
{

	IndexedList::iterator::iterator( visible_iterator const& iterator )
		: _iterator( iterator.base().to_list_iterator() )
	{}


	IndexedList::const_iterator::const_iterator( visible_iterator const& iterator )
		: _iterator( iterator.base().to_list_iterator() )
	{}


	IndexedList::const_iterator::const_iterator( const_visible_iterator const& iterator )
		: _iterator( iterator.base().to_list_iterator() )
	{}

	IndexedList::IndexedList()
		: _visible_count( 0 )
		, _first_visible( _string_list.end() )
		, _last_visible( _string_list.end() )
	{}


	IndexedList::~IndexedList()
	{}


	bool IndexedList::empty() const
	{
		return _string_list.empty();
	}


	IndexedList::iterator IndexedList::begin()
	{
		return _string_list.begin();
	}


	IndexedList::const_iterator IndexedList::begin() const
	{
		return _string_list.begin();
	}


	IndexedList::iterator IndexedList::end()
	{
		return _string_list.end();
	}


	IndexedList::const_iterator IndexedList::end() const
	{
		return _string_list.end();
	}


	IndexedList::size_type IndexedList::size() const
	{
		return _string_list.size();
	}


	void IndexedList::clear()
	{
		_string_list.clear();
	}


	// IndexedString copy constructor forbidden.
//	void IndexedList::push_back( const IndexedString& value )
//	{
//		_string_list.push_back( value );
//	}


	void IndexedList::push_back( IndexedString&& value )
	{
		base_list::iterator it = _string_list.insert( _string_list.end(), Node( _string_list, std::move( value ) ) );

		assert( it == _string_list.begin() || it->value.get_offset() > (--it)->value.get_offset() );
	}


	IndexedList::iterator IndexedList::erase( iterator pos )
	{
		return erase_visible( const_visible_iterator( pos ) );
	}


	IndexedList::iterator IndexedList::insert( const_iterator pos, IndexedString&& value )
	{
		base_list::iterator it = _string_list.emplace( pos.to_list_iterator(), Node( _string_list, std::move( value ) ) );

		assert_offsets( pos.to_list_iterator(), it );
		return it;
	}


	IndexedList::iterator IndexedList::emplace( const_iterator pos, unsigned offset, std::string&& value )
	{
		base_list::iterator it = _string_list.emplace( pos.to_list_iterator(), Node( _string_list, IndexedString( offset, value ) ) );

		assert_offsets( pos.to_list_iterator(), it );
		return it;
	}


	bool IndexedList::visible_empty() const
	{
		return visible_size() == 0;
	}


	IndexedList::visible_iterator IndexedList::visible_begin()
	{
		return _first_visible;
	}


	IndexedList::const_visible_iterator IndexedList::visible_begin() const
	{
		return const_visible_iterator( _first_visible );
	}


	IndexedList::visible_iterator IndexedList::visible_end()
	{
		return visible_iterator( _string_list.end() );
	}


	IndexedList::const_visible_iterator IndexedList::visible_end() const
	{
		return const_visible_iterator( _string_list.end() );
	}


	IndexedList::size_type IndexedList::visible_size() const
	{
		assert( _visible_count != 0 || ( _first_visible == _string_list.end() && _last_visible == _string_list.end() ) );
		return _visible_count;
	}


	IndexedList::visible_iterator IndexedList::erase_visible( const_visible_iterator pos )
	{
		visible_iterator it = visible_disable( pos );
		_string_list.erase( pos.base().to_list_iterator() );
		return it;
	}


	// IndexedString copy constructor forbidden.
//	void IndexedList::push_back_as_visible( const_iterator nearest_visible, const IndexedString& value )
//	{
//	}


	void IndexedList::push_back_as_visible( const_visible_iterator nearest_visible, IndexedString&& value )
	{
		push_back( std::move( value ) );
		visible_enable( nearest_visible, --_string_list.end() );
	}


	IndexedList::visible_iterator IndexedList::visible_enable( const_visible_iterator nearest_visible, iterator pos )
	{
		if (pos.to_list_iterator()->is_visible( _string_list, _first_visible ))
			return visible_iterator( pos );

		if (visible_empty())
		{
			_first_visible = pos.to_list_iterator()._M_const_cast();
			_last_visible  = _first_visible;
			_visible_count++;
			assert_visibility();
			return _first_visible;
		}

		assert( nearest_visible != pos );

		if (nearest_visible == visible_end())
			nearest_visible = visible_begin();

		unsigned const            pos_offset           = pos->get_offset();
		base_list::const_iterator pos_node             = pos.to_list_iterator();
		base_list::const_iterator nearest_visible_node = nearest_visible.base().to_list_iterator();

		assert( nearest_visible_node != _string_list.end() );
		assert( nearest_visible->get_offset() != pos_offset );
		if (nearest_visible->get_offset() < pos_offset)
		{
			while (nearest_visible_node->next != _string_list.end() || nearest_visible_node->next->value.get_offset() < pos_offset)
				++nearest_visible_node;

			assert( nearest_visible_node->next != pos_node );
			assert( nearest_visible_node->next != _string_list.end() || nearest_visible_node == _last_visible );

			pos_node->next                   = nearest_visible_node->next;
			pos_node->prev                   = nearest_visible_node._M_const_cast();
			nearest_visible_node->next       = pos_node._M_const_cast();

			if (pos_node->next == _string_list.end()) _last_visible        = pos_node._M_const_cast();
			else                                      pos_node->next->prev = pos_node._M_const_cast();
		}
		else
		{
			while (nearest_visible_node->prev != _string_list.end() || nearest_visible_node->prev->value.get_offset() > pos_offset)
				--nearest_visible_node;

			assert( nearest_visible_node->prev != pos_node );
			assert( nearest_visible_node->prev != _string_list.end() || nearest_visible_node == _first_visible );

			pos_node->next                   = nearest_visible_node._M_const_cast();
			pos_node->prev                   = nearest_visible_node->prev;
			nearest_visible_node->prev       = pos_node._M_const_cast();

			if (pos_node->prev == _string_list.end()) _first_visible       = pos_node._M_const_cast();
			else                                      pos_node->prev->next = pos_node._M_const_cast();
		}

		assert( pos_node->next == _string_list.end() || pos_node->next->prev == pos_node );
		assert( pos_node->prev == _string_list.end() || pos_node->prev->next == pos_node );

		_visible_count++;
		assert_visibility();
		return visible_iterator( pos );
	}


	IndexedList::visible_iterator IndexedList::visible_disable( const_visible_iterator pos )
	{
		assert( pos != _string_list.end() );

		base_list::const_iterator node      = pos.base().to_list_iterator();
		base_list::iterator       prev_node = node->prev;
		base_list::iterator       next_node = node->next;
		assert( node->is_visible( _string_list, _first_visible ) );

		if (prev_node != _string_list.end()) prev_node->next = next_node;
		else                                 _first_visible  = next_node;

		if (next_node != _string_list.end()) next_node->prev = prev_node;
		else                                 _last_visible   = prev_node;

		assert( _visible_count != 0 );
		_visible_count--;

		assert_visibility();
		return visible_iterator( prev_node != _string_list.end() ? prev_node : _first_visible );
	}


	void IndexedList::assert_offsets( base_list::const_iterator previous, base_list::iterator current ) const
	{
		assert( [this]( base_list::const_iterator next_it, base_list::iterator it )->bool
			{
				base_list::const_iterator prev_it = it;

				bool const prev_check = it == _string_list.begin() || it->value.get_offset() > (--prev_it)->value.get_offset();
				bool const next_check = next_it == _string_list.end() || next_it->value.get_offset() > it->value.get_offset();

				return prev_check && next_check;
			}
			( previous, current ) );
	}


	void IndexedList::assert_visibility() const
	{
		assert( _visible_count != 0 || (_first_visible == _string_list.end() && _last_visible == _string_list.end() ) );
		assert( _visible_count == 0 || (_first_visible != _string_list.end() && _last_visible != _string_list.end() ) );
		assert( _visible_count != 1 || (_first_visible != _string_list.end() && _last_visible == _first_visible ) );
	}

} /* namespace Tral */
