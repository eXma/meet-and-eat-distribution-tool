#include <limits>
#include <iostream>

#include "mue_algorithm.h"

mue::Calculation::Calculation(unsigned int teamcount, Distance_matrix const &distance_matrix)
:
	_teamcount(teamcount),
	_teams_per_round(teamcount / 3),
	_distance_matrix(distance_matrix),
	_team_round_random(0, _teams_per_round -1),
	_random_generator(std::random_device()()),
	_best_distance(std::numeric_limits<float>::max())
{
	BOOST_ASSERT(_teamcount <= MAX_TEAMS);
	BOOST_ASSERT(_teamcount / 3 == float(_teamcount / 3));
}


mue::Distance mue::Calculation::dummy_distance(Team_id host, Guest_tuple_generator::GuestPair const &guests) const
{
	if (_is_round_host(guests.first, SECOND))
		return _distance_matrix.lookup(host, guests.first);
	if (_is_round_host(guests.second, SECOND))
		return _distance_matrix.lookup(host, guests.second);
	return _distance_matrix.lookup(host, _random_host(SECOND)) * 2;
}


mue::Distance mue::Calculation::guest_distance(Round_data const &round_data, Team_id host, Guest_tuple_generator::GuestPair const &guests) const
{
	return _distance_matrix.lookup(round_data.prev_host(guests.first), host) +
	       _distance_matrix.lookup(round_data.prev_host(guests.second), host);
	      
}

std::vector<mue::Calculation::Guest_candidate> mue::Calculation::determine_guest_candidates(Round_data const &round_data, Iteration_data const &iteration_data, Team_id current_host) const
{
	std::vector<Guest_candidate> candidates;
	Guest_tuple_generator generator(round_data.guests, iteration_data.used_guests);
	for (Guest_tuple_generator::GuestPair const &guests : generator) {
		if (! iteration_data.seen_table.seen(current_host, guests.first, guests.second)) {
			if (! round_data.first_round()) {
				float distance = iteration_data.distance + guest_distance(round_data, current_host, guests);
				if (distance < _best_distance)
					candidates.emplace_back(iteration_data.distance + guest_distance(round_data, current_host, guests), guests);
			} else {
				candidates.emplace_back(iteration_data.distance + dummy_distance(current_host, guests), guests);
			}
		}
	}
	std::sort(candidates.begin(), candidates.end(), [](Guest_candidate const &a, Guest_candidate const &b) { return a.first < b.first; });
	return candidates;
}
