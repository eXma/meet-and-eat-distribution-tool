#ifndef MUE_DISTANCES_H
#define MUE_DISTANCES_H


#include <vector>


#include <teams.h>


namespace mue {

	typedef float Distance;

	class Distance_matrix
	{
		private:
			std::vector<std::vector<Distance> > _table;
			unsigned int _teamcount;
			Distance     _min_cost;

		public:
			Distance_matrix(int teamcount) noexcept;

			void set_cost(Team_id const source, Team_id const destination, Distance cost);

			Distance lookup(Team_id const source, Team_id const destination) const noexcept
			{
				return _table[source][destination];
			}

			int teamcount() const { return _teamcount; }

			Distance min_cost() const { return _min_cost; }
	};
}


#endif
