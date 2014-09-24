#ifndef MUE_ALGORITHM_H
#define MUE_ALGORITHM_H

#include <random>
#include <map>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <ios>


#include <boost/assert.hpp>

#include "guest_tuple_iterator.h"
#include "distances.h"
#include "config.h"
#include "seen_table.h"


#define PREDEFINED_RANDOM

namespace mue {



class Calculation
{
	public:
		enum Round : std::uint8_t {
			FIRST = 0,
			SECOND = 1,
			THIRD = 2
		};


		struct Round_data
		{
			Calculation::Round   const round;
			std::vector<Team_id> const hosts;
			std::vector<Team_id> const guests;

			std::vector<std::vector<Team_id> > const prev_stations;

			bool first_round() const { return round == Calculation::FIRST; }
			Team_id prev_host(Team_id team) const {
				return prev_stations.back()[team];
			}

			Round_data(Calculation::Round round,
				   std::vector<Team_id> const &hosts,
				   std::vector<Team_id> const &guests,
				   std::vector<std::vector<Team_id> > const &prev_stations)
			:
				round(round),
				hosts(hosts),
				guests(guests),
				prev_stations(prev_stations)
			{ }

			Round_data(Round_data &&) = default;
			Round_data(Round_data const &) = delete;
			Round_data & operator=(Round_data const &other) = delete;
		};

	private:

		struct Iteration_data
		{
			Distance                    distance;
			Guest_tuple_generator::Used_bits used_guests;
			std::vector<Team_id>        round_station;
			Seen_table                  seen_table;

			void set_station(Team_id host, Team_id guest1, Team_id guest2)
			{
				round_station[host] = host;
				round_station[guest1] = host;
				round_station[guest2] = host;

				used_guests.set(guest1);
				used_guests.set(guest2);

				seen_table.add_meeting(host, guest1, guest2);
			}

			Iteration_data(unsigned int teamcount)
			:
				distance(0),
				used_guests(),
				round_station(teamcount),
				seen_table(teamcount)
			{ }


			Iteration_data(Iteration_data const &other) = delete;
			Iteration_data& operator=(Iteration_data const &) = delete;
			Iteration_data(Iteration_data&& other) = default;

			Iteration_data(Distance new_distance, Iteration_data const &other)
			:
				distance(new_distance),
				used_guests(other.used_guests),
				round_station(other.round_station),
				seen_table(other.seen_table.clone())
			{}

			bool seen(Team_id host, Team_id guestA, Team_id guestB) const
			{
				return seen_table.seen(host, guestA, guestB);
			}

			void clear_round_data()
			{
				used_guests.reset();
			}

			Iteration_data next_iteration(Distance new_distance,
						      Team_id host,
						      Guest_tuple_generator::GuestPair const &guests)
			{
				Iteration_data new_data(new_distance, *this);
				new_data.set_station(host, guests.first, guests.second);
				return new_data;
			}
		};


		struct Guest_candidate
		{
			Distance distance;
			Guest_tuple_generator::GuestPair guests;

			Guest_candidate(Distance distance, Guest_tuple_generator::GuestPair const &guests)
			: distance(distance), guests(guests)
			{ }
		};



		size_t                              const _teamcount;
		size_t                              const _teams_per_round;
		std::vector<std::vector<Team_id> >        _round_hosts;
		std::vector<std::vector<Team_id> >        _round_guests;
		Distance_matrix                     const _distance_matrix;
		Distance                                  _best_distance;
		Distance                            const _max_single_distance;
		Distance_forecast                   const _forecast;
		std::vector<std::vector<Team_id> >        _best_stations;
		size_t                                    _solutions;

#ifndef PREDEFINED_RANDOM
		mutable std::uniform_int_distribution<Team_id> _team_round_random;
		mutable std::mt19937 _random_generator;
#else
		std::vector<Team_id> _team_shuffle;
#endif

		bool _is_round_host(Team_id team, Round round) const
		{
			return     (_teams_per_round * round       <= team)
				&& (_teams_per_round * (round + 1) >  team);
		}

#ifndef PREDEFINED_RANDOM
		Team_id _random_host(Round round) const
		{
			return _team_round_random(_random_generator) + (_teams_per_round * round);
		}
#endif

		bool _distance_is_better(Distance new_dist) const  { return _best_distance > new_dist; }
		void run_new_round(Round_data const &round_data, Iteration_data &iteration);
		void run_distribution(Round_data const &round_data, Iteration_data &iteration, size_t host_index);
		void report_success(Round_data const &round_data, Iteration_data const &iteration);
		void print_stations(std::vector<std::vector<Team_id> > const &stations);

		std::vector<Guest_candidate> determine_guest_candidates(Round_data const &round_data,
									Iteration_data const &iteration_data,
									Team_id current_host,
									size_t const &host_idx,
									size_t slice) const;

		void update_best(float best) { _best_distance = best; }

		Round_data initial_round_data() const;

		Round_data next_round_data(Round_data const &round_data,
					   Iteration_data const &data) const;

	public: // UnitTests...
		Distance dummy_distance(Team_id host, Guest_tuple_generator::GuestPair const &guests) const;

		Distance guest_distance(Round_data const &round_data,
					Team_id host,
					Guest_tuple_generator::GuestPair const &guests) const;

		Distance host_distance(Round_data const &round_data,
				       Team_id host) const;


	public:
		Calculation(unsigned int teamcount,
			    Distance_matrix const &distance_matrix,
			    Distance max_single_distance);

		unsigned int teamcount() { return _teamcount; }
		unsigned int solutions() { return _solutions; }

		std::vector<Team_id> round_stations(Round round) const;

		void calculate_distribution();

};


}

#endif


