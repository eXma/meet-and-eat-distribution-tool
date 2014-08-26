#ifndef TEST_GUEST_TUPLE_ITERATOR_H
#define TEST_GUEST_TUPLE_ITERATOR_H

#include <cxxtest/TestSuite.h>
#include <unordered_set>
#include <vector>

#include "teams.h"
#include "guest_tuple_iterator.h"

class TestSeenTable : public CxxTest::TestSuite
{
	private:
		std::vector<mue::Team> make_testteams(int num)
		{
			std::vector<mue::Team> teams;
			
			for (mue::Team_id i = 0; i < num; ++i)
				teams.push_back(mue::Team(i));
			return teams;
		}

	public:
		void testFooBar(void)
		{


		}
};

#endif
