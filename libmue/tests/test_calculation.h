#ifndef TEST__CALCULATION_H
#define TEST__CALCULATION_H

#include <cxxtest/TestSuite.h>

#include <iostream>

#include "distances.h"
#include "mue_algorithm.h"

class TestCalculation : public CxxTest::TestSuite
{
	public:
		void testDummyDistance(void)
		{
			mue::Distance_matrix matrix(9);
			for (int k = 0; k < 2; ++k) {
				int lower = k * 3;
				int upper = lower + 3;
				for (int i = lower; i < upper; ++i) {
					for (int j = 3 + lower; j < 3 + upper; ++j) {
						matrix.set_cost(i, j, k + 1);
					}
				}
			}

			mue::Calculation  calc(9, matrix, 10);

			for (mue::Team_id teamA = 0; teamA < 9; ++teamA) {
				for (mue::Team_id teamB = 0; teamB < 9; ++teamB) {
					for (mue::Team_id host = 0; host < 9; ++host) {
						int koeff = 1;
						if (((teamA < 3) || (teamA > 5)) && ((teamB < 3) || (teamB > 5))) {
							koeff = 2;
						}
						int expected = 0;
						if (host < 3) {
							expected = 1;
						} else if (host < 6) {
							expected = 0;
						} else {
							expected = 2;;
						}
						TS_ASSERT_EQUALS(
							calc.dummy_distance(host, mue::Guest_tuple_generator::GuestPair(teamA, teamB)),
						        expected * koeff);
					}
				}
			}

		}


		void testGuestDistance(void)
		{
			mue::Distance_matrix matrix(9);
			for (int k = 0; k < 2; ++k) {
				int lower = k * 3;
				int upper = lower + 3;
				for (int i = lower; i < upper; ++i) {
					for (int j = 3 + lower; j < 3 + upper; ++j) {
						matrix.set_cost(i, j, k + 1);
					}
				}
			}

			mue::Calculation  calc(9, matrix, 10);
			mue::Calculation::Round_data round_data(mue::Calculation::SECOND,
								std::vector<mue::Team_id>(),
								std::vector<mue::Team_id>(),
								std::vector<std::vector<mue::Team_id> > { std::vector<mue::Team_id>{0,1,6,8,0,0,0,0,0}});

			TS_ASSERT_EQUALS(calc.guest_distance(round_data, 3, mue::Guest_tuple_generator::GuestPair(1, 2)), 1+2);
			TS_ASSERT_EQUALS(calc.guest_distance(round_data, 0, mue::Guest_tuple_generator::GuestPair(6, 7)), 0+0);
		}

};


#endif

