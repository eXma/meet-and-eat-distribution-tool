#ifndef SEEN_TABLE_H
#define SEEN_TABLE_H

#include "config.h"

#include <vector>
#include <list>
#include <bitset>

namespace mue {

class Seen_table
{

	private:
		typedef std::bitset<MAX_TEAMS> Seen_bitset;

		static std::vector<Seen_bitset> _id_bitsets;

		int const _generation;
		int const _max_teams;

		Seen_bitset _table[MAX_TEAMS];

		Seen_table(Seen_table const &old, int new_generation) noexcept;

		inline void _update_table(Team_id table_idx, Team_id team_a, Team_id team_b) noexcept
		{
			_table[table_idx] |= (_id_bitsets[team_a] | _id_bitsets[team_b]);
		}

	public:

		Seen_table( const Seen_table& ) = delete;
		Seen_table& operator=( const Seen_table& ) = delete;
		Seen_table& operator=( const Seen_table&& ) = delete;


		Seen_table(Seen_table&& other) noexcept;
		int generation() const noexcept { return _generation; }


		Seen_table clone() const noexcept { return Seen_table(*this, _generation + 1); }

		bool seen(Team_id id_1, Team_id id_2, Team_id id_3) const noexcept
		{
			return (_table[id_1] & (_id_bitsets[id_2] | _id_bitsets[id_3])).any()
				|| (_table[id_2] & _id_bitsets[id_3]).any();
		}

		void add_meeting(Team_id id_1, Team_id id_2, Team_id id_3) noexcept;

		Seen_table(int max_teams) noexcept;
};

}

#endif
