#ifndef MUE_FIRSTROUND_SELECT_H
#define MUE_FIRSTROUND_SELECT_H


#include <vector>

#include <boost/assert.hpp>

#include "teams.h"
#include "distances.h"
#include "team_container.h"


namespace mue {

	class Firstround_team_selection
	{
		private:
			const unsigned int _teamcount;
			const unsigned int _round_teams;
			const Distance _max_distance;
			Limited_array<std::vector<Team_id>, MAX_TEAMS / 3> _candidates;

			void _sort_filter_teams(Distance_matrix const &distance_matrix,
						size_t slice);

		public:
			Firstround_team_selection(Distance_matrix const &distance_matrix,
						  Distance max_distance,
					          size_t slice = 0) __attribute__((visibility("default"))) ;

			const std::vector<Team_id>& for_host(Team_id const &host) const
			{
				BOOST_ASSERT(host < _teamcount / 3);
				return _candidates[host];
			}
	};
}

#endif
