#pragma once


#ifndef BaseUnitAiComponent__h_
#define BaseUnitAiComponent__h_

#include "CvUnitAI.h"

class BaseUnitAiComponent
{
public:
	BaseUnitAiComponent(CvUnitAI* unit);
	
	virtual bool Run();
protected:
	char* m_unitAiType;
};

#endif