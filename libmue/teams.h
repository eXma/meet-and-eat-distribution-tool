#ifndef MUE__TEAMS_H
#define MUE__TEAMS_H


#include <iostream>

#include "config.h"


namespace mue {


class Team {
	private:
		Team_id _id;

		Team() {}
	public:
		explicit Team(Team_id id) noexcept
		: _id(id)
		{}

		inline bool operator<(const Team& other)  const { return _id < other._id; }
		inline bool operator>(const Team& other)  const { return _id > other._id; }
		inline bool operator<=(const Team& other) const { return _id <= other._id; }
		inline bool operator>=(const Team& other) const { return _id >= other._id; }
		inline bool operator==(const Team& other) const { return _id == other._id; }
		inline bool operator!=(const Team& other) const { return _id != other._id; }

		inline operator Team_id() const { return _id; }
		inline Team_id id() const { return _id; }

};

std::ostream& operator<<(std::ostream& os, Team const& team) { return os << int(team.id()); }

}
#endif
