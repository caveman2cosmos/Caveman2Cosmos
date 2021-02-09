#include "CvGameCoreDLL.h"

uint64_t SCvDebug::m_mode = 0;

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
		m_mode &= ~((uint64_t)1 << mode);
}
