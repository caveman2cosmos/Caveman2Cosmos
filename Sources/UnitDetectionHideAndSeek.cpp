#include "CvGameCoreDLL.h"
#include "UnitDetectionHideAndSeek.h"

CvWString UnitDetectionHideAndSeek::getToolTipText() const
{
	std::vector<CvWString> lines;
	return bst::algorithm::join(lines, NEWLINE);
}
