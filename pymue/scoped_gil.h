#ifndef PYMUE__SCOPED_GIL_H
#define PYMUE__SCOPED_GIL_H

#include <boost/python.hpp>

namespace pymue {

	class ScopedGILRelease
	{
		private:
			PyThreadState * _thread_state;

		public:
			ScopedGILRelease();
			~ScopedGILRelease();
	};
}


#endif
