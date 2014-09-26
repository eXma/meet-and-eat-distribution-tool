#ifndef TEST_FIRSTROUND_TEAM_SELECTION_H
#define TEST_FIRSTROUND_TEAM_SELECTION_H

#include <cxxtest/TestSuite.h>

#include "firstround_select.h"
#include "config.h"


class TestFirstroundTeamSelection : public CxxTest::TestSuite
{
	public:
		void testDistanceSorting(void)
		{
			std::cout << std::endl;
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

			mue::Firstround_team_selection selection(matrix);
			std::vector<mue::Team_id> sorted(selection.for_host(0));
			std::vector<mue::Team_id> reference = {4, 8, 9, 10, 11, 5, 6, 7};

			TS_ASSERT_EQUALS(sorted.size(), reference.size());

			for (size_t i = 0; i < reference.size(); ++i)
			{
				TS_ASSERT_EQUALS(sorted[i], reference[i]);
			}
		}
};

#endif
