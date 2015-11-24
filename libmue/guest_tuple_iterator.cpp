#include "guest_tuple_iterator.h"

#include "teams.h"


mue::Guest_tuple_generator::Guest_tuple_generator(std::vector<Team_id> const& guests, Used_bits const& used)
:
	_possible_guests(guests),
	_guest_end(
		std::remove_if(_possible_guests.begin(), _possible_guests.end(),
			[used](const Team_id t) -> bool { return used[t]; }))
{ }

mue::Guest_tuple_generator::Guest_tuple_generator(Guests const& guests, Used_bits const& used)
:
	_possible_guests(guests),
	_guest_end(
		std::remove_if(_possible_guests.begin(), _possible_guests.end(),
			[used](const Team_id t) -> bool { return used[t]; }))
{ }

std::ostream& mue::operator<< (std::ostream& os, Guest_tuple_generator::GuestPair const& p)
{
	return os << "[" << ((int)p.first) << ", " << ((int)p.second) << "]";
}
