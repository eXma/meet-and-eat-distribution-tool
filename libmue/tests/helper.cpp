#include "helper.h"

std::vector<mue::Team> test::make_testteams(unsigned int num)
{
	std::vector<mue::Team> teams;
	
	for (mue::Team_id i = 0; i < num; ++i)
		teams.push_back(mue::Team(i));
	return teams;
}
