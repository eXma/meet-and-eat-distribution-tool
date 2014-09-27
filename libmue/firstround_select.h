#ifndef MUE_FIRSTROUND_SELECT_H
#define MUE_FIRSTROUND_SELECT_H


#include <vector>

#include <boost/assert.hpp>

#include "teams.h"
#include "distances.h"


namespace mue {

	class Firstround_team_selection
	{
		private:
			unsigned int _teamcount;
			std::vector<std::vector<Team_id> > _candidates;

		public:
			Firstround_team_selection(Distance_matrix const &distance_matrix,
						  Distance max_distance,
					          size_t slice = 0);

			std::vector<Team_id> for_host(Team_id const &host) const
			{
				BOOST_ASSERT(host < _teamcount / 3);
				return _candidates[host];
			}
	};
}

#endif
