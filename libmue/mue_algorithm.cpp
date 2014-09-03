#include <limits>
#include <iostream>

#include "mue_algorithm.h"

mue::Calculation::Calculation(unsigned int teamcount, Distance_matrix const &distance_matrix, Distance max_single_distance)
:
	_teamcount(teamcount),
	_teams_per_round(teamcount / 3),
	_round_hosts(3),
	_round_guests(3),
	_distance_matrix(distance_matrix),
	_best_distance(std::numeric_limits<float>::max()),
	_max_single_distance(max_single_distance),
#ifndef PREDEFINED_RANDOM
	_team_round_random(0, _teams_per_round -1),
	_random_generator(std::random_device()())
#else
	_team_shuffle(teamcount * 2)
#endif
{
	BOOST_ASSERT(_teamcount <= MAX_TEAMS);
	BOOST_ASSERT(_teamcount / 3 == float(_teamcount / 3));

	for (unsigned int round = 0; round < 3; ++round) {
		for (Team_id team = 0; team < teamcount; ++team) {
			if (_is_round_host(team, static_cast<Round>(round)))
				_round_hosts[round].push_back(team);
			else
				_round_guests[round].push_back(team);
		}
	}

#ifdef PREDEFINED_RANDOM
	std::random_device randd;
	std::mt19937 generator(randd());
	std::uniform_int_distribution<Team_id> random(0, _teams_per_round -1);


	for (Team_id team = 0; team < teamcount * 2; ++team)
		_team_shuffle[team] = random(generator) + (_teams_per_round * SECOND);
#endif
}


mue::Distance mue::Calculation::dummy_distance(Team_id host, Guest_tuple_generator::GuestPair const &guests) const
{
	if (_is_round_host(guests.first, SECOND))
		return _distance_matrix.lookup(host, guests.first);
	if (_is_round_host(guests.second, SECOND))
		return _distance_matrix.lookup(host, guests.second);

#ifdef PREDEFINED_RANDOM
	return _distance_matrix.lookup(host, _team_shuffle[guests.first + guests.second]) * 2;
#else
	return _distance_matrix.lookup(host, _random_host(SECOND)) * 2;
#endif
}


mue::Distance mue::Calculation::guest_distance(Round_data const &round_data, Team_id host, Guest_tuple_generator::GuestPair const &guests) const
{
	return _distance_matrix.lookup(round_data.prev_host(guests.first), host) +
	       _distance_matrix.lookup(round_data.prev_host(guests.second), host);
}

mue::Distance mue::Calculation::host_distance(Round_data const &round_data, Team_id host) const
{
	return _distance_matrix.lookup(round_data.prev_host(host), host);
}

std::vector<mue::Calculation::Guest_candidate> mue::Calculation::determine_guest_candidates(Round_data const &round_data, Iteration_data const &iteration_data, Team_id current_host) const
{
	std::vector<Guest_candidate> candidates;
	/*
	 * 63 teams have 24 guests (2/3)
	 * (math.factorial(24) / math.factorial(24-2)) / 2 = 276
	 * So for guests of 63 Teams 300 elements are enough.
	 */
	candidates.reserve(300);
	Guest_tuple_generator generator(round_data.guests, iteration_data.used_guests);
	for (Guest_tuple_generator::GuestPair const &guests : generator) {
		if (! iteration_data.seen_table.seen(current_host, guests.first, guests.second)) {
			if (! round_data.first_round()) {
				float single_distance = guest_distance(round_data, current_host, guests);
				float distance = guest_distance(round_data, current_host, guests);
				if (distance < _best_distance &&  single_distance < _max_single_distance)
					candidates.emplace_back(iteration_data.distance + guest_distance(round_data, current_host, guests), guests);
			} else {
				candidates.emplace_back(dummy_distance(current_host, guests), guests);
			}
		}
	}
	std::sort(candidates.begin(), candidates.end(), [](Guest_candidate const &a, Guest_candidate const &b) { return a.distance < b.distance; });
	return candidates;
}


std::ostream& mue::operator<<(std::ostream& os, mue::Calculation::Guest_candidate const& candidate)
{
	return os << "[(" << (int)candidate.guests.first << ", " << (int)candidate.guests.second << ")->" << candidate.distance << "]";
}


mue::Calculation::Round_data mue::Calculation::initial_round_data() const
{
	return Round_data(FIRST, _round_hosts[FIRST], _round_guests[FIRST], std::vector<std::vector<Team_id> >());
}


mue::Calculation::Round_data mue::Calculation::next_round_data(Round_data const &old, Iteration_data const &iteration) const
{
	BOOST_ASSERT(old.round < 2);
	Round next_rount = static_cast<Round>(old.round + 1);
	std::vector<std::vector<Team_id> > station_backlog(old.prev_stations);
	station_backlog.push_back(iteration.round_station);
	return Round_data(next_rount, _round_hosts[next_rount], _round_guests[next_rount], station_backlog);
}


std::vector<mue::Team_id> mue::Calculation::round_stations(Round round, Round_data const &round_data, Iteration_data const &iteration_data) const
{
	BOOST_ASSERT(round <= round_data);

	if (round == round_data.prev_stations.size())
		return iteration_data.round_station;

	return round_data.prev_stations[round];
}
