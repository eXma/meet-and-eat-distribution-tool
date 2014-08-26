#ifndef TEST__SEEN_TABLE_H
#define TEST__SEEN_TABLE_H

#include <cxxtest/TestSuite.h>

#include "seen_table.h"

class TestSeenTable : public CxxTest::TestSuite
{
	public:
		void testGeneration(void)
		{
			mue::Seen_table st(10);
			TS_ASSERT_EQUALS(st.generation(), 0);

			mue::Seen_table st1_1(st.clone());
			TS_ASSERT_EQUALS(st.generation(), 0);
			TS_ASSERT_EQUALS(st1_1.generation(), 1);

			mue::Seen_table st1_2(st.clone());
			TS_ASSERT_EQUALS(st.generation(), 0);
			TS_ASSERT_EQUALS(st1_2.generation(), 1);

			mue::Seen_table st2(st1_1.clone());
			TS_ASSERT_EQUALS(st1_1.generation(), 1);
			TS_ASSERT_EQUALS(st2.generation(), 2);
		}


		void testSeen(void)
		{
			mue::Seen_table st(10);

			for (int i = 0; i < 10; ++i) {
				TS_ASSERT(!st.seen(i % 10, i % 10, i % 10));
			}

			st.add_meeting(0, 1, 2);

			TS_ASSERT(st.seen(0, 1, 2));
			TS_ASSERT(st.seen(0, 1, 3));
			TS_ASSERT(!st.seen(3, 3, 4));
		}


		void testAddMeeting(void)
		{
			mue::Seen_table original(10);
			TS_ASSERT(!original.seen(0, 1, 2));

			{
				mue::Seen_table gen1(original.clone());
				gen1.add_meeting(0, 1, 2);

				TS_ASSERT(gen1.seen(0, 1, 2));
				TS_ASSERT(!original.seen(0, 1, 2));

				gen1.add_meeting(0, 1, 3);
				TS_ASSERT(gen1.seen(0, 1, 3));
				TS_ASSERT(!original.seen(0, 1, 3));

				mue::Seen_table gen2(gen1.clone());
				gen1.add_meeting(3, 4, 5);

				TS_ASSERT(gen2.seen(0, 1, 2));
				TS_ASSERT(gen2.seen(3, 4, 5));

				TS_ASSERT(!gen1.seen(2, 3, 4));
			}

			for (int i = 0; i < 10; ++i) {
				TS_ASSERT(!original.seen(i % 10, i % 10, i % 10));
			}
		}
};


#endif
