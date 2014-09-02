#include "mue_algorithm.h"

mue::Calculation::Calculation(unsigned int teamcount, Distance_matrix const &distance_matrix)
:
	_teamcount(teamcount),
	_teams_per_round(teamcount / 3),
	_distance_matrix(distance_matrix),
	_team_round_random(0, _teams_per_round -1),
	_random_generator(std::random_device()())
{
	BOOST_ASSERT(_teamcount <= MAX_TEAMS);
	BOOST_ASSERT(_teamcount / 3 == float(_teamcount / 3));
}


mue::Distance mue::Calculation::dummy_distance(Team_id host, Guest_tuple_generator::GuestPair const &guests)
{
	if (_is_round_host(guests.first, SECOND))
		return _distance_matrix.lookup(host, guests.first);
	if (_is_round_host(guests.second, SECOND))
		return _distance_matrix.lookup(host, guests.second);
	return _distance_matrix.lookup(host, _random_host(SECOND)) * 2;
}
