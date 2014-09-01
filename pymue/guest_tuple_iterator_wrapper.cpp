#include "guest_tuple_iterator_wrapper.h"

#include "conversion_helper.h"

#include "teams.h"


pymue::Guest_tuple_generator_wrapper::Guest_tuple_generator_wrapper(
		boost::python::list const &guests,
		boost::python::list const &used)
: _generator(pymue::translate_vector<mue::Team_id>(guests),
	     pymue::translate_set<mue::Team_id>(used))
{ }
