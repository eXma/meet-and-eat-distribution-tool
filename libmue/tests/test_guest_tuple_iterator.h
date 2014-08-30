#ifndef TEST_GUEST_TUPLE_ITERATOR_H
#define TEST_GUEST_TUPLE_ITERATOR_H

#include <cxxtest/TestSuite.h>
#include <unordered_set>
#include <vector>
#include <iostream>

#include "helper.h"
#include "teams.h"
#include "guest_tuple_iterator.h"

class TestGuestTupleGenerator : public CxxTest::TestSuite
{
	public:
		void testSimpleGeneration(void)
		{
			std::vector<mue::Team> teams(test::make_testteams(8));

			std::unordered_set<mue::Team_id> used;

			std::vector<mue::Team_id> team_ids;
			for (mue::Team& t: teams) {
				team_ids.push_back(t);
			}

			int count(0);
			mue::Guest_tuple_generator gen(team_ids, used);
			std::for_each(gen.begin(), gen.end(), [&count](mue::Guest_tuple_generator::GuestPair const&) { ++count; });
			TS_ASSERT_EQUALS(count, 28)
		}

		void testUniqueGeneration(void)
		{
			std::vector<mue::Team> teams(test::make_testteams(8));
			std::vector<std::unordered_set<mue::Team_id> > seen(8);

			std::unordered_set<mue::Team_id> used;

			std::vector<mue::Team_id> team_ids;
			for (mue::Team& t: teams) {
				team_ids.push_back(t);
			}

			mue::Guest_tuple_generator gen(team_ids, used);
			for (mue::Guest_tuple_generator::GuestPair const &pair : gen) {
				TS_ASSERT(seen[pair.first].find(pair.second) == seen[pair.first].end());
				TS_ASSERT(seen[pair.second].find(pair.first) == seen[pair.second].end());

				seen[pair.first].insert(pair.second);
				seen[pair.second].insert(pair.first);

			}
		}

		void testUsedGeneration(void)
		{
			std::vector<mue::Team> teams(test::make_testteams(8));
			std::unordered_set<mue::Team_id> used{1, 3, 5};

			std::vector<mue::Team_id> team_ids;
			for (mue::Team& t: teams) {
				team_ids.push_back(t);
			}

			mue::Guest_tuple_generator gen(team_ids, used);
			for (mue::Guest_tuple_generator::GuestPair const &pair : gen) {
				TS_ASSERT(used.find(pair.first) == used.end());
				TS_ASSERT(used.find(pair.second) == used.end());
			}
		}
};

#endif
