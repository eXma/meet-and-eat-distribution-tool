#include <limits>
#include <algorithm>
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
	_best_stations(3),
	_solutions(0),
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

	std::generate(_team_shuffle.begin(), _team_shuffle.end(),
			[&] () { return random(generator) + (_teams_per_round * SECOND); });
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

std::vector<mue::Calculation::Guest_candidate> mue::Calculation::determine_guest_candidates(Round_data const &round_data, Iteration_data const &iteration_data, Team_id current_host, size_t slice) const
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
		if (! iteration_data.seen(current_host, guests.first, guests.second)) {
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
	return std::vector<Guest_candidate>(candidates.begin(), candidates.begin() + std::min(candidates.size(), slice));
}



mue::Calculation::Round_data mue::Calculation::initial_round_data() const
{
	return Round_data(FIRST, _round_hosts[FIRST], _round_guests[FIRST], std::vector<std::vector<Team_id> >());
}


mue::Calculation::Round_data mue::Calculation::next_round_data(Round_data const &old, Iteration_data const &iteration) const
{
	BOOST_ASSERT(old.round < 2);
	Round next_round = static_cast<Round>(old.round + 1);
	std::vector<std::vector<Team_id> > station_backlog(old.prev_stations);
	station_backlog.push_back(iteration.round_station);
	return Round_data(next_round, _round_hosts[next_round], _round_guests[next_round], station_backlog);
}


std::vector<mue::Team_id> mue::Calculation::round_stations(Round round) const
{
	return _best_stations[round];
}


void mue::Calculation::print_stations(std::vector<std::vector<Team_id> > const &stations)
{
	std::cout << "[";
	bool list_first(true);
	for (int round = 0; round< 3; ++round)
	{
		if (!list_first) {
			std::cout << ", ";
		}
		list_first = false;
		std::cout << "[";
		std::map<Team_id, std::vector<Team_id> > tmp;
		for (size_t i = 0; i < stations[round].size(); ++i) {
			tmp[stations[round][i]].push_back(i);
		}

		bool set_first(true);
		for (std::pair<const Team_id, std::vector<Team_id> > const &meeting : tmp) {
			if (!set_first) {
				std::cout << ", ";
			}
			set_first = false;
			std::cout << "set([" << (int) meeting.second[0]
				  <<    ", " << (int) meeting.second[1]
				  <<    ", " << (int) meeting.second[2]
				  << "])";
		}
		std::cout << "]";
	}
	std::cout << "]";
}


void mue::Calculation::report_success(Round_data const &round_data, Iteration_data const &iteration)
{
	_solutions++;
	if (!_distance_is_better(iteration.distance))
		return;
	_best_distance = iteration.distance;
	std::cout << "new best (" << _solutions << ") " << std::fixed << _best_distance << "  ";
	for (uint8_t round = 0; round < 3; ++round) {
		if (round == round_data.prev_stations.size())
			_best_stations[round] = iteration.round_station;
		else
			_best_stations[round] = round_data.prev_stations[round];
	}
	print_stations(_best_stations);
	std::cout << std::endl;

}


void mue::Calculation::run_distribution(Round_data const &round_data, Iteration_data &iteration, size_t host_index) {
	if (host_index == _teams_per_round) {
		run_new_round(round_data, iteration);
		return;
	}

	Team_id host = round_data.hosts[host_index];

	if (! round_data.first_round()) {
		iteration.distance += host_distance(round_data, host);
		if (!_distance_is_better(iteration.distance))
			return;
	}

	size_t slices = (round_data.first_round() ? _teams_per_round * 3 : _teams_per_round / 3);

	for (Guest_candidate const &candidate : determine_guest_candidates(round_data, iteration, host, slices)) {
		Iteration_data new_iteration(iteration.next_iteration((round_data.first_round() ? iteration.distance : candidate.distance),
					     host, candidate.guests));
		run_distribution(round_data, new_iteration, host_index + 1);
	}
}


void mue::Calculation::run_new_round(Round_data const &round_data, Iteration_data &iteration) {
	if (round_data.round == THIRD) {
		report_success(round_data, iteration);
		return;
	}

	Round_data new_round(next_round_data(round_data, iteration));
	iteration.clear_round_data();

	run_distribution(new_round, iteration, 0);
}


void mue::Calculation::calculate_distribution()
{
	Round_data round_data(initial_round_data());
	Iteration_data iteration_data(_teamcount);

	run_distribution(round_data, iteration_data, 0);

}
