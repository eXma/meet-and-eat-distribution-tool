#ifndef TEST_FIRSTROUND_TEAM_SELECTION_H
#define TEST_FIRSTROUND_TEAM_SELECTION_H

#include <cxxtest/TestSuite.h>

#include "firstround_select.h"
#include "config.h"
#include "guest_tuple_iterator.h"

class TestFirstroundTeamSelection : public CxxTest::TestSuite
{
	private:
		mue::Distance_matrix make_matrix()
		{
			std::vector<std::vector<mue::Distance> > distances = {
				{0, 0, 0, 0, 1, 6, 11, 16, 0, 0, 0, 0},
				{0, 0, 0, 0, 1, 6, 11, 16, 0, 0, 0, 0},
				{0, 0, 0, 0, 1, 6, 11, 16, 0, 0, 0, 0},
				{0, 0, 0, 0, 1, 6, 11, 16, 0, 0, 0, 0},
				{1, 1, 1, 1, 0, 0, 0, 0, 1, 2, 3, 4},
				{6, 6, 6, 6, 0, 0, 0, 0, 1, 2, 3, 4},
				{11, 11, 11, 11, 0, 0, 0, 0, 1, 2, 3, 4},
				{16, 16, 16, 16, 0, 0, 0, 0, 1, 2, 3, 4},
				{0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
				{0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0},
				{0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0},
				{0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0}
			};

			mue::Distance_matrix matrix(distances.size());
			for (size_t src = 0; src < distances.size(); ++src) {
				for (size_t dst = src + 1; dst < distances.size(); ++dst) {
					matrix.set_cost(src, dst, distances[src][dst]);
				}
			}
			return matrix;
		}
	public:
		void testDistanceSorting(void)
		{
			mue::Firstround_team_selection selection(make_matrix(), 10000000);
			std::vector<mue::Team_id> sorted(selection.for_host(0));
			std::vector<mue::Team_id> reference = {4, 8, 9, 10, 11, 5, 6, 7};

			TS_ASSERT_EQUALS(sorted.size(), reference.size());

			for (size_t i = 0; i < reference.size(); ++i)
			{
				TS_ASSERT_EQUALS(sorted[i], reference[i]);
			}
		}

		void testSortedGuestTupleGeneration(void)
		{
			mue::Firstround_team_selection selection(make_matrix(), 1000000, 4);
			std::vector<mue::Team_id> sorted(selection.for_host(0));

			mue::Guest_tuple_generator generator(sorted, std::bitset<MAX_TEAMS>(0x00));

			using guests = mue::Guest_tuple_generator::GuestPair;

			std::vector<guests> reference = {guests(4, 8),
							 guests(4, 9),
							 guests(4, 10),
							 guests(4, 5),
							 guests(4, 6),
							 guests(4, 7),
							 guests(4, 11),
							 guests(8, 9),
							 guests(8, 10),
							 guests(8, 5),
							 guests(8, 6),
							 guests(8, 7),
							 guests(8, 11),
							 guests(9, 10),
							 guests(9, 5),
							 guests(9, 6),
							 guests(9, 7),
							 guests(9, 11),
							 guests(10, 5),
							 guests(10, 6),
							 guests(10, 7),
							 guests(10, 11),
							 guests(5, 6),
							 guests(5, 7),
							 guests(5, 11),
							 guests(6, 7),
							 guests(6, 11),
							 guests(7, 11)};


			auto it = reference.begin();
			for (guests const &g : generator) {
				TS_ASSERT(it != reference.end());
				TS_ASSERT_EQUALS(it->first, g.first);
				TS_ASSERT_EQUALS(it->second, g.second);
				++it;
			}
		}
};

#endif
