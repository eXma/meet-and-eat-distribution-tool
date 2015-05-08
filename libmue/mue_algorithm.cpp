#include <limits>
#include <algorithm>
#include <iostream>

#include "mue_algorithm.h"

mue::Calculation::Calculation(unsigned int           teamcount,
			      Distance_matrix const &distance_matrix,
			      Distance               max_single_distance)
:
	_teamcount(teamcount),
	_teams_per_round(teamcount / 3),
	_round_hosts(3),
	_round_guests(3),
	_distance_matrix(distance_matrix),
	_best_distance(std::numeric_limits<float>::max()),
	_max_single_distance(max_single_distance),
	_forecast(distance_matrix),
	_best_stations(3),
	_solutions(0),
	_firstround_selection(distance_matrix, _max_single_distance, _teams_per_round * 2 -2 ),
	_candidate_pool()
{
	BOOST_ASSERT(_teamcount <= MAX_TEAMS);
	BOOST_ASSERT(_teamcount / 3 == float(_teamcount / 3));

	for (unsigned int round = 0; round < 3; ++round) {
		std::vector<Team_id> round_hosts;
		std::vector<Team_id> round_guests;
		for (Team_id team = 0; team < teamcount; ++team) {
			if (_is_round_host(team, static_cast<Round>(round)))
				round_hosts.push_back(team);
			else
				round_guests.push_back(team);
		}
		_round_hosts[round] = Hosts(round_hosts);
		_round_guests[round] = Guests(round_guests);
	}

	Guest_candidate_allocator alloc(200 * _teamcount);
	/*
	 * 63 teams have 42 guests (2/3)
	 * (math.factorial(42) / math.factorial(42-2)) / 2 = 861
	 * So for guests of 63 Teams 900 elements are enough.
	 */
	_candidate_pool.reserve(_teamcount);
	for(unsigned int i = 0; i < _teamcount; i++) {
		_candidate_pool.emplace_back(alloc);
		_candidate_pool[i].reserve(200);
	}

}

mue::Distance mue::Calculation::guest_distance(Round_data                       const &round_data,
					       Team_id                                host,
					       Guest_tuple_generator::GuestPair const &guests) const
{
	return _distance_matrix.lookup(round_data.prev_host(guests.first), host) +
	       _distance_matrix.lookup(round_data.prev_host(guests.second), host);
}

mue::Distance mue::Calculation::host_distance(Round_data const &round_data, Team_id host) const
{
	return _distance_matrix.lookup(round_data.prev_host(host), host);
}

std::vector<mue::Calculation::Guest_candidate>
mue::Calculation::determine_guest_candidates(Round_data     const &round_data,
					     Iteration_data const &iteration_data,
					     Team_id               current_host,
					     size_t         const &host_idx,
					     size_t                slice)
{
	BOOST_ASSERT(! round_data.first_round());

	Guest_candidate_vec &candidates(_candidate_pool[host_idx + (_teams_per_round * round_data.round)]);
	candidates.clear();

	Guest_tuple_generator generator(round_data.guests, iteration_data.used_guests);
	for (Guest_tuple_generator::GuestPair const &guests : generator) {
		if (! iteration_data.seen(current_host, guests.first, guests.second)) {
			float single_distance =
				guest_distance(round_data, current_host, guests);
			float distance = single_distance + iteration_data.distance;

			if (single_distance < _max_single_distance
			   &&  _distance_is_better(distance + (round_data.round == SECOND
							       ? _forecast.first_move(host_idx)
							       : _forecast.second_move(host_idx))))
				candidates.emplace_back(distance, guests);
		}
	}
	std::sort(candidates.begin(), candidates.end(),
			[](Guest_candidate const &a, Guest_candidate const &b)
				{ return a.distance < b.distance; });
	return std::vector<Guest_candidate>(candidates.begin(),
					    candidates.begin()
					    + std::min(candidates.size(), slice));
}


mue::Calculation::Round_data mue::Calculation::initial_round_data() const
{
	return Round_data(FIRST, _round_hosts[FIRST], _round_guests[FIRST],
			  Stations(3));
}


mue::Calculation::Round_data mue::Calculation::next_round_data(Round_data const &old,
							       Iteration_data const &iteration) const
{
	BOOST_ASSERT(old.round < 2);
	Round next_round = static_cast<Round>(old.round + 1);
	Stations station_backlog(old.prev_stations);
	station_backlog[old.round] = iteration.round_station;
	return Round_data(next_round, _round_hosts[next_round], _round_guests[next_round], station_backlog);
}


std::vector<mue::Team_id> mue::Calculation::round_stations(Round round) const
{
	Teams const &round_teams(_best_stations[round]);
	return std::vector<Team_id>(round_teams.begin(), round_teams.end());
}


void mue::Calculation::print_stations(Stations const &stations)
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
		if (round == round_data.round)
			_best_stations[round] = iteration.round_station;
		else
			_best_stations[round] = round_data.prev_stations[round];
	}
	print_stations(_best_stations);
	std::cout << std::endl;
	std::cout.flush();
	if (_solutions == 20)
		exit(0);
}


void mue::Calculation::run_firstround_distribution(Round_data const &round_data,
						   Iteration_data &iteration,
						   size_t host_index)
{
	if (host_index == _teams_per_round) {
		run_new_round(round_data, iteration);
		return;
	}

	Team_id host = round_data.hosts[host_index];

	Guest_tuple_generator generator(_firstround_selection.for_host(host),
			                iteration.used_guests);
	for (Guest_tuple_generator::GuestPair const &guests : generator) {
		Iteration_data new_iteration(0, iteration);
		new_iteration.set_station(host, guests.first, guests.second);
		run_firstround_distribution(round_data, new_iteration, host_index + 1);
	}
}


void mue::Calculation::run_distribution(Round_data const &round_data,
					Iteration_data &iteration,
					size_t host_index)
{
	BOOST_ASSERT(! round_data.first_round());
	if (host_index == _teams_per_round) {
		run_new_round(round_data, iteration);
		return;
	}

	Team_id host = round_data.hosts[host_index];

	iteration.distance += host_distance(round_data, host);
	if (!_distance_is_better(iteration.distance))
		return;

	size_t slices = _teams_per_round / 3;

	for (Guest_candidate const &candidate : determine_guest_candidates(round_data, iteration, host, host_index, slices)) {
		Iteration_data new_iteration(candidate.distance, iteration);
		new_iteration.set_station(host, candidate.guests.first, candidate.guests.second);
		run_distribution(round_data, new_iteration, host_index + 1);
	}
}


void mue::Calculation::run_new_round(Round_data const &round_data, Iteration_data &iteration)
{
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
	Round_data round_data(FIRST,
			      _round_hosts[FIRST],
			      _round_guests[FIRST],
			      Stations(3));

	Iteration_data iteration_data(_teamcount);

	run_firstround_distribution(round_data, iteration_data, 0);

}
