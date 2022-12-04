#pragma once

#ifndef ASSERT_DIALOG_H
#define ASSERT_DIALOG_H

#include "FAssert.h"

class AssertDialog
{
public:
	// These are the return values from the modal Assert Dialog
	enum Result
	{
		ASSERT_DLG_DEBUG = 0x01,
		ASSERT_DLG_IGNORE = 0x02,
		ASSERT_DLG_IGNOREALWAYS = 0x03,
		ASSERT_DLG_EXIT = 0x04,
	};

	static Result display(const FAssertInfo& assertInfo);
};

#endif

