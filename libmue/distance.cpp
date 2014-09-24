#include <boost/assert.hpp>
#include <limits>
#include <algorithm>


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


std::vector<mue::Distance>
mue::Distance_forecast::_min_costs(Distance_matrix const &distances, unsigned hostcount,
		                   unsigned multiply)
{
	std::vector<Distance> first_tmp;
	for (unsigned src_index = 0; src_index < hostcount; ++src_index) {
		for (unsigned dst_index = 0; dst_index < hostcount; ++dst_index) {
			first_tmp.push_back(distances.lookup(src_index + multiply * hostcount,
							     dst_index + (multiply + 1) * hostcount));
		}
	}

	std::sort(first_tmp.begin(), first_tmp.end(),
			[] (Distance const &a, Distance const &b) { return a > b; });

	BOOST_ASSERT(first_tmp.size() >= hostcount);

	return std::vector<Distance>(first_tmp.end() - hostcount, first_tmp.end());
}


std::vector<mue::Distance>
mue::Distance_forecast::_fill_costs(Distance_matrix const &distances, unsigned hostcount,
		                    unsigned multiply, Distance const &offset)
{
	std::vector<Distance> min_costs(_min_costs(distances, hostcount, multiply));

	std::vector<Distance> accumulated;
	for (auto i = min_costs.begin(); i != min_costs.end(); ++i) {
		accumulated.push_back(std::accumulate(i, min_costs.end(), offset,
				      [] (Distance const &a, Distance const &b) { return a + b; }));
	}
	accumulated.push_back(offset);

	BOOST_ASSERT(accumulated.size() == hostcount + 1);
	return accumulated;
}


mue::Distance_forecast::Distance_forecast(Distance_matrix const &distances)
:
	_hostcount(distances.teamcount() / 3),
	_second_costs(_fill_costs(distances, _hostcount, 1, 0)),
	_first_costs(_fill_costs(distances, _hostcount, 0, _second_costs[0]))
{
}
