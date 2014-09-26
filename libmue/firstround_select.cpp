#include <algorithm>

#include "firstround_select.h"

mue::Firstround_team_selection::Firstround_team_selection(Distance_matrix const &distance_matrix,
							  unsigned int slice)
:
	_teamcount(distance_matrix.teamcount()),
	_candidates(_teamcount / 3)
{
	(void) slice;
	unsigned int round_teams = _teamcount / 3;
	BOOST_ASSERT(slice < round_teams * 2);

	for (Team_id host_team = 0; host_team < round_teams; ++host_team) {
		std::vector<Candidate> guest_candidates;
		guest_candidates.reserve(round_teams * round_teams);

		for (Team_id firststep_guest = round_teams;
		     firststep_guest < 2 * round_teams;
		     ++firststep_guest)
		{
			Distance firststep_distance = distance_matrix.lookup(host_team,
									     firststep_guest);
			std::vector<Candidate> second_candidates;

			for (Team_id secondstep_guest = 2 * round_teams;
			     secondstep_guest < 3 * round_teams;
			     ++secondstep_guest)
			{
				Distance secondstep_distance = distance_matrix.lookup(firststep_guest,
										      secondstep_guest);
				second_candidates.emplace_back(firststep_distance + secondstep_distance,
						               secondstep_guest);
			}
			auto min_candidate = std::min_element(second_candidates.begin(),
					                      second_candidates.end());

			// Ensure the firststep is cheaper than cheapest secindstep for this first
			firststep_distance = std::max(min_candidate->distance - 0.01, 0.0);
			guest_candidates.emplace_back(firststep_distance, firststep_guest);

			guest_candidates.insert(guest_candidates.end(),
					        second_candidates.begin(),
						second_candidates.end());
		}

		// Prepare the candidates for std::unique
		std::sort(guest_candidates.begin(), guest_candidates.end(),
				[] (Candidate const &a, Candidate const &b)
				{
					return a.team < b.team
					       || (a.team == b.team
						   && a.distance < b.distance);
				}
				);

		// Erase "longer" duplicates
		guest_candidates.erase(std::unique(guest_candidates.begin(), guest_candidates.end()),
				       guest_candidates.end());

		std::sort(guest_candidates.begin(), guest_candidates.end());

		// Put it into the destination container
		if (slice == 0)
			std::for_each(guest_candidates.begin(), guest_candidates.end(),
					[ & ] (Candidate const &c)
					{ _candidates[host_team].emplace_back(c.team); });
		else
			std::for_each(guest_candidates.begin(), guest_candidates.begin() + slice,
					[ & ] (Candidate const &c)
					{ _candidates[host_team].emplace_back(c.team); });

	}
}

