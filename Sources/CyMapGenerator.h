#pragma once

#ifndef CyMapGenerator_h
#define CyMapGenerator_h

//
//	FILE:	 CyMapGenerator.h
//	AUTHOR:  Mustafa Thamer
//	PURPOSE:
//			Python wrapper class for CvMapGenerator
//
//-----------------------------------------------------------------------------
//	Copyright (c) 2005 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//

class CvMapGenerator;
class CyPlot;

class CyMapGenerator
{
public:
	CyMapGenerator();
	//explicit CyMapGenerator(CvMapGenerator* pMapGenerator);		// Call from C++

	void doRiver(CyPlot* pStartPlot, CardinalDirectionTypes eCardinalDirection);
	void addFeatures();
	void addBonuses();

	void generatePlotTypes();
	void generateTerrain();

	void setPlotTypes(const python::list& lPlotTypes);

protected:
	CvMapGenerator* m_pMapGenerator;
};

#endif	// CyMapGenerator_h
