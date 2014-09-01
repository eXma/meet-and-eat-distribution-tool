#ifndef PYMUE_GUEST_TUPLE_ITERATOR_WRAPPER
#define PYMUE_GUEST_TUPLE_ITERATOR_WRAPPER

#include <vector>
#include <list>
#include <unordered_set>

#include <boost/python/list.hpp>

#include "guest_tuple_iterator.h"

namespace pymue {

	class Guest_tuple_generator_wrapper
	{
		private :
			mue::Guest_tuple_generator _generator;

		public:
			class Iterator_wrapper : public std::iterator<std::input_iterator_tag, const mue::Guest_tuple_generator::GuestPair> {
				private:
				       	mue::Guest_tuple_generator::Iterator _iterator;
					std::list<mue::Guest_tuple_generator::GuestPair> _pair_cache;
					bool _saved;

					inline mue::Guest_tuple_generator::GuestPair const & _last_generated()
					{
						if (!_saved)
							_pair_cache.push_back(*_iterator);
						return _pair_cache.back();
					}

				public:
					Iterator_wrapper(mue::Guest_tuple_generator::Iterator const &iterator)
					:
						_iterator(iterator),
						_saved(false)
					{ }

					inline Iterator_wrapper& operator++()
					{
						++_iterator;
						_saved = false;
						return *this;
					}

					inline Iterator_wrapper operator++(int)
					{
						Iterator_wrapper tmp(*this);
						++_iterator;
						_saved = false;
						return tmp;
					}

					inline const mue::Guest_tuple_generator::GuestPair& operator* ()
					{
						return _last_generated();	
					}

					inline const mue::Guest_tuple_generator::GuestPair * operator-> ()
					{
						return &_last_generated();
					}

					inline bool operator== (Iterator_wrapper const& other) const { return _iterator == other._iterator; }
					inline bool operator!= (Iterator_wrapper const& other) const { return !(*this == other); }

			};

			typedef Iterator_wrapper iterator;

			Guest_tuple_generator_wrapper(boost::python::list const &guests,
						      boost::python::list const &used);


			Iterator_wrapper begin() const { return Iterator_wrapper(_generator.begin()); }
			Iterator_wrapper end() const { return Iterator_wrapper(_generator.end()); }
	};
}


#endif

