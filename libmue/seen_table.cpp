#include "seen_table.h"

#include <boost/assert.hpp>
#include <string.h>


void mue::Seen_table::add_meeting(Team_id id_1, Team_id id_2, Team_id id_3) noexcept
{
	_update_table(id_1, id_2, id_3);
	_update_table(id_2, id_3, id_1);
	_update_table(id_3, id_1, id_2);
}


mue::Seen_table::Seen_table(Seen_table const &old, int new_generation) noexcept
:
	_generation(new_generation),
	_max_teams(old._max_teams),
	_table(old._table)
{ }


mue::Seen_table::Seen_table(Seen_table&& other) noexcept
:
	_generation(std::move(other._generation)),
	_max_teams(std::move(other._max_teams)),
	_table(std::move(other._table))
{ }


mue::Seen_table::Seen_table(int max_teams) noexcept
:
	_generation(0),
	_max_teams(max_teams)
{
	BOOST_ASSERT(max_teams <= MAX_TEAMS);

	for (int i = 0; i < max_teams; ++i) {
		_table[i].reset();
	}

	if (_id_bitsets.empty()) {
		for (int i = 0; i < MAX_TEAMS; ++i) {
			_id_bitsets.push_back(0x01LL << i);
		}
	}
}


std::vector<mue::Seen_table::Seen_bitset> mue::Seen_table::_id_bitsets;
