#pragma once

#ifndef FASSERT_DIALOG_H
#define FASSERT_DIALOG_H

#include "FAssert.h"

class FAssertDialog
{
public:

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

