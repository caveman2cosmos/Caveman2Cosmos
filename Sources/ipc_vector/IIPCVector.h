#pragma once
#include <ObjBase.h>

// {5219B44A-0874-449E-8611-B7080DBFA6AB}
static const GUID IID_IIPCVector = { 0x5219b44a, 0x874, 0x449e, { 0x86, 0x11, 0xb7, 0x8, 0xd, 0xbf, 0xa6, 0xab } };

interface IIPCVector : IUnknown
{
	virtual void __stdcall getAt(const int index, int& value) const = 0;
	virtual int __stdcall size() const = 0;
	virtual void __stdcall push_back(int value) = 0;
};
