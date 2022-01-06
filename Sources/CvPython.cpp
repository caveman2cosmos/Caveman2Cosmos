#include "CvGameCoreDLL.h"

#include "CvPython.h"

#include "Python.h"
#include "frameobject.h"

namespace Cy {;

	namespace {;
		void trace_thread(PyThreadState* st, std::vector<StackFrame>& trace)
		{
			st->curexc_traceback = NULL;

			for (PyFrameObject* frm = st->frame; frm; frm = frm->f_back)
			{
				if (PyTraceBack_Here(frm) < 0)
				{
					//fprintf(stderr, "  PyTraceBack_Here failed\n");
					return;
				}
			}

			PyTracebackObject* tb = (PyTracebackObject*)st->curexc_traceback;
			if (tb == NULL)
			{
				//fprintf(stderr, "  traceback is NULL in wxFatalSignalHandler\n");
				return;
			}

			while (tb)
			{
				trace.push_back(StackFrame(PyString_AsString(tb->tb_frame->f_code->co_filename), tb->tb_lineno, PyString_AsString(tb->tb_frame->f_code->co_name)));
				tb = tb->tb_next;
			}
		}
	}


	std::vector<StackFrame> get_stack_trace()
	{
		std::vector<StackFrame> trace;

		PyInterpreterState* firstInterp = PyInterpreterState_Head();

		if (firstInterp)
		{
			PyGILState_STATE state = PyGILState_Ensure();

			PyThreadState* curThread = PyThreadState_GET();
			if (curThread)
			{
				trace_thread(curThread, trace);
			}

			for (PyInterpreterState* interp = firstInterp; interp; interp = interp->next)
			{
				if (firstInterp->next)
				{
					//fprintf(stderr, "Interpreter:\n");
				}
				for (PyThreadState* st = interp->tstate_head; st; st = st->next)
				{
					if (st == curThread)
					{
						continue;
					}
					trace_thread(st, trace);
				}
			}
			PyGILState_Release(state);
		}
		return trace;
	}
}