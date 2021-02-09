#pragma once

#ifndef CvDLLPlotBuilderIFaceBase_h
#define CvDLLPlotBuilderIFaceBase_h

//
// abstract interface for CvPlotBuilder functions used by DLL
//
class CvEntity;
class CvPlotBuilder;
class cvInternalGlobals;
class CvDLLUtilityIFaceBase;
class CvDLLEntityIFaceBase;

class CvDLLPlotBuilderIFaceBase : public CvDLLEntityIFaceBase
{
public:
	virtual void init(CvPlotBuilder*, CvPlot*) = 0;
	virtual CvPlotBuilder* create()  = 0;

	// derived methods
	virtual void destroy(CvPlotBuilder*& pPlotBuilder, bool bSafeDelete=true) {
		gDLL->getEntityIFace()->destroyEntity((CvEntity*&)pPlotBuilder, bSafeDelete);
	}
};

#endif	// CvDLLPlotBuilderIFaceBase_h
