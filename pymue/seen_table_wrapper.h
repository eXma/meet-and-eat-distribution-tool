#ifndef PYMUE_SEEN_TABLE_WRAPPER
#define PYMUE_SEEN_TABLE_WRAPPER

#include <memory>

#include "seen_table.h"

namespace pymue {


	class Seen_table_wrapper
	{
		private:
			std::shared_ptr<mue::Seen_table> _seen_table;

			Seen_table_wrapper(mue::Seen_table&& seen_table)
			: _seen_table(new mue::Seen_table(std::move(seen_table)))
			{ }

		public:
			Seen_table_wrapper(int num_teams)
			: _seen_table(new mue::Seen_table(num_teams))
			{ }

			Seen_table_wrapper clone() const { return Seen_table_wrapper(_seen_table->clone()); }

			int generation() const { return _seen_table->generation(); }
			bool seen(mue::Team_id a, mue::Team_id b, mue::Team_id c) const { return _seen_table->seen(a, b, c); }
			void add_meeting(mue::Team_id a, mue::Team_id b, mue::Team_id c) { _seen_table->add_meeting(a, b, c); }
			
	};
}


#endif
