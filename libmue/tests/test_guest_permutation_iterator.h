#ifndef TEST_GUEST_PERMUTATION_ITERATOR_H
#define TEST_GUEST_PERMUTATION_ITERATOR_H

#include <cxxtest/TestSuite.h>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <algorithm>

#include "config.h"

class TestGuestTupleGenerator : public CxxTest::TestSuite
{
public:
	void testNaiveGenerationConcept(void)
	{
		mue::Team_id i = 0;

		std::vector<mue::Team_id> teams(6);
		std::generate(teams.begin(), teams.end(), [&](){ return i++; });
		unsigned int all(0), use(0);

		do {
			++all;

			bool valid(true);
			for (auto it = teams.begin(); it != teams.end();) {
				std::pair<mue::Team_id, mue::Team_id> guests(*it, *std::next(it));
				if (guests.first > guests.second) {
					valid = false;
					break;
				}
				std::advance(it, 2);

			}
			if (valid) {
				++use;
			}
		} while (std::next_permutation(teams.begin(), teams.end()));

		TS_ASSERT_EQUALS(all, 720);
		TS_ASSERT_EQUALS(use, 90);
	}
};

#endif
