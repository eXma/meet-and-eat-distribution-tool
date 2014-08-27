
#include "teams.h"


std::ostream& mue::operator<<(std::ostream& os, mue::Team const& team)
{
       return os << int(team.id());
}
