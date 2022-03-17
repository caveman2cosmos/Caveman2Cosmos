#pragma once

#ifndef CvGameCoreDLLUndefNew_h__
#define CvGameCoreDLLUndefNew_h__

#ifdef USE_MEMMANAGER
#undef new
#undef malloc
#undef realloc
#undef _msize
#undef free
#endif

#endif // CvGameCoreDLLUndefNew_h__
