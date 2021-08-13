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

#include "CvGameCoreDLL.h"
#include "CvMapGenerator.h"
#include "CyMapGenerator.h"
#include "CyPlot.h"

CyMapGenerator::CyMapGenerator() : m_pMapGenerator(NULL)
{
	m_pMapGenerator = &CvMapGenerator::GetInstance();
}

//CyMapGenerator::CyMapGenerator(CvMapGenerator* pMapGenerator) : m_pMapGenerator(pMapGenerator)
//{
//}

void CyMapGenerator::doRiver(CyPlot* pStartPlot, CardinalDirectionTypes eCardinalDirection)
{
	m_pMapGenerator->doRiver(pStartPlot->getPlot(), eCardinalDirection);
}

void CyMapGenerator::addFeatures()
{
	m_pMapGenerator->addFeatures();
}

void CyMapGenerator::addBonuses()
{
	m_pMapGenerator->addBonuses();
}

void CyMapGenerator::generatePlotTypes()
{
	m_pMapGenerator->generatePlotTypes();
}

void CyMapGenerator::generateTerrain()
{
	m_pMapGenerator->generateTerrain();
}

void CyMapGenerator::setPlotTypes(const python::list& lPlotTypes)
{
	std::vector<int> v;
	python::container_utils::extend_container(v, lPlotTypes);
	m_pMapGenerator->setPlotTypes(v);
}
