#ifndef MUE_ALGORITHM_H
#define MUE_ALGORITHM_H

#include <map>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <ios>

#include <string.h>

#include <boost/assert.hpp>

#include "guest_tuple_iterator.h"
#include "distances.h"
#include "config.h"
#include "seen_table.h"
#include "firstround_select.h"
#include "team_container.h"


namespace mue {



class Calculation
{
	public:
		enum Round : std::uint8_t {
			FIRST = 0,
			SECOND = 1,
			THIRD = 2
		};

		using Stations = Limited_array<Teams, 3>;

		struct Round_data
		{

			Calculation::Round const round;
			Hosts              const hosts;
			Guests             const guests;

			Stations           const prev_stations;

			bool first_round() const { return round == Calculation::FIRST; }
			Team_id prev_host(Team_id team) const {
				BOOST_ASSERT( round != 0 );
				return prev_stations[round - 1][team];
			}

			Round_data(Calculation::Round round,
				   Hosts const &hosts,
				   Guests const &guests,
				   Stations const &prev_stations)
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
			Teams                       round_station;
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
			Iteration_data(Iteration_data&& other) = delete;

			Iteration_data(Distance new_distance, Iteration_data const &other)
			:
				distance(new_distance),
				used_guests(other.used_guests),
				round_station(other.round_station),
				seen_table(other.seen_table.clone())
			{ }

			bool seen(Team_id host, Team_id guestA, Team_id guestB) const
			{
				return seen_table.seen(host, guestA, guestB);
			}

			void clear_round_data()
			{
				used_guests.reset();
			}

			std::vector<Team_id>& report_stations(std::vector<Team_id>& dest,
							      const Team_id teamcount) const
			{
				dest.clear();
				for (Team_id team = 0; team < teamcount; ++team) {
					dest.push_back(round_station[team]);
				}
				return dest;
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


		template<typename Tp>
		class Simple_block_allocator
		{
			public:
			typedef Tp value_type;

			private:
			const std::size_t _elements;
			std::shared_ptr<std::size_t> _offset;
			std::shared_ptr<value_type> _memory;

			public:
			Simple_block_allocator(Simple_block_allocator const &other)
			:
				_elements(other._elements),
				_offset(other._offset),
				_memory(other._memory)
			{ }

			Simple_block_allocator()
			:
				_elements(0),
				_offset(new std::size_t(0)),
				_memory(0)
			{ }

			Simple_block_allocator(std::size_t elements)
			:
				_elements(elements),
				_offset(new std::size_t(0)),
				_memory(static_cast<value_type*>(::operator new(sizeof(value_type) * _elements)))
			{ }


			value_type* allocate(std::size_t n)
			{
				BOOST_ASSERT(*_offset + n <= _elements);

				value_type* ptr(static_cast<value_type*>(_memory.get() + *_offset));
				*_offset += n;
				return ptr;
			}

			void deallocate(value_type* , std::size_t ) { }

			bool operator==(Simple_block_allocator const &other) { return _memory == other._memory; }
		};
		using Guest_candidate_allocator = Simple_block_allocator<Guest_candidate>;
		using Guest_candidate_vec = std::vector<Guest_candidate, Guest_candidate_allocator>;
		using Guest_candidate_pool = std::vector<Guest_candidate_vec>;



		size_t                              const _teamcount;
		size_t                              const _teams_per_round;
		std::vector<Hosts>                        _round_hosts;
		std::vector<Guests>                       _round_guests;
		Distance_matrix                     const _distance_matrix;
		Distance                                  _best_distance;
		Distance                            const _max_single_distance;
		Distance_forecast                   const _forecast;
		Stations			          _best_stations;
		size_t                                    _solutions;
		Firstround_team_selection           const _firstround_selection;
		Guest_candidate_pool                      _candidate_pool;

		bool _is_round_host(Team_id team, Round round) const
		{
			return     (_teams_per_round * round       <= team)
				&& (_teams_per_round * (round + 1) >  team);
		}


		bool _distance_is_better(Distance new_dist) const  { return _best_distance > new_dist; }
		void run_new_round(Round_data const &round_data, Iteration_data &iteration);

		void run_distribution(Round_data const &round_data,
				      Iteration_data &iteration,
				      size_t host_index);

		void run_firstround_distribution(Round_data const &round_data, Iteration_data &iteration, size_t host_index);
		void report_success(Round_data const &round_data, Iteration_data const &iteration);
		void print_stations(Stations const &stations);

		Guest_candidate_vec&
		determine_guest_candidates(Round_data const &round_data,
					   Iteration_data const &iteration_data,
					   Team_id current_host,
					   size_t const &host_idx,
					   size_t slice);

		void update_best(Distance best) { _best_distance = best; }

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


