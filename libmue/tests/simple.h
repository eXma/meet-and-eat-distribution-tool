#include <cxxtest/TestSuite.h>

#include "seen_table.h"

class MyTestSuite1 : public CxxTest::TestSuite
{
public:
    void testAddition(void)
    {
        TS_ASSERT(1 + 1 > 1);
        TS_ASSERT_EQUALS(1 + 1, 2);
    }
    void testLinking(void)
    {
	mue::Seen_table st(10);
	TS_ASSERT_EQUALS(st.generation(), 0);

	mue::Seen_table st2(st.clone());
	TS_ASSERT_EQUALS(st.generation(), 0);
	TS_ASSERT_EQUALS(st2.generation(), 1);
	//
//	TS_ASSERT(!st.seen(1,2,3));
    }
};
