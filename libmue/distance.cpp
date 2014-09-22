#include <boost/assert.hpp>
#include <limits>


#include "distances.h"
#include "config.h"


mue::Distance_matrix::Distance_matrix(int teamcount) noexcept
:
	_table(teamcount, std::vector<Distance>(teamcount, 0)),
	_teamcount(teamcount),
	_min_cost(std::numeric_limits<Distance>::max())
{
	BOOST_ASSERT(teamcount <= MAX_TEAMS);
}


void mue::Distance_matrix::set_cost(Team_id const source, Team_id const destination, Distance cost)
{
	BOOST_ASSERT(source < _teamcount);
	BOOST_ASSERT(destination < _teamcount);

	_table[source][destination] = cost;
	_table[destination][source] = cost;

	if (cost < _min_cost)
		_min_cost = cost;
}
