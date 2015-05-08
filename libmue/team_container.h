#ifndef MUE__TEAM_CONTAINER
#define MUE__TEAM_CONTAINER

#include <array>
#include <vector>

#include <string.h>

#include <boost/assert.hpp>

#include "config.h"

namespace mue {

template <typename T, std::size_t NUM>
class Limited_array
{
	private:
		using _Value_type = T;
		using _Array = std::array<_Value_type, NUM>;

		std::size_t _len;
		_Array _array;

	public:
		Limited_array() : _len(0) { }

		Limited_array(std::size_t len) : _len(len) { BOOST_ASSERT( _len <= NUM); }

		Limited_array(Limited_array<_Value_type, NUM> const &other)
		:
			_len(other._len),
			_array(other._array)
		{ }

		Limited_array(std::size_t len, const _Value_type &template_value)
		:
			_len(len)
		{
			for (std::size_t idx = 0; idx < _len; ++idx)
				_array[idx++] = template_value;
		}

		Limited_array(std::initializer_list<_Value_type> const &values)
		:
			_len(values.size())
		{
			BOOST_ASSERT( _len <= NUM );

			std::size_t idx(0);
			for (const _Value_type& value : values)
				_array[idx++] = value;
		}

		Limited_array(std::vector<_Value_type> const &other)
		:
			_len(other.size())
		{
			BOOST_ASSERT( _len <= NUM );
			memcpy(_array.data(), other.data(), sizeof(_Value_type) * _len);
		}

		std::size_t size() const noexcept { return _len; }

		typedef typename _Array::iterator iterator;
		typedef typename _Array::const_iterator const_iterator;
		typedef typename _Array::value_type value_type;

		const_iterator begin() const noexcept { return _array.begin(); }
		iterator begin() noexcept { return _array.begin(); }

		const_iterator end() const noexcept { return _array.begin() + _len; }
		iterator end() noexcept { return _array.begin() + _len; }

		value_type& operator[](std::size_t elem) noexcept
		{
			BOOST_ASSERT( elem < _len );
			return _array[elem];
		}

		constexpr
		const value_type& operator[](std::size_t elem) const noexcept
		{
			BOOST_ASSERT( elem < _len );
			return _array[elem];
		}
};

typedef Limited_array<Team_id, MAX_TEAMS / 3>       Hosts;
typedef Limited_array<Team_id, (MAX_TEAMS / 3) * 2> Guests;
typedef Limited_array<Team_id, MAX_TEAMS>           Teams;

}

#endif
