#include <boost/assert.hpp>


#include "distances.h"
#include "config.h"


mue::Distance_matrix::Distance_matrix(int teamcount) noexcept
: 
	_table(teamcount, std::vector<Distance>(teamcount, 0)),
	_teamcount(teamcount)
{
	BOOST_ASSERT(teamcount <= MAX_TEAMS);
}
  

void mue::Distance_matrix::set_cost(Team_id const source, Team_id const destination, Distance cost)
{
	BOOST_ASSERT(source < _teamcount);
	BOOST_ASSERT(destination < _teamcount);

	_table[source][destination] = cost;
	_table[destination][source] = cost;
}
