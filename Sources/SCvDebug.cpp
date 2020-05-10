#include "CvGameCoreDLL.h"

unsigned __int64 SCvDebug::m_mode = 0;

bool SCvDebug::checkMode(Mode mode)
{
	FAssert(mode >= 0 && mode < 64);
	return (m_mode & (1 << mode)) != 0;
}

void SCvDebug::setMode(Mode mode, bool on)
{
	FAssert(mode >= 0 && mode < 64);
	if (on)
		m_mode |= 1 << mode;
	else
		m_mode &= ~((unsigned __int64)1 << mode);
}
