#ifndef TEST_DISTANCES_H
#define TEST_HISTANCES_H

#include <cxxtest/TestSuite.h>

#include "distances.h"
#include "config.h"


class TestDistances : public CxxTest::TestSuite
{
	public:
		void testDistanceMatrix(void)
		{
			mue::Distance_matrix distances(MAX_TEAMS);

			int cnt(0);
			for (int i = 0; i < MAX_TEAMS; ++i) {
				for (int j = i; j < MAX_TEAMS; ++j) {
					distances.set_cost(mue::Team(i), mue::Team(j), cnt++);
				}
			}

			cnt = 0;
			for (int i = 0; i < MAX_TEAMS; ++i) {
				for (int j = i; j < MAX_TEAMS; ++j) {
					TS_ASSERT_EQUALS(distances.lookup(mue::Team(i), mue::Team(j)), cnt++)
				}
			}

		}

};

#endif
