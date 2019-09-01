//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvGameObject.cpp
//
//  PURPOSE: Wrapper classes for Civ4 game objects
//
//------------------------------------------------------------------------------------------------

#include "CvGameCoreDLL.h"
#include <boost/bind.hpp>

CvGameObjectGame::CvGameObjectGame()
{
}

CvGameObjectTeam::CvGameObjectTeam(CvTeam *pTeam)
{
	m_pTeam = pTeam;
}

CvGameObjectPlayer::CvGameObjectPlayer(CvPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

CvGameObjectCity::CvGameObjectCity(CvCity* pCity)
{
	m_pCity = pCity;
}

CvGameObjectUnit::CvGameObjectUnit(CvUnit* pUnit)
{
	m_pUnit = pUnit;
}

CvGameObjectPlot::CvGameObjectPlot(CvPlot* pPlot)
{
	m_pPlot = pPlot;
}

GameObjectTypes CvGameObjectGame::getGameObjectType() const
{
	return GAMEOBJECT_GAME;
}

GameObjectTypes CvGameObjectTeam::getGameObjectType() const
{
	return GAMEOBJECT_TEAM;
}

GameObjectTypes CvGameObjectPlayer::getGameObjectType() const
{
	return GAMEOBJECT_PLAYER;
}

GameObjectTypes CvGameObjectCity::getGameObjectType() const
{
	return GAMEOBJECT_CITY;
}

GameObjectTypes CvGameObjectUnit::getGameObjectType() const
{
	return GAMEOBJECT_UNIT;
}

GameObjectTypes CvGameObjectPlot::getGameObjectType() const
{
	return GAMEOBJECT_PLOT;
}

CvProperties* CvGameObjectGame::getProperties() const
{
	return GC.getGameINLINE().getProperties();
}

const CvProperties* CvGameObjectGame::getPropertiesConst() const
{
	return GC.getGameINLINE().getPropertiesConst();
}

CvProperties* CvGameObjectTeam::getProperties() const
{
	return m_pTeam->getProperties();
}

const CvProperties* CvGameObjectTeam::getPropertiesConst() const
{
	return m_pTeam->getPropertiesConst();
}

CvProperties* CvGameObjectPlayer::getProperties() const
{
	return m_pPlayer->getProperties();
}

const CvProperties* CvGameObjectPlayer::getPropertiesConst() const
{
	return m_pPlayer->getPropertiesConst();
}

CvProperties* CvGameObjectCity::getProperties() const
{
	return m_pCity->getProperties();
}

const CvProperties* CvGameObjectCity::getPropertiesConst() const
{
	return m_pCity->getPropertiesConst();
}

CvProperties* CvGameObjectUnit::getProperties() const
{
	return m_pUnit->getProperties();
}

const CvProperties* CvGameObjectUnit::getPropertiesConst() const
{
	return m_pUnit->getPropertiesConst();
}

CvProperties* CvGameObjectPlot::getProperties() const
{
	return m_pPlot->getProperties();
}

const CvProperties* CvGameObjectPlot::getPropertiesConst() const
{
	return m_pPlot->getPropertiesConst();
}

// helper function to call foreach on the object
void callForeach(CvGameObject* pObject, GameObjectTypes eType, boost::function<void (CvGameObject*)> func)
{
	pObject->foreach(eType, func);
}

void CvGameObjectGame::foreach(GameObjectTypes eType, boost::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_TEAM:
			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					func(GET_TEAM((TeamTypes)iI).getGameObject());
				}
			}
			break;

		case GAMEOBJECT_PLAYER:
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
				if (kLoopPlayer.isAlive())
				{
					func(kLoopPlayer.getGameObject());
				}
			}
			break;

		case GAMEOBJECT_CITY:
			foreach(GAMEOBJECT_PLAYER, boost::bind(callForeach, _1, GAMEOBJECT_CITY, func));
			break;

		case GAMEOBJECT_UNIT:
			foreach(GAMEOBJECT_PLAYER, boost::bind(callForeach, _1, GAMEOBJECT_UNIT, func));
			break;

		case GAMEOBJECT_PLOT:
			for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
			{
				func(GC.getMapINLINE().plotByIndexINLINE(iI)->getGameObject());
			}
			break;

		case GAMEOBJECT_GAME:
			func(this);
			break;
	}
}

void CvGameObjectTeam::foreach(GameObjectTypes eType, boost::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGameINLINE().getGameObject());
			break;

		case GAMEOBJECT_PLAYER:
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
				if (kLoopPlayer.isAlive())
				{
					if (kLoopPlayer.getTeam() == m_pTeam->getID())
					{
						func((CvGameObject*)&CvGameObjectPlayer(&kLoopPlayer));
					}
				}
			}
			break;

		case GAMEOBJECT_CITY:
			foreach(GAMEOBJECT_PLAYER, boost::bind(callForeach, _1, GAMEOBJECT_CITY, func));
			break;

		case GAMEOBJECT_UNIT:
			foreach(GAMEOBJECT_PLAYER, boost::bind(callForeach, _1, GAMEOBJECT_UNIT, func));
			break;

		case GAMEOBJECT_PLOT:
			for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
				if (pLoopPlot->getTeam() == m_pTeam->getID())
				{
					func(pLoopPlot->getGameObject());
				}
			}
			break;

		case GAMEOBJECT_TEAM:
			func(this);
			break;
	}
}

void CvGameObjectPlayer::foreach(GameObjectTypes eType, boost::function<void (CvGameObject*)> func)
{
	int iLoop;
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGameINLINE().getGameObject());
			break;

		case GAMEOBJECT_TEAM:
			func(GET_TEAM(m_pPlayer->getTeam()).getGameObject());
			break;

		case GAMEOBJECT_CITY:
			for (CvCity* pCity = m_pPlayer->firstCity(&iLoop); pCity != NULL; pCity = m_pPlayer->nextCity(&iLoop))
			{
				func(pCity->getGameObject());
			}
			break;

		case GAMEOBJECT_UNIT:
			for (CvUnit* pUnit = m_pPlayer->firstUnit(&iLoop); pUnit != NULL; pUnit = m_pPlayer->nextUnit(&iLoop))
			{
				func(pUnit->getGameObject());
			}
			break;

		case GAMEOBJECT_PLOT:
			for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
				if (pLoopPlot->getOwnerINLINE() == m_pPlayer->getID())
				{
					func(pLoopPlot->getGameObject());
				}
			}
			break;

		case GAMEOBJECT_PLAYER:
			func(this);
			break;
	}
}

void CvGameObjectCity::foreach(GameObjectTypes eType, boost::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGameINLINE().getGameObject());
			break;

		case GAMEOBJECT_TEAM:
			func(GET_TEAM(m_pCity->getTeam()).getGameObject());
			break;

		case GAMEOBJECT_PLAYER:
			func(GET_PLAYER(m_pCity->getOwnerINLINE()).getGameObject());
			break;

		case GAMEOBJECT_UNIT:
			m_pCity->plot()->getGameObject()->foreach(eType, func);
			break;

		case GAMEOBJECT_PLOT:
			func(m_pCity->plot()->getGameObject());
			break;

		case GAMEOBJECT_CITY:
			func(this);
			break;
	}
}

void CvGameObjectUnit::foreach(GameObjectTypes eType, boost::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGameINLINE().getGameObject());
			break;

		case GAMEOBJECT_TEAM:
			func(GET_TEAM(m_pUnit->getTeam()).getGameObject());
			break;

		case GAMEOBJECT_PLAYER:
			func(GET_PLAYER(m_pUnit->getOwnerINLINE()).getGameObject());
			break;

		case GAMEOBJECT_CITY:
			m_pUnit->plot()->getGameObject()->foreach(eType, func);
			break;

		case GAMEOBJECT_PLOT:
			func(m_pUnit->plot()->getGameObject());
			break;

		case GAMEOBJECT_UNIT:
			func(this);
			break;
	}
}

void CvGameObjectPlot::foreach(GameObjectTypes eType, boost::function<void (CvGameObject*)> func)
{
	switch(eType)
	{
		case GAMEOBJECT_GAME:
			func(GC.getGameINLINE().getGameObject());
			break;

		case GAMEOBJECT_TEAM:
			if (m_pPlot->getTeam() != NO_TEAM)
				func(GET_TEAM(m_pPlot->getTeam()).getGameObject());
			break;

		case GAMEOBJECT_PLAYER:
			if (m_pPlot->getOwnerINLINE() != NO_PLAYER)
				func(GET_PLAYER(m_pPlot->getOwnerINLINE()).getGameObject());
			break;

		case GAMEOBJECT_CITY:
			if (m_pPlot->getPlotCity())
				func(m_pPlot->getPlotCity()->getGameObject());
			break;

		case GAMEOBJECT_PLOT:
			func(this);
			break;

		case GAMEOBJECT_UNIT:
			CLLNode<IDInfo>* pUnitNode = m_pPlot->headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = m_pPlot->nextUnitNode(pUnitNode);
				func(pUnit->getGameObject());
			}
			break;
	}
}

CvGameObjectPlayer* CvGameObjectGame::getOwner()
{
	return NULL;
}

CvGameObjectPlayer* CvGameObjectTeam::getOwner() 
{
	return NULL;
}

CvGameObjectPlayer* CvGameObjectPlayer::getOwner() 
{
	return this;
}

CvGameObjectPlayer* CvGameObjectCity::getOwner() 
{
	return GET_PLAYER(m_pCity->getOwnerINLINE()).getGameObject();
}

CvGameObjectPlayer* CvGameObjectUnit::getOwner() 
{
	return GET_PLAYER(m_pUnit->getOwnerINLINE()).getGameObject();
}

CvGameObjectPlayer* CvGameObjectPlot::getOwner() 
{
	if (m_pPlot->getOwnerINLINE() != NO_PLAYER)
		return GET_PLAYER(m_pPlot->getOwnerINLINE()).getGameObject();
	return NULL;
}

CvGameObjectTeam* CvGameObjectGame::getTeam() 
{
	return NULL;
}

CvGameObjectTeam* CvGameObjectTeam::getTeam() 
{
	return this;
}

CvGameObjectTeam* CvGameObjectPlayer::getTeam() 
{
	return GET_TEAM(m_pPlayer->getTeam()).getGameObject();
}

CvGameObjectTeam* CvGameObjectCity::getTeam() 
{
	return GET_TEAM(m_pCity->getTeam()).getGameObject();
}

CvGameObjectTeam* CvGameObjectUnit::getTeam() 
{
	return GET_TEAM(m_pUnit->getTeam()).getGameObject();
}

CvGameObjectTeam* CvGameObjectPlot::getTeam() 
{
	if (m_pPlot->getTeam() != NO_TEAM)
		return GET_TEAM(m_pPlot->getTeam()).getGameObject();
	return NULL;
}

CvGameObjectPlot* CvGameObjectGame::getPlot() 
{
	return NULL;
}

CvGameObjectPlot* CvGameObjectTeam::getPlot() 
{
	return NULL;
}

CvGameObjectPlot* CvGameObjectPlayer::getPlot() 
{
	return NULL;
}

CvGameObjectPlot* CvGameObjectCity::getPlot() 
{
	return m_pCity->plot()->getGameObject();
}

CvGameObjectPlot* CvGameObjectUnit::getPlot() 
{
	return m_pUnit->plot()->getGameObject();
}

CvGameObjectPlot* CvGameObjectPlot::getPlot() 
{
	return this;
}

void CvGameObject::foreachOn(GameObjectTypes eType, boost::function<void(CvGameObject *)> func)
{
	CvGameObjectPlot* pPlot = getPlot();
	if (pPlot)
		pPlot->foreachOn(eType, func);
}

void CvGameObject::foreachNear(GameObjectTypes eType, boost::function<void(CvGameObject *)> func, int iDistance)
{
	CvGameObjectPlot* pPlot = getPlot();
	if (pPlot)
		pPlot->foreachNear(eType, func, iDistance);
}

void CvGameObject::foreachRelated(GameObjectTypes eType, RelationTypes eRelation, boost::function<void(CvGameObject *)> func, int iData)
{
	switch (eRelation)
	{
		case RELATION_ASSOCIATED:
			foreach(eType, func);
			break;
		case RELATION_NEAR:
			foreachNear(eType, func, iData);
			break;
		case RELATION_SAME_PLOT:
			foreachOn(eType, func);
			break;
	}
}

// helper function to call function if expression true
void callFuncIf(CvGameObject* pObject, BoolExpr* pExpr, boost::function<void (CvGameObject*)> func)
{
	if (pExpr->evaluate(pObject))
		func(pObject);
}

void CvGameObject::foreachRelatedCond(GameObjectTypes eType, RelationTypes eRelation, boost::function<void(CvGameObject *)> func, BoolExpr* pExpr, int iData)
{
	if (pExpr)
	{
		foreachRelated(eType, eRelation, boost::bind(callFuncIf, _1, pExpr, func), iData);
	}
	else
	{
		foreachRelated(eType, eRelation, func, iData);
	}
}

//helper function to add game object to vector
void addToVector(CvGameObject* pObject, std::vector<CvGameObject*> *kEnum)
{
	kEnum->push_back(pObject);
}

void CvGameObject::enumerate(std::vector<CvGameObject*> &kEnum, GameObjectTypes eType)
{
	foreach(eType, boost::bind(addToVector, _1, &kEnum));
}

void CvGameObject::enumerateOn(std::vector<CvGameObject*> &kEnum, GameObjectTypes eType)
{
	foreachOn(eType, boost::bind(addToVector, _1, &kEnum));
}

void CvGameObject::enumerateNear(std::vector<CvGameObject*> &kEnum, GameObjectTypes eType, int iDistance)
{
	foreachNear(eType, boost::bind(addToVector, _1, &kEnum), iDistance);
}

void CvGameObject::enumerateRelated(std::vector<CvGameObject*>& kEnum, GameObjectTypes eType, RelationTypes eRelation, int iData)
{
	foreachRelated(eType, eRelation, boost::bind(addToVector, _1, &kEnum), iData);
}

void CvGameObject::enumerateRelatedCond(std::vector<CvGameObject*>& kEnum, GameObjectTypes eType, RelationTypes eRelation, BoolExpr* pExpr, int iData)
{
	foreachRelatedCond(eType, eRelation, boost::bind(addToVector, _1, &kEnum), pExpr, iData);
}

void CvGameObjectPlot::foreachOn(GameObjectTypes eType, boost::function<void(CvGameObject *)> func)
{
	if (eType == GAMEOBJECT_PLOT)
	{
		func(this);
	}
	else
	{
		foreach(eType, func);
	}
}

void CvGameObjectPlot::foreachNear(GameObjectTypes eType, boost::function<void(CvGameObject *)> func, int iDistance)
{
	int iPlotX = m_pPlot->getX_INLINE();
	int iPlotY = m_pPlot->getY_INLINE();
	
	for (int iX=iPlotX - iDistance; iX <= iPlotX + iDistance; iX++)
	{
		for (int iY=iPlotY - iDistance; iY <= iPlotY + iDistance; iY++)
		{
			CvPlot* pPlot = GC.getMapINLINE().plotINLINE(iX, iY);
			if (pPlot)
				pPlot->getGameObject()->foreachOn(eType, func);
		}
	}
}

void CvGameObjectCity::foreachRelated(GameObjectTypes eType, RelationTypes eRelation, boost::function<void(CvGameObject *)> func, int iData)
{
	if (eRelation == RELATION_TRADE)
	{
		if (eType == GAMEOBJECT_CITY)
		{
			int iRoutes = m_pCity->getTradeRoutes();
			for (int i=0; i<iRoutes; i++)
			{
				CvCity* pTradeCity = m_pCity->getTradeCity(i);
				if (pTradeCity)
				{
					func(pTradeCity->getGameObject());
				}
			}
		}
	}
	else if (eRelation == RELATION_WORKING)
	{
		if (eType == GAMEOBJECT_PLOT)
		{
			for (int iI = 0; iI < m_pCity->getNumCityPlots(); iI++)
			{
				CvPlot* pLoopPlot = plotCity(m_pCity->getX_INLINE(), m_pCity->getY_INLINE(), iI);
				if (pLoopPlot)
				{
					if (pLoopPlot->getWorkingCity() == m_pCity)
					{
						func(pLoopPlot->getGameObject());
					}
				}
			}
		}
	}
	else
	{
		CvGameObject::foreachRelated(eType, eRelation, func, iData);
	}
}

void CvGameObjectPlot::foreachRelated(GameObjectTypes eType, RelationTypes eRelation, boost::function<void(CvGameObject *)> func, int iData)
{
	if (eRelation == RELATION_WORKING)
	{
		if (eType == GAMEOBJECT_CITY)
		{
			func(m_pPlot->getWorkingCity()->getGameObject());
		}
	}
	else
	{
		CvGameObject::foreachRelated(eType, eRelation, func, iData);
	}
}

void CvGameObjectGame::foreachManipulator(boost::function<void(CvGameObject *,CvPropertyManipulators *)> func)
{
	// No specific manipulators apply to game, only global ones
}

void CvGameObjectTeam::foreachManipulator(boost::function<void(CvGameObject *,CvPropertyManipulators *)> func)
{
	// No specific manipulators apply to teams, only global ones
}

void CvGameObjectPlayer::foreachManipulator(boost::function<void(CvGameObject *,CvPropertyManipulators *)> func)
{
	// Civics
	for (int i=0; i<GC.getNumCivicOptionInfos(); i++)
	{
		func(this, GC.getCivicInfo(m_pPlayer->getCivics((CivicOptionTypes)i)).getPropertyManipulators());
	}

	// State religion
	if (m_pPlayer->getStateReligion() != NO_RELIGION)
	{
		func(this, GC.getReligionInfo(m_pPlayer->getStateReligion()).getPropertyManipulators());
	}

	// Leader traits
	for (int i=0; i<GC.getNumTraitInfos(); i++)
	{
		//TB Traits begin
		if (m_pPlayer->hasTrait((TraitTypes)i))
		//TB Traits end
		{
			func(this, GC.getTraitInfo((TraitTypes)i).getPropertyManipulators());
		}
	}

	// Handicap
	func(this, GC.getHandicapInfo(m_pPlayer->getHandicapType()).getPropertyManipulators());
}

void CvGameObjectCity::foreachManipulator(boost::function<void(CvGameObject *,CvPropertyManipulators *)> func)
{
	// Building manipulators apply to cities
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if(m_pCity->getNumActiveBuilding((BuildingTypes)iI) > 0)
		{
			//Team Project (5)
			if (!m_pCity->isReligiouslyDisabledBuilding((BuildingTypes)iI))
			{
				func(this, GC.getBuildingInfo((BuildingTypes)iI).getPropertyManipulators());
			}
		}
	}

	// Religions
	for (int i=0; i< GC.getNumReligionInfos(); i++)
	{
		if (m_pCity->isHasReligion((ReligionTypes)i))
		{
			func(this, GC.getReligionInfo((ReligionTypes)i).getPropertyManipulators());
		}
	}

	// Corporations
	for (int i=0; i< GC.getNumCorporationInfos(); i++)
	{
		if (m_pCity->isHasCorporation((CorporationTypes)i))
		{
			func(this, GC.getCorporationInfo((CorporationTypes)i).getPropertyManipulators());
		}
	}

	// Specialists
	for (int i=0; i< GC.getNumSpecialistInfos(); i++)
	{
		int iCount = m_pCity->getSpecialistCount((SpecialistTypes)i) + m_pCity->getFreeSpecialistCount((SpecialistTypes)i);
		for (int j=0; j<iCount; j++)
		{
			func(this, GC.getSpecialistInfo((SpecialistTypes)i).getPropertyManipulators());
		}
	}
}

void CvGameObjectUnit::foreachManipulator(boost::function<void(CvGameObject *,CvPropertyManipulators *)> func)
{
	// Unit Type
	func(this, m_pUnit->getUnitInfo().getPropertyManipulators());

	// Promotions
	for (int i=0; i<GC.getNumPromotionInfos(); i++)
	{
		if (m_pUnit->isHasPromotion((PromotionTypes)i))
		{
			func(this, GC.getPromotionInfo((PromotionTypes)i).getPropertyManipulators());
		}
	}
}

void CvGameObjectPlot::foreachManipulator(boost::function<void(CvGameObject *,CvPropertyManipulators *)> func)
{
	// Terrain Type
	func(this, GC.getTerrainInfo(m_pPlot->getTerrainType()).getPropertyManipulators());

	// Feature Type
//#ifdef MULTI_FEATURE_MOD
//	for (int i=0; i<m_pPlot->getNumFeatures(); i++)
//	{
//		func(this, GC.getFeatureInfo(m_pPlot->getFeatureByIndex(i)).getPropertyManipulators());
//	}
//#else
	if (m_pPlot->getFeatureType() != NO_FEATURE)
	{
		func(this, GC.getFeatureInfo(m_pPlot->getFeatureType()).getPropertyManipulators());
	}
//#endif

	// Improvement Type
	if (m_pPlot->getImprovementType() != NO_IMPROVEMENT)
	{
		func(this, GC.getImprovementInfo(m_pPlot->getImprovementType()).getPropertyManipulators());
	}

	// Route Type
	if (m_pPlot->getRouteType() != NO_ROUTE)
	{
		func(this, GC.getRouteInfo(m_pPlot->getRouteType()).getPropertyManipulators());
	}

	// Bonus Type
	if (m_pPlot->getBonusType() != NO_BONUS)
	{
		func(this, GC.getBonusInfo(m_pPlot->getBonusType()).getPropertyManipulators());
	}
}

void CvGameObject::eventPropertyChanged(PropertyTypes eProperty, int iNewValue)
{
}

void CvGameObjectCity::eventPropertyChanged(PropertyTypes eProperty, int iNewValue)
{
	//CvString szBuffer;
	CvPropertyInfo& kInfo = GC.getPropertyInfo(eProperty);
	int iNum = kInfo.getNumPropertyBuildings();
	//TB Combat Mods (disease special manifestation and removal system)
	//PropertyTypes eDiseaseType = (PropertyTypes)GC.getInfoTypeForString("PROPERTY_DISEASE");

	//if (eProperty != eDiseaseType)
	//{
	//
	if (!GC.getGame().isOption(GAMEOPTION_OUTBREAKS_AND_AFFLICTIONS) || !kInfo.isOAType())
	{
		//TB Combat Mods end
		if (!GET_PLAYER(m_pCity->getOwner()).isNPC())
		{
			for (int i=0; i<iNum; i++)
			{
				PropertyBuilding& kBuilding = kInfo.getPropertyBuilding(i);
				bool bHasBuilding = m_pCity->getNumRealBuilding(kBuilding.eBuilding) > 0;
				bool bInRange = (iNewValue >= kBuilding.iMinValue) && (iNewValue <= kBuilding.iMaxValue);
				if (!bInRange)
				{
					if (bHasBuilding)
					{//TBWORKINGHERE
						//szBuffer.format("Removing Building, Player %s, Building %s, iNewValue %i.", GET_PLAYER(m_pCity->getOwner()).getNameKey(), GC.getBuildingInfo(kBuilding.eBuilding).getTextKeyWide(), iNewValue);
						//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
						m_pCity->setNumRealBuilding(kBuilding.eBuilding, 0);
					}
				}
				else
				{
					if (m_pCity->canConstruct(kBuilding.eBuilding, false, false, true, true))
					{
						if (!bHasBuilding)
						{
							//szBuffer.format("Adding Building, Player %s, Building %s, iNewValue %i.", GET_PLAYER(m_pCity->getOwner()).getNameKey(), GC.getBuildingInfo(kBuilding.eBuilding).getTextKeyWide(), iNewValue);
							//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
							m_pCity->setNumRealBuilding(kBuilding.eBuilding, 1);
						}
					}
					else
					{
						if (bHasBuilding)
						{
							//szBuffer.format("Removing Building - no longer qualified, Player %s, Building %s, iNewValue %i.", GET_PLAYER(m_pCity->getOwner()).getNameKey(), GC.getBuildingInfo(kBuilding.eBuilding).getTextKeyWide(), iNewValue);
							//gDLL->logMsg("PropertyBuildingOOS.log", szBuffer.c_str(), false, false);
							m_pCity->setNumRealBuilding(kBuilding.eBuilding, 0);
						}
					}
				}
			}
		}
	}
}

void CvGameObjectUnit::eventPropertyChanged(PropertyTypes eProperty, int iNewValue)
{
	PROFILE_FUNC();

	CvPropertyInfo& kInfo = GC.getPropertyInfo(eProperty);
	int iNum = kInfo.getNumPropertyPromotions();

	for (int i=0; i<iNum; i++)
	{
		PropertyPromotion& kPromotion = kInfo.getPropertyPromotion(i);
		bool bHasPromotion = m_pUnit->isHasPromotion(kPromotion.ePromotion);
		bool bInRange = (iNewValue >= kPromotion.iMinValue) && (iNewValue <= kPromotion.iMaxValue);
		if (!bInRange)
		{
			if (bHasPromotion)
			{
				m_pUnit->setHasPromotion(kPromotion.ePromotion, false);
			}
		}
		else
		{
			//TB Combat Mods begin
			bool bEquip = GC.getPromotionInfo(kPromotion.ePromotion).isEquipment();
			bool bAfflict = GC.getPromotionInfo(kPromotion.ePromotion).isAffliction();
			bool bPromote = false;
			if (!bEquip && !bAfflict)
			{
				bPromote = true;
			}
			if (m_pUnit->canAcquirePromotion(kPromotion.ePromotion, true, bEquip, bAfflict, bPromote))
			//TB Combat Mods end
			{
				if (!bHasPromotion)
				{
					m_pUnit->setHasPromotion(kPromotion.ePromotion, true);
				}
			}
			else
			{
				if (bHasPromotion)
				{
					m_pUnit->setHasPromotion(kPromotion.ePromotion, false);
				}
			}
		}
	}
}

bool CvGameObject::isTag(TagTypes eTag) const
{
	return false;
}

bool CvGameObjectPlayer::isTag(TagTypes eTag) const
{
	switch (eTag)
	{
		case TAG_ANARCHY:
			return m_pPlayer->isAnarchy();
			break;
	}
	return false;
}

bool CvGameObjectCity::isTag(TagTypes eTag) const
{
	switch (eTag)
	{
		case TAG_ANARCHY:
			return m_pCity->isDisorder();
			break;
	}
	return false;
}

bool CvGameObjectUnit::isTag(TagTypes eTag) const
{
	switch (eTag)
	{
		case TAG_ONLY_DEFENSIVE:
			return m_pUnit->isOnlyDefensive();
			break;

		case TAG_SPY:
			return m_pUnit->isSpy();
			break;

		case TAG_FIRST_STRIKE_IMMUNE:
			return m_pUnit->immuneToFirstStrikes();
			break;

		case TAG_NO_DEFENSIVE_BONUS:
			return m_pUnit->noDefensiveBonus();
			break;

		case TAG_CAN_MOVE_IMPASSABLE:
			return m_pUnit->canMoveImpassable();
			break;

		case TAG_HIDDEN_NATIONALITY:
			return m_pUnit->isHiddenNationality();
			break;

		case TAG_BLITZ:
			return m_pUnit->isBlitz();
			break;

		case TAG_ALWAYS_HEAL:
			return m_pUnit->isAlwaysHeal();
			break;

		case TAG_ENEMY_ROUTE:
			return m_pUnit->isEnemyRoute();
			break;

		case TAG_FRESH_WATER:
		case TAG_WATER:
		case TAG_PEAK:
		case TAG_HILL:
		case TAG_FLATLAND:
		case TAG_OWNED:
		case TAG_CITY:
		case TAG_ANARCHY:
		case TAG_COASTAL:
			return m_pUnit->plot()->getGameObject()->isTag(eTag);
			break;
	}
	return false;
}

bool CvGameObjectPlot::isTag(TagTypes eTag) const
{
	CvCity* pCity = m_pPlot->getPlotCity();
	switch (eTag)
	{
		case TAG_FRESH_WATER:
			return m_pPlot->isFreshWater();
			break;

		case TAG_WATER:
			return m_pPlot->isRiver() || m_pPlot->isWater();
			break;

		case TAG_PEAK:
			return m_pPlot->isPeak();
			break;

		case TAG_HILL:
			return m_pPlot->isHills();
			break;

		case TAG_FLATLAND:
			return m_pPlot->isFlatlands();
			break;

		case TAG_OWNED:
			return m_pPlot->isOwned();
			break;

		case TAG_CITY:
			return m_pPlot->isCity();
			break;

		case TAG_ANARCHY:
			if (pCity)
			{
				return pCity->isDisorder();
			}
			break;

		case TAG_COASTAL:
			return m_pPlot->isCoastalLand();
			break;
	}
	return false;
}

int CvGameObject::getAttribute(AttributeTypes eAttribute) const
{
	return 0;
}

int CvGameObjectCity::getAttribute(AttributeTypes eAttribute) const
{
	switch (eAttribute)
	{
		case ATTRIBUTE_POPULATION:
			return m_pCity->getPopulation();
			break;

		case ATTRIBUTE_HEALTH:
			return m_pCity->healthRate();
			break;

		case ATTRIBUTE_HAPPINESS:
			return m_pCity->happyLevel();
			break;
	}
	return 0;
}

int CvGameObjectGame::getAttribute(AttributeTypes eAttribute) const
{
	int iCount = 0; 

	switch (eAttribute)
	{
	case ATTRIBUTE_PLAYERS:

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			if (kLoopPlayer.isAlive())
			{
				++iCount;
			}
		}
		return iCount;
		break;

	case ATTRIBUTE_TEAMS:
		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				++iCount;
			}
		}
		return iCount;
		break;
	}

	return 0;
}

void aggregateHasGOM(CvGameObject* pObject, GOMTypes eType, int iID, bool* bHasGOM)
{
	*bHasGOM = *bHasGOM || pObject->hasGOM(eType, iID);
}

bool CvGameObjectGame::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// If there is any building of that type created, return true
			BuildingTypes eBuilding = (BuildingTypes) iID;
			return GC.getGameINLINE().getBuildingClassCreatedCount((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()) > 0;
			//break;
		}

		case GOM_PROMOTION:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;
	
		case GOM_TRAIT:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_FEATURE:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_OPTION:
		{
			GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGameINLINE().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_GAMESPEED:
		{
			GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGameINLINE().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_BONUS:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_UNITTYPE:
		{
			// If there is any unit of that type created, return true
			UnitTypes eUnit = (UnitTypes) iID;
			return GC.getGameINLINE().getUnitClassCreatedCount((UnitClassTypes)GC.getUnitInfo(eUnit).getUnitClassType()) > 0;
			//break;
		}

		case GOM_TECH:
		{
			// If any team has researched that tech, return true
			TechTypes eTech = (TechTypes) iID;
			return GC.getGameINLINE().countKnownTechNumTeams(eTech) > 0;
			//break;
		}

		case GOM_CIVIC:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_RELIGION:
		{
			// True if the religion has been founded
			return GC.getGameINLINE().isReligionFounded((ReligionTypes) iID);
			//break;
		}

		case GOM_CORPORATION:
		{
			// True if the corporation has been founded
			return GC.getGameINLINE().isCorporationFounded((CorporationTypes) iID);
			//break;
		}

		case GOM_IMPROVEMENT:
			// This is not stored in a readily accessible way for the entire game, so return false
			return false;
			//break;

		case GOM_UNITCOMBAT:
			// This only makes sense on units and maybe sometimes on plots and cities
			return false;
			//break;

		case GOM_HANDICAP:
		{
			HandicapTypes eHandicap = (HandicapTypes) iID;
			return GC.getGameINLINE().getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectTeam::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// If there is any building of that type in the team, return true
			BuildingTypes eBuilding = (BuildingTypes) iID;
			return m_pTeam->getBuildingClassCount((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()) > 0;
			//break;
		}

		case GOM_PROMOTION:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;
	
		case GOM_TRAIT:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_FEATURE:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;

		case GOM_OPTION:
		{
			GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGameINLINE().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;

		case GOM_GAMESPEED:
		{
			GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGameINLINE().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;

		case GOM_BONUS:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_UNITTYPE:
		{
			// If there is any unit of that type in the team, return true
			UnitTypes eUnit = (UnitTypes) iID;
			return m_pTeam->getUnitClassCount((UnitClassTypes)GC.getUnitInfo(eUnit).getUnitClassType()) > 0;
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			TechTypes eTech = (TechTypes) iID;
			return m_pTeam->isHasTech(eTech);
			//break;
		}

		case GOM_CIVIC:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_RELIGION:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_CORPORATION:
		{
			// Defer to players, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_PLAYER, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_IMPROVEMENT:
			// This is not stored in a readily accessible way for the entire team, so return false
			return false;
			//break;

		case GOM_UNITCOMBAT:
			// This only makes sense on units and maybe sometimes on plots and cities
			return false;
			//break;

		case GOM_HANDICAP:
		{
			HandicapTypes eHandicap = (HandicapTypes) iID;
			return m_pTeam->getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectPlayer::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// If there is any building of that type of the player, return true
			BuildingTypes eBuilding = (BuildingTypes) iID;
			return m_pPlayer->getBuildingClassCount((BuildingClassTypes)GC.getBuildingInfo(eBuilding).getBuildingClassType()) > 0;
			//break;
		}

		case GOM_PROMOTION:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;
	
		case GOM_TRAIT:
		{
			// Return true if this player has the trait
			TraitTypes eTrait = (TraitTypes) iID;
			return m_pPlayer->hasTrait(eTrait);
			//break;
		}

		case GOM_FEATURE:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;

		case GOM_OPTION:
		{
			GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGameINLINE().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;

		case GOM_GAMESPEED:
		{
			GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGameINLINE().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;

		case GOM_BONUS:
		{
			// Use getNumAvailableBonuses which takes the bonuses in the capitals plot group, alternative would be hasBonus which loops through all cities
			BonusTypes eBonus = (BonusTypes) iID;
			return m_pPlayer->getNumAvailableBonuses(eBonus) > 0;
			//break;
		}

		case GOM_UNITTYPE:
		{
			// If there is any unit of that type of the player, return true
			UnitTypes eUnit = (UnitTypes) iID;
			return m_pPlayer->getUnitClassCount((UnitClassTypes)GC.getUnitInfo(eUnit).getUnitClassType()) > 0;
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			TechTypes eTech = (TechTypes) iID;
			return GET_TEAM(m_pPlayer->getTeam()).isHasTech(eTech);
			//break;
		}

		case GOM_CIVIC:
		{
			// Return true if this player has the civic active
			CivicTypes eCivic = (CivicTypes) iID;
			return m_pPlayer->isCivic(eCivic);
			//break;
		}

		case GOM_RELIGION:
		{
			// True if the religion is state religion
			return m_pPlayer->getStateReligion() == (ReligionTypes)iID;
			//break;
		}

		case GOM_CORPORATION:
		{
			// True if the corporation is active
			return m_pPlayer->isActiveCorporation((CorporationTypes)iID);
			//break;
		}

		case GOM_IMPROVEMENT:
			// This is not stored in a readily accessible way for the player, so return false
			return false;
			//break;

		case GOM_UNITCOMBAT:
			// This only makes sense on units and maybe sometimes on plots and cities
			return false;
			//break;

		case GOM_HANDICAP:
		{
			HandicapTypes eHandicap = (HandicapTypes) iID;
			return m_pPlayer->getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectCity::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// return true if the building is present and active
			BuildingTypes eBuilding = (BuildingTypes) iID;
			return m_pCity->getNumActiveBuilding(eBuilding) > 0;
			//break;
		}

		case GOM_PROMOTION:
		{
			// return true if that is a free promotion from the city
			PromotionTypes ePromotion = (PromotionTypes) iID;
			return m_pCity->isFreePromotion(ePromotion);
			//break;
		}
	
		case GOM_TRAIT:
		{
			// Return true if the owner has the trait
			TraitTypes eTrait = (TraitTypes) iID;
			return GET_PLAYER(m_pCity->getOwnerINLINE()).hasTrait(eTrait);
			//break;
		}

		case GOM_FEATURE:
		{
			// Defer to city plots, combine with OR
			bool bHasGOM = false;
			foreachRelated(GAMEOBJECT_PLOT, RELATION_WORKING, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_OPTION:
		{
			GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGameINLINE().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
		{
			// Defer to city plots, combine with OR
			bool bHasGOM = false;
			foreachRelated(GAMEOBJECT_PLOT, RELATION_WORKING, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_GAMESPEED:
		{
			GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGameINLINE().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
		{
			// Defer to city plots, combine with OR
			bool bHasGOM = false;
			foreachRelated(GAMEOBJECT_PLOT, RELATION_WORKING, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_BONUS:
		{
			// Vicinity not required, only connection
			BonusTypes eBonus = (BonusTypes) iID;
			return m_pCity->hasBonus(eBonus);
			//break;
		}

		case GOM_UNITTYPE:
		{
			// If there is any unit of that type in the city, return true, defer to the plot for that
			return m_pCity->plot()->getGameObject()->hasGOM(eType, iID);
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			TechTypes eTech = (TechTypes) iID;
			return GET_TEAM(m_pCity->getTeam()).isHasTech(eTech);
			//break;
		}

		case GOM_CIVIC:
		{
			// Return true if the owning player has the civic active
			CivicTypes eCivic = (CivicTypes) iID;
			return GET_PLAYER(m_pCity->getOwnerINLINE()).isCivic(eCivic);
			//break;
		}

		case GOM_RELIGION:
		{
			// True if the religion is in the city
			return m_pCity->isHasReligion((ReligionTypes)iID);
			//break;
		}

		case GOM_CORPORATION:
		{
			// True if the corporation is in the city
			return m_pCity->isHasCorporation((CorporationTypes)iID);
			//break;
		}

		case GOM_IMPROVEMENT:
		{
			// Defer to city plots, combine with OR
			bool bHasGOM = false;
			foreachRelated(GAMEOBJECT_PLOT, RELATION_WORKING, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_UNITCOMBAT:
		{
			// If there is any unit of that type in the city, return true, defer to the plot for that
			return m_pCity->plot()->getGameObject()->hasGOM(eType, iID);
			//break;
		}

		case GOM_HANDICAP:
		{
			HandicapTypes eHandicap = (HandicapTypes) iID;
			return m_pCity->getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectUnit::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// return true if the building is present in the city the unit is in and active
			BuildingTypes eBuilding = (BuildingTypes) iID;
			CvCity* pCity = m_pUnit->plot()->getPlotCity();
			if (pCity)
			{
				return pCity->getNumActiveBuilding(eBuilding) > 0;
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_PROMOTION:
		{
			// return true if the unit has that promotion
			PromotionTypes ePromotion = (PromotionTypes) iID;
			return m_pUnit->isHasPromotion(ePromotion);
			//break;
		}
	
		case GOM_TRAIT:
		{
			// Return true if the owner has the trait
			TraitTypes eTrait = (TraitTypes) iID;
			return GET_PLAYER(m_pUnit->getOwnerINLINE()).hasTrait(eTrait);
			//break;
		}

		case GOM_FEATURE:
		{
			// Check plot on which the unit is
			FeatureTypes eFeature = (FeatureTypes) iID;
//#ifdef MULTI_FEATURE_MOD
//			return m_pUnit->plot()->getHasFeature(eFeature);
//#else
			return m_pUnit->plot()->getFeatureType() == eFeature;
//#endif
			//break;
		}

		case GOM_OPTION:
		{
			GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGameINLINE().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
		{
			// Check plot on which the unit is
			TerrainTypes eTerrain = (TerrainTypes) iID;
			return m_pUnit->plot()->getTerrainType() == eTerrain;
			//break;
		}

		case GOM_GAMESPEED:
		{
			GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGameINLINE().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
		{
			// Check plot on which the unit is
			RouteTypes eRoute = (RouteTypes) iID;
			return m_pUnit->plot()->getRouteType() == eRoute;
			//break;
		}

		case GOM_BONUS:
		{
			// Check plot on which the unit is
			BonusTypes eBonus = (BonusTypes) iID;
			return m_pUnit->plot()->getBonusType() == eBonus;
			//break;
		}

		case GOM_UNITTYPE:
		{
			// Check the type of the unit
			UnitTypes eType = (UnitTypes) iID;
			return m_pUnit->getUnitType() == eType;
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			TechTypes eTech = (TechTypes) iID;
			return GET_TEAM(m_pUnit->getTeam()).isHasTech(eTech);
			//break;
		}

		case GOM_CIVIC:
		{
			// Return true if the owning player has the civic active
			CivicTypes eCivic = (CivicTypes) iID;
			return GET_PLAYER(m_pUnit->getOwnerINLINE()).isCivic(eCivic);
			//break;
		}

		case GOM_RELIGION:
		{
			// True if the religion is state religion
			return GET_PLAYER(m_pUnit->getOwnerINLINE()).getStateReligion() == (ReligionTypes)iID;
			//break;
		}

		case GOM_CORPORATION:
		{
			// True if the corporation is active
			return GET_PLAYER(m_pUnit->getOwnerINLINE()).isActiveCorporation((CorporationTypes)iID);
			//break;
		}

		case GOM_IMPROVEMENT:
		{
			// Check plot on which the unit is
			ImprovementTypes eImprovement = (ImprovementTypes) iID;
			return m_pUnit->plot()->getImprovementType() == eImprovement;
			//break;
		}

		case GOM_UNITCOMBAT:
		{
			// Check the combat types of the unit
			UnitCombatTypes eType = (UnitCombatTypes) iID;
			return m_pUnit->isHasUnitCombat(eType);
			//break;
		}

		case GOM_HANDICAP:
		{
			HandicapTypes eHandicap = (HandicapTypes) iID;
			return m_pUnit->getHandicapType() == eHandicap;
			//break;
		}
	}
	return false;
}

bool CvGameObjectPlot::hasGOM(GOMTypes eType, int iID)
{
	switch (eType)
	{
		case GOM_BUILDING:
		{
			// return true if the building is present in the city on this plot and active
			BuildingTypes eBuilding = (BuildingTypes) iID;
			CvCity* pCity = m_pPlot->getPlotCity();
			if (pCity)
			{
				return pCity->getNumActiveBuilding(eBuilding) > 0;
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_PROMOTION:
		{
			// Defer to units on the plot, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_UNIT, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}
	
		case GOM_TRAIT:
		{
			// Return true if the owner has the trait
			TraitTypes eTrait = (TraitTypes) iID;
			PlayerTypes ePlayer = m_pPlot->getOwnerINLINE();
			if (ePlayer != NO_PLAYER)
			{
				return GET_PLAYER(ePlayer).hasTrait(eTrait);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_FEATURE:
		{
			// Check feature type
			FeatureTypes eFeature = (FeatureTypes) iID;
//#ifdef MULTI_FEATURE_MOD
//			return m_pPlot->getHasFeature(eFeature);
//#else
			return m_pPlot->getFeatureType() == eFeature;
//#endif
			//break;
		}

		case GOM_OPTION:
		{
			GameOptionTypes eOption = (GameOptionTypes) iID;
			return GC.getGameINLINE().isOption(eOption);
			//break;
		}

		case GOM_TERRAIN:
		{
			// Check terrain type
			TerrainTypes eTerrain = (TerrainTypes) iID;
			return m_pPlot->getTerrainType() == eTerrain;
			//break;
		}

		case GOM_GAMESPEED:
		{
			GameSpeedTypes eSpeed = (GameSpeedTypes) iID;
			return GC.getGameINLINE().getGameSpeedType() == eSpeed;
			//break;
		}

		case GOM_ROUTE:
		{
			// Check route type
			RouteTypes eRoute = (RouteTypes) iID;
			return m_pPlot->getRouteType() == eRoute;
			//break;
		}

		case GOM_BONUS:
		{
			// Check bonus type
			BonusTypes eBonus = (BonusTypes) iID;
			return m_pPlot->getBonusType() == eBonus;
			//break;
		}

		case GOM_UNITTYPE:
		{
			// Defer to units on the plot, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_UNIT, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_TECH:
		{
			// If the team has researched that tech, return true
			TechTypes eTech = (TechTypes) iID;
			TeamTypes eTeam = m_pPlot->getTeam();
			if (eTeam != NO_TEAM)
			{
				return GET_TEAM(eTeam).isHasTech(eTech);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_CIVIC:
		{
			// Return true if the owning player has the civic active
			CivicTypes eCivic = (CivicTypes) iID;
			PlayerTypes ePlayer = m_pPlot->getOwnerINLINE();
			if (ePlayer != NO_PLAYER)
			{
				return GET_PLAYER(ePlayer).isCivic(eCivic);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_RELIGION:
		{
			// return true if the religion is present in the city on this plot
			CvCity* pCity = m_pPlot->getPlotCity();
			if (pCity)
			{
				return pCity->isHasReligion((ReligionTypes)iID);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_CORPORATION:
		{
			// return true if the corporation is present in the city on this plot
			CvCity* pCity = m_pPlot->getPlotCity();
			if (pCity)
			{
				return pCity->isHasCorporation((CorporationTypes)iID);
			}
			else
			{
				return false;
			}
			//break;
		}

		case GOM_IMPROVEMENT:
		{
			// Check improvement type
			ImprovementTypes eImprovement = (ImprovementTypes) iID;
			return m_pPlot->getImprovementType() == eImprovement;
			//break;
		}

		case GOM_UNITCOMBAT:
		{
			// Defer to units on the plot, combine with OR
			bool bHasGOM = false;
			foreach(GAMEOBJECT_UNIT, boost::bind(aggregateHasGOM, _1, eType, iID, &bHasGOM));
			return bHasGOM;
			//break;
		}

		case GOM_HANDICAP:
		{
			// Return true if the owning player has the handicap
			HandicapTypes eHandicap = (HandicapTypes) iID;
			PlayerTypes ePlayer = m_pPlot->getOwnerINLINE();
			if (ePlayer != NO_PLAYER)
			{
				return GET_PLAYER(ePlayer).getHandicapType() == eHandicap;
			}
			else
			{
				return false;
			}
			//break;
		}
	}
	return false;
}

void* CvGameObjectGame::addPythonArgument(CyArgsList *argsList)
{
	CyGame* pGame = new CyGame();
	argsList->add(gDLL->getPythonIFace()->makePythonObject(pGame));
	return (void*)pGame;
}

void CvGameObjectGame::disposePythonArgument(void *pArgument)
{
	delete (CyGame*)pArgument;
}

void* CvGameObjectTeam::addPythonArgument(CyArgsList *argsList)
{
	CyTeam* pTeam = new CyTeam(m_pTeam);
	argsList->add(gDLL->getPythonIFace()->makePythonObject(pTeam));
	return (void*)pTeam;
}

void CvGameObjectTeam::disposePythonArgument(void *pArgument)
{
	delete (CyTeam*)pArgument;
}

void* CvGameObjectPlayer::addPythonArgument(CyArgsList *argsList)
{
	CyPlayer* pPlayer = new CyPlayer(m_pPlayer);
	argsList->add(gDLL->getPythonIFace()->makePythonObject(pPlayer));
	return (void*)pPlayer;
}

void CvGameObjectPlayer::disposePythonArgument(void *pArgument)
{
	delete (CyPlayer*)pArgument;
}

void* CvGameObjectCity::addPythonArgument(CyArgsList *argsList)
{
	CyCity* pCity = new CyCity(m_pCity);
	argsList->add(gDLL->getPythonIFace()->makePythonObject(pCity));
	return (void*)pCity;
}

void CvGameObjectCity::disposePythonArgument(void *pArgument)
{
	delete (CyCity*)pArgument;
}

void* CvGameObjectUnit::addPythonArgument(CyArgsList *argsList)
{
	CyUnit* pUnit = new CyUnit(m_pUnit);
	argsList->add(gDLL->getPythonIFace()->makePythonObject(pUnit));
	return (void*)pUnit;
}

void CvGameObjectUnit::disposePythonArgument(void *pArgument)
{
	delete (CyUnit*)pArgument;
}

void* CvGameObjectPlot::addPythonArgument(CyArgsList *argsList)
{
	CyPlot* pPlot = new CyPlot(m_pPlot);
	argsList->add(gDLL->getPythonIFace()->makePythonObject(pPlot));
	return (void*)pPlot;
}

void CvGameObjectPlot::disposePythonArgument(void *pArgument)
{
	delete (CyPlot*)pArgument;
}


int CvGameObject::adaptValueToGame(int iID, int iValue) const
{
	iValue = (iValue * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getPercent(iID)) / 100;
	return (iValue * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getPercent(iID)) / 100;
}

int CvGameObjectPlayer::adaptValueToGame(int iID, int iValue) const
{
	iValue = (iValue * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getPercent(iID)) / 100;
	iValue = (iValue * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getPercent(iID)) / 100;
	return (iValue * GC.getHandicapInfo(m_pPlayer->getHandicapType()).getPercent(iID)) / 100;
}

int CvGameObjectCity::adaptValueToGame(int iID, int iValue) const
{
	iValue = (iValue * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getPercent(iID)) / 100;
	iValue = (iValue * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getPercent(iID)) / 100;
	return (iValue * GC.getHandicapInfo(GET_PLAYER(m_pCity->getOwnerINLINE()).getHandicapType()).getPercent(iID)) / 100;
}

int CvGameObjectUnit::adaptValueToGame(int iID, int iValue) const
{
	iValue = (iValue * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getPercent(iID)) / 100;
	iValue = (iValue * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getPercent(iID)) / 100;
	return (iValue * GC.getHandicapInfo(GET_PLAYER(m_pUnit->getOwnerINLINE()).getHandicapType()).getPercent(iID)) / 100;
}