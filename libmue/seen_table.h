#ifndef SEEN_TABLE_H
#define SEEN_TABLE_H

#include "config.h"

#include <boost/noncopyable.hpp>
#include <vector>
#include <list>
#include <bitset>

namespace mue {

class Seen_table : boost::noncopyable
{

	private:
		struct Seen_data {
			typedef std::bitset<MAX_TEAMS> Seen_bitset;

			int const generation;
			Seen_bitset bitset;

			Seen_data(int generation) : generation(generation), bitset() { }
			Seen_data(Seen_data const &other) : generation(other.generation + 1), bitset(other.bitset) { }
		};

		static std::vector<Seen_data::Seen_bitset> _id_bitsets;

		int const _generation;
		int const _max_teams;

		std::vector<Seen_data *> _table;
		std::list<Seen_data *> _allocated_lines;

		Seen_table(Seen_table const &old, int new_generation);



		void _update_table(Team_id table_idx, Team_id team_a, Team_id team_b);

	public:

		Seen_table(Seen_table&& other);


		Seen_table clone() { return Seen_table(*this, _generation + 1); }

		bool seen(Team_id id_1, Team_id id_2, Team_id id_3) const
		{
			return (_table[id_1]->bitset & (_id_bitsets[id_2] | _id_bitsets[id_3])).any()
				|| (_table[id_2]->bitset & _id_bitsets[id_3]).any();
		}

		void add_meeting(Team_id id_1, Team_id id_2, Team_id id_3);

		Seen_table(int max_teams);

		~Seen_table();
};

}

#endif
