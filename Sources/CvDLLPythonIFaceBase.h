#pragma once

#ifndef CvDLLPythonIFaceBase_h__
#define CvDLLPythonIFaceBase_h__

//
// abstract interface for Python functions used by DLL
// Creator - Mustafa Thamer
// Copyright 2005 Firaxis Games
//

class CvUnit;
class CvPlot;
class CvDLLPythonIFaceBase
{
public:

	virtual bool isInitialized() = 0;

	virtual const char* getMapScriptModule() = 0;

	template <typename T>
	static PyObject* makePythonObject(T*);

	virtual PyObject* MakeFunctionArgs(void** args, int argc) = 0;

	virtual bool moduleExists(const char* moduleName, bool bLoadIfNecessary) = 0;

	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg=NULL) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, long* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, CvString* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, CvWString* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<uint8_t>* pList) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<int> *pIntList) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, int* pIntList, int* iListSize) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<float> *pFloatList) = 0;

	virtual bool callPythonFunction(const char* szModName, const char* szFxnName, int iArg, long* result) = 0; // HELPER version that handles 1 arg for you

	virtual bool pythonUsingDefaultImpl() = 0;
};


/* Use CvPython::call and CyArgsList() << operation like so:

std::vector<int> arr = CvPython::call(PYGameModule, "getOrderArray", CyArgsList() << arg1 << arg2 << CyArrayArg(enabled, 4));
or
std::vector<int> arr = CvPython::call(PYGameModule, "getOrderArray", arg1, arg2, CyArrayArg(enabled, 4));
*/

template <typename T>
PyObject* CvDLLPythonIFaceBase::makePythonObject(T* pObj)
{
	if (!pObj)
		return Py_None;

	python::object bpo(pObj);
	PyObject* pyobj = bpo.ptr();
	Py_INCREF(pyobj);
	FAssert(pyobj->ob_refcnt == 2);
	return pyobj;	// decrefs pyobj when bpo goes out of scope
}

#endif // CvDLLPythonIFaceBase_h__
