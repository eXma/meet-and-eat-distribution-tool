#ifndef MUE__GUEST_TUPLE_GENERATOR
#define MUE__GUEST_TUPLE_GENERATOR


#include <unordered_set>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>

#include "config.h"


namespace mue {

class Team;


class Guest_tuple_generator
{
	public:
		typedef std::pair<Team*, Team*> GuestPair;

	private:
		typedef std::vector<Team* >::const_iterator  Team_iterator;

		std::vector<Team* > _possible_guests;
		Team_iterator _guest_end;

	public:
		class Iterator  : public std::iterator<std::input_iterator_tag, GuestPair> {
			private:
				friend class Guest_tuple_generator;

				Team_iterator        _guest_a;
				Team_iterator        _guest_b;
				Team_iterator const& _end;

				Iterator(Team_iterator const& start, Team_iterator const& end)
				:
					_guest_a(start),
					_guest_b(start == end ? end : start + 1),
					_end(end)
				{}

			public:

				Iterator(Iterator const& other)
				: _guest_a(other._guest_a), _guest_b(other._guest_b), _end(other._end)
				{}

				inline Iterator& operator++()
				{
					assert(_guest_a != _end);
					assert(_guest_b != _end);

					++_guest_b;
					if (_guest_b == _end) {
						++_guest_a;
						if (_guest_a == _end)
							_guest_b = _end;
						else {
							_guest_b = _guest_a + 1;
							if (_guest_b == _end)
								_guest_a = _end;
						}
					}
					return *this;
				}

				inline Iterator operator++(int)
				{
					Iterator tmp(*this);
					++(*this);
					return tmp;
				}

				inline bool operator== (Iterator const& other) const { return (_guest_a == other._guest_a) && (_guest_b == other._guest_b); }
				inline bool operator!= (Iterator const& other) const { return !(*this == other); }

				inline GuestPair operator* ()
				{
					assert(_guest_a != _end);
					assert(_guest_b != _end);
					return GuestPair(*_guest_a, *_guest_b);
				}

		};

		typedef Iterator iterator;

		Guest_tuple_generator(std::vector<Team*> const& guests, std::unordered_set<Team_id> const& used);

		iterator begin() { return Iterator(_possible_guests.begin(), _guest_end); }

		iterator end() { return Iterator(_guest_end, _guest_end); }
};

std::ostream& operator<< (std::ostream& os, Guest_tuple_generator::GuestPair const& p);
}

#endif