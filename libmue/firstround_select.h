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
			struct Candidate
			{
				Distance distance;
				Team_id team;

				Candidate(Distance const &distance, Team_id const &team)
				: distance(distance), team(team)
				{ }

				bool operator <(Candidate const &other)
				{ return distance < other.distance; }

				bool operator >(Candidate const &other)
				{ return distance > other.distance; }

				bool operator ==(Candidate const &other)
				{ return team == other.team; }
			};

			unsigned int _teamcount;
			std::vector<std::vector<Team_id> > _candidates;

		public:
			Firstround_team_selection(Distance_matrix const &distance_matrix,
					          unsigned int slice = 0);

			std::vector<Team_id> for_host(Team_id const &host)
			{
				BOOST_ASSERT(host < _teamcount / 3);
				return _candidates[host];
			}
	};
}

#endif
