#pragma once

#include "CyArgsList.h"

namespace CvPython
{
	template < class ReturnValueTy_ >
	ReturnValueTy_ call(const char* const moduleName, const char* const functionName, const CyArgsList& args )
	{
		ReturnValueTy_ rval;
		bool bOK = IFPPythonCall(moduleName, functionName, args.makeFunctionArgs(), &rval);
		return rval;
	}
}