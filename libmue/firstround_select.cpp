#include <algorithm>
#include <set>

#include "firstround_select.h"


namespace mue {

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

}


void
mue::Firstround_team_selection::_sort_filter_teams(Distance_matrix const &distance_matrix,
						   size_t slice)
{
	for (Team_id host_team = 0; host_team < _round_teams; ++host_team) {
		std::vector<Candidate> guest_candidates;
		guest_candidates.reserve(_round_teams * _round_teams);

		for (Team_id firststep_guest = _round_teams;
		     firststep_guest < 2 * _round_teams;
		     ++firststep_guest)
		{
			Distance firststep_distance = distance_matrix.lookup(host_team,
									     firststep_guest);
			if (firststep_distance > _max_distance)
				continue;

			std::vector<Candidate> second_candidates;

			for (Team_id secondstep_guest = 2 * _round_teams;
			     secondstep_guest < 3 * _round_teams;
			     ++secondstep_guest)
			{
				Distance secondstep_distance = distance_matrix.lookup(firststep_guest,
										      secondstep_guest);
				if (secondstep_distance > _max_distance)
					continue;

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
			std::for_each(guest_candidates.begin(),
				      guest_candidates.begin() + std::min(slice, guest_candidates.size()),
					[ & ] (Candidate const &c)
					{ _candidates[host_team].emplace_back(c.team); });

	}

}


mue::Firstround_team_selection::Firstround_team_selection(Distance_matrix const &distance_matrix,
							  Distance max_distance,
							  size_t slice)
:
	_teamcount(distance_matrix.teamcount()),
	_round_teams(_teamcount / 3),
	// ToDo: review this
	_max_distance(302500),
	_candidates(_teamcount / 3)
{
	(void)max_distance;
	BOOST_ASSERT(slice < _round_teams * 2);

	_sort_filter_teams(distance_matrix, slice);

	// Put back "missinng" (optimized out) teams

	// Make a set of "guests"
	std::set<Team_id> present;
	for (Team_id t = _round_teams; t < _teamcount; ++t) {
		present.insert(t);
	}

	// filter out present teams
	for (auto const &teams : _candidates) {
		for (Team_id const &team : teams) {
			present.erase(team);
		}
	}

	// Iterate missing teams
	for (Team_id const missing_team : present) {
		std::vector<Candidate> candidates;

		// Iterate hosts
		for (Team_id host = 0; host < _round_teams; ++host) {
			if (missing_team < 2 * _round_teams) {
				// main round team
				candidates.emplace_back(distance_matrix.lookup(host, missing_team), host);
			} else {
				// dessert team - we have one potential hop - so build the
				// distances over each potential hop
				for (Team_id hop = _round_teams; hop < 2 * _round_teams; ++hop) {
					Distance dist = distance_matrix.lookup(host, hop)
							+ distance_matrix.lookup(hop, missing_team);
					candidates.emplace_back(dist, host);
				}
			}
		}

		// Put "best" candidate in the candidate list
		BOOST_ASSERT(candidates.size() != 0);
		std::sort(candidates.begin(), candidates.end());
		_candidates[candidates[0].team].push_back(missing_team);
	}
}
