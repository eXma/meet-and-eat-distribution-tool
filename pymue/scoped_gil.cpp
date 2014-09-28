#include "scoped_gil.h"

pymue::ScopedGILRelease::ScopedGILRelease()
{
	_thread_state = PyEval_SaveThread();
}

pymue::ScopedGILRelease::~ScopedGILRelease()
{
	PyEval_RestoreThread(_thread_state);
        _thread_state = NULL;
}
