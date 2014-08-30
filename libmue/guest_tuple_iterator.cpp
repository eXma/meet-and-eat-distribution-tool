#include "guest_tuple_iterator.h"

#include "teams.h"


mue::Guest_tuple_generator::Guest_tuple_generator(std::vector<Team_id> const& guests, std::unordered_set<Team_id> const& used)
:
	_possible_guests(guests),
	_guest_end(
		std::remove_if(_possible_guests.begin(), _possible_guests.end(),
			[used](const Team_id t) -> bool { return used.find(t) != used.end(); }))
{}

std::ostream& mue::operator<< (std::ostream& os, Guest_tuple_generator::GuestPair const& p)
{
	return os << "[" << (p.first) << ", " << (p.second) << "]";
}
