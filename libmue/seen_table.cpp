#include "seen_table.h"

void mue::Seen_table::_update_table(Team_id table_idx, Team_id team_a, Team_id team_b) noexcept {
	if (_table[table_idx]->generation < _generation) {
		 Seen_data * new_data = new Seen_data(*_table[table_idx]);
		 _allocated_lines.push_back(new_data);
		 _table[table_idx] = new_data;
	}
	_table[table_idx]->bitset |= (_id_bitsets[team_a] | _id_bitsets[team_b]);
}

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
	_table(std::move(other._table)),
	_allocated_lines(std::move(other._allocated_lines))
{ }


mue::Seen_table::Seen_table(int max_teams) noexcept
:
	_generation(0),
	_max_teams(max_teams),
	_table(max_teams, 0)
{
	for (int i = 0; i < max_teams; ++i) {
		Seen_data * new_data = new Seen_data(0);
		_allocated_lines.push_back(new_data);
		_table[i] = new_data;
	}

	if (_id_bitsets.empty()) {
		for (int i = 0; i < MAX_TEAMS; ++i) {
			_id_bitsets.push_back(0x01LL << i);
		}
	}
}


mue::Seen_table::~Seen_table()
{
	for (Seen_data * data : _allocated_lines)
		delete data;
}

std::vector<mue::Seen_table::Seen_data::Seen_bitset> mue::Seen_table::_id_bitsets;
