#include <limits>
#include <algorithm>
#include <iostream>
#include <unistd.h>

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
	_firstround_selection(distance_matrix, _max_single_distance, _teams_per_round * 2 -2 )
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

std::vector<mue::Calculation::Guest_candidate>&
mue::Calculation::determine_guest_candidates(Round_data     const &round_data,
					     Iteration_data const &iteration_data,
					     Team_id               current_host,
					     size_t         const &host_idx,
					     size_t                slice,
						 std::vector<Guest_candidate>& candidates) const
{
	BOOST_ASSERT(! round_data.first_round());

    candidates.clear();
	Guest_tuple_generator generator(round_data.guests, iteration_data.used_guests);
	for (Guest_tuple_generator::GuestPair const &guests : generator) {
		if (! iteration_data.seen(current_host, guests.first, guests.second)) {
			float single_distance =
				guest_distance(round_data, current_host, guests);
			float distance = single_distance + iteration_data.distance;

			if (single_distance < _max_single_distance
			   &&  _distance_is_better(distance + (round_data.round == FIRST
							       ? _forecast.first_move(host_idx)
							       : _forecast.second_move(host_idx))))
				candidates.emplace_back(distance, guests);
		}
	}
	std::sort(candidates.begin(), candidates.end(),
			[](Guest_candidate const &a, Guest_candidate const &b)
				{ return a.distance < b.distance; });

    candidates.erase(candidates.begin() + std::min(candidates.size(), slice), candidates.end());
    return candidates;
}


mue::Calculation::Round_data mue::Calculation::initial_round_data() const
{
	return Round_data(FIRST, _round_hosts[FIRST], _round_guests[FIRST],
			  std::vector<std::vector<Team_id> >());
}


mue::Calculation::Round_data mue::Calculation::next_round_data(Round_data const &old,
							       Iteration_data const &iteration) const
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
#if defined(_OPENMP)
#pragma omp critical
#endif
	{
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
		std::cout.flush();
	}
}


void mue::Calculation::run_firstround_distribution_seq(Round_data const &round_data,
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
		Iteration_data new_iteration(iteration.next_iteration((0),
						 host, guests));
		run_firstround_distribution_seq(round_data, new_iteration, host_index + 1);
	}
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
#if defined(_OPENMP)
#pragma omp parallel default(none) shared(iteration, round_data, host_index, generator, host)
	{
#pragma omp single
#endif
		{
			for (Guest_tuple_generator::GuestPair guests : generator) {
#if defined(_OPENMP)
#pragma omp task
#endif
				{
					Iteration_data new_iteration(iteration.next_iteration((0),
									 host, guests));
					run_firstround_distribution_seq(round_data, new_iteration, host_index + 1);
				}
			}
		}
#if defined(_OPENMP)
	}
#endif
}


void mue::Calculation::run_distribution(Round_data const &round_data,
					Iteration_data &iteration, size_t host_index, std::vector<std::vector<Guest_candidate>>& candidate_pool)
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

	std::vector<Guest_candidate>& candidates = candidate_pool[host_index];
	for (Guest_candidate const &candidate : determine_guest_candidates(round_data, iteration, host, host_index, slices, candidates)) {
		Iteration_data new_iteration(iteration.next_iteration((candidate.distance),
					     host, candidate.guests));
		run_distribution(round_data, new_iteration, host_index + 1, candidate_pool);
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

	// a pre-allocated vector that is reused to avoid allocations
	std::vector<std::vector<Guest_candidate>> round_candidates;
	/*
	 * 63 teams have 42 guests (2/3)
	 * (math.factorial(42) / math.factorial(42-2)) / 2 = 861
	 * So for guests of 63 Teams 900 elements are enough.
	 */
    round_candidates.reserve(_teams_per_round);
	for(unsigned int i = 0; i < _teams_per_round; i++) {
		round_candidates.emplace_back();
		round_candidates[i].reserve(900);
	}

	run_distribution(new_round, iteration, 0, round_candidates);
}


void mue::Calculation::calculate_distribution()
{
	Round_data round_data(initial_round_data());
	Iteration_data iteration_data(_teamcount);

	run_firstround_distribution(round_data, iteration_data, 0);

}
