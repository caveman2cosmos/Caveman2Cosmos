#include "CvGameCoreDLL.h"
#include "CvBuildingInfo.h"
#include "CvBugOptions.h"
#include "CvCity.h"
#include "CvGame.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvInfos.h"
#include "CvInitCore.h"
#include "CvMap.h"
#include "CvMessageControl.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPopupInfo.h"
#include "CvPython.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"

void CvGame::updateColoredPlots()
{
	PROFILE_FUNC();

	gDLL->getEngineIFace()->clearColoredPlots(PLOT_LANDSCAPE_LAYER_BASE);
	gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_CITY_RADIUS);
	gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_RANGED);
	gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_BLOCKADING);
	gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_COMMAND_FIELD);

	const NiColorA cHighlightText(GC.getColorInfo(GC.getCOLOR_HIGHLIGHT_TEXT()).getColor());

	if (!gDLL->GetWorldBuilderMode() || gDLL->getInterfaceIFace()->isInAdvancedStart())
	{
		gDLL->getEngineIFace()->clearColoredPlots(PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
	}

	if (isDebugMode())
	{
		for (int i = 0; i < GC.getMap().numPlots(); i++)
		{
			const CvPlot* plotX = GC.getMap().plotByIndex(i);
			if (plotX)
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					// AI city sites circles
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).AI_isPlotCitySite(plotX))
					{
						gDLL->getEngineIFace()->addColoredPlot(
							plotX->getViewportX(), plotX->getViewportY(),
							GC.getColorInfo((ColorTypes)GC.getPlayerColorInfo(GET_PLAYER((PlayerTypes)iI).getPlayerColor()).getColorTypePrimary()).getColor(),
							PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_BASE
						);
					}
					// AI plot improvement replacement circles
					const CvCity* pWorkingCity = plotX->getWorkingCity();

					if (pWorkingCity && plotX->getImprovementType() != NO_IMPROVEMENT)
					{
						const BuildTypes eBestBuild = pWorkingCity->AI_getBestBuild(pWorkingCity->getCityPlotIndex(plotX));

						if (NO_BUILD != eBestBuild
						&& GC.getBuildInfo(eBestBuild).getImprovement() != NO_IMPROVEMENT
						&& GC.getBuildInfo(eBestBuild).getImprovement() != plotX->getImprovementType())
						{
							gDLL->getEngineIFace()->addColoredPlot(
								plotX->getViewportX(), plotX->getViewportY(),
								GC.getColorInfo(GC.getCOLOR_RED()).getColor(),
								PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_BASE
							);
						}
					}
				}
			}
		}
	}

	// City circles when in Advanced Start
	if (gDLL->getInterfaceIFace()->isInAdvancedStart())
	{
		for (int i = 0; i < GC.getMap().numPlots(); i++)
		{
			const CvPlot* plotX = GC.getMap().plotByIndex(i);

			if (plotX && GET_PLAYER(getActivePlayer()).getAdvancedStartCityCost(true, plotX) > 0)
			{
				bool bStartingPlot = false;
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getActiveTeam())
					&&  GET_PLAYER((PlayerTypes)iI).getStartingPlot() == plotX)
					{
						bStartingPlot = true;
						break;
					}
				}
				if (bStartingPlot)
				{
					gDLL->getEngineIFace()->addColoredPlot(
						plotX->getViewportX(), plotX->getViewportY(),
						GC.getColorInfo(GC.getCOLOR_WARNING_TEXT()).getColor(),
						PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
					);
				}
				else if (GET_PLAYER(getActivePlayer()).AI_isPlotCitySite(plotX))
				{
					gDLL->getEngineIFace()->addColoredPlot(
						plotX->getViewportX(), plotX->getViewportY(), cHighlightText,
						PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
					);
				}
				if (plotX->isRevealed(getActiveTeam(), false))
				{
					NiColorA color(GC.getColorInfo(GC.getCOLOR_WHITE()).getColor()); color.a = 0.4f;

					gDLL->getEngineIFace()->fillAreaBorderPlot(plotX->getViewportX(), plotX->getViewportY(), color, AREA_BORDER_LAYER_CITY_RADIUS);
				}
			}
		}
	}

	const CvCity* pHeadSelectedCity = gDLL->getInterfaceIFace()->getHeadSelectedCity();
	CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if (pHeadSelectedCity)
	{
		if (gDLL->getInterfaceIFace()->isCityScreenUp())
		{
			for (int i = 0; i < NUM_CITY_PLOTS; i++)
			{
				if (pHeadSelectedCity->isWorkingPlot(i))
				{
					const CvPlot* plotX = plotCity(pHeadSelectedCity->getX(), pHeadSelectedCity->getY(), i);

					if (plotX)
					{
						NiColorA color(GC.getColorInfo(GC.getCOLOR_WHITE()).getColor()); color.a = 0.7f;

						gDLL->getEngineIFace()->addColoredPlot(plotX->getViewportX(), plotX->getViewportY(), color, PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_BASE);
					}
				}
			}
		}
		else
		{
			if (getBugOptionBOOL("CityBar__CityPlotStatus", true, "BUG_CITY_PLOT_STATUS"))
			{
				const bool bShowWorkingImprovedTile = getBugOptionBOOL("CityBar__WorkingImprovedPlot", false, "BUG_CITY_WORKING_IMPROVED_PLOT");
				const bool bShowWorkingImprovableTile = getBugOptionBOOL("CityBar__WorkingImprovablePlot", true, "BUG_CITY_WORKING_IMPROVABLE_PLOT");
				const bool bShowWorkingImprovableBonusTile = getBugOptionBOOL("CityBar__WorkingImprovableBonusPlot", true, "BUG_CITY_WORKING_IMPROVABLE_BONUS_PLOT");
				const bool bShowWorkingUnimprovableTile = getBugOptionBOOL("CityBar__WorkingUnimprovablePlot", true, "BUG_CITY_WORKING_UNIMPROVABLE_PLOT");

				const bool bShowNotWorkingImprovedTile = getBugOptionBOOL("CityBar__NotWorkingImprovedPlot", true, "BUG_CITY_NOT_WORKING_IMPROVED_PLOT");
				const bool bShowNotWorkingImprovableTile = getBugOptionBOOL("CityBar__NotWorkingImprovablePlot", true, "BUG_CITY_NOT_WORKING_IMPROVABLE_PLOT");
				const bool bShowNotWorkingImprovableBonusTile = getBugOptionBOOL("CityBar__NotWorkingImprovableBonusPlot", true, "BUG_CITY_NOT_WORKING_IMPROVABLE_BONUS_PLOT");
				const bool bShowNotWorkingUnimprovableTile = getBugOptionBOOL("CityBar__NotWorkingUnimprovablePlot", false, "BUG_CITY_NOT_WORKING_UNIMPROVABLE_PLOT");

				NiColorA workingImprovedTile(GC.getColorInfo(GC.getCOLOR_WHITE()).getColor());
				NiColorA workingImprovableTile(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_DARK_ORANGE")).getColor());
				NiColorA workingImprovableBonusTile(GC.getColorInfo(GC.getCOLOR_RED()).getColor());
				NiColorA workingUnimprovableTile(GC.getColorInfo(GC.getCOLOR_YELLOW()).getColor());

				NiColorA notWorkingImprovedTile(GC.getColorInfo(GC.getCOLOR_GREEN()).getColor());
				NiColorA notWorkingImprovableTile(GC.getColorInfo((ColorTypes)GC.getInfoTypeForString("COLOR_PLAYER_DARK_CYAN")).getColor());
				NiColorA notWorkingImprovableBonusTile(GC.getColorInfo(GC.getCOLOR_CYAN()).getColor());
				NiColorA notWorkingUnimprovableTile(GC.getColorInfo(GC.getCOLOR_BLACK()).getColor());

				CLLNode<IDInfo>* pSelectedCityNode = gDLL->getInterfaceIFace()->headSelectedCitiesNode();

				while (pSelectedCityNode)
				{
					const CvCity* pSelectedCity = ::getCity(pSelectedCityNode->m_data);
					pSelectedCityNode = gDLL->getInterfaceIFace()->nextSelectedCitiesNode(pSelectedCityNode);

					TeamTypes eOwnerTeam = pSelectedCity->getTeam();

					for (int iI = pSelectedCity->getNumCityPlots() - 1; iI >= SKIP_CITY_HOME_PLOT; iI--)
					{
						const CvPlot* plotX = pSelectedCity->getCityIndexPlot(iI);

						if (!plotX || plotX->getWorkingCity() != pSelectedCity || !pSelectedCity->canWork(plotX))
						{
							continue;
						}
						const FeatureTypes eFeature = plotX->getFeatureType();

						const bool bUnimprovable = eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature).isNoImprovement();

						bool bFullyImproved = false;
						bool bCanBeImproved = false;
						bool bCanProvideBonus = false;

						if (!bUnimprovable)
						{
							const RouteTypes eRoute = plotX->getRouteType();
							const BonusTypes eBonus = plotX->getBonusType(eOwnerTeam);
							const ImprovementTypes eImprovement = plotX->getImprovementType();

							const bool bHasImprovement = eImprovement != NO_IMPROVEMENT && eImprovement != GC.getIMPROVEMENT_CITY_RUINS();

							BuildTypes eBestBuild = pSelectedCity->AI_getBestBuild(iI);
							RouteTypes eBestRoute = NO_ROUTE;
							ImprovementTypes eBestImprovement = NO_IMPROVEMENT;
							bool bBestBuildRemovesFeature = false;

							if (eBestBuild != NO_BUILD)
							{

								const CvBuildInfo& kBestBuild = GC.getBuildInfo(eBestBuild);
								eBestImprovement = kBestBuild.getImprovement();

								eBestRoute = (RouteTypes)kBestBuild.getRoute();

								if (eFeature != NO_FEATURE && kBestBuild.isFeatureRemove(eFeature))
								{
									bBestBuildRemovesFeature = true;
								}

								// will the best build provide a bonus?
								if (eBonus != NO_BONUS)
								{
									if (eBestImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eBestImprovement).isImprovementBonusTrade(eBonus)
									&& (!bHasImprovement || !GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eBonus)))
									{
										bCanBeImproved = true;
										bCanProvideBonus = true;
									}
									else if (bHasImprovement && eBestRoute != NO_ROUTE && eRoute == NO_ROUTE && !plotX->isWater()
									&& !plotX->isConnectedTo(pSelectedCity)
									&& GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eBonus))
									{
										bCanProvideBonus = true;
									}
								}
							}

							if (!bCanProvideBonus)
							{
								if (bHasImprovement)
								{
									if (eBestBuild != NO_BUILD)
									{
										const CvImprovementInfo& kPlotImprovement = GC.getImprovementInfo(eImprovement);

										if (eBestRoute != NO_ROUTE)
										{
											// will the route increase yields?
											if (eRoute == NO_ROUTE)
											{
												for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
												{
													if (kPlotImprovement.getRouteYieldChanges(eBestRoute, iJ) > 0)
													{
														bCanBeImproved = true;
														break;
													}
												}
											}
											else
											{
												for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
												{
													if (kPlotImprovement.getRouteYieldChanges(eBestRoute, iJ) > kPlotImprovement.getRouteYieldChanges(eRoute, iJ))
													{
														bCanBeImproved = true;
														break;
													}
												}
											}
										}
										else if (eBestImprovement == NO_IMPROVEMENT)
										{
											// does the best build clear a bad feature?
											if (bBestBuildRemovesFeature && !kPlotImprovement.isRequiresFeature() && GC.getFeatureInfo(eFeature).isOnlyBad())
											{
												bCanBeImproved = true;
											}
										}
									}
									bFullyImproved = !bCanBeImproved;
								}
								// no improvement
								else if (eBestBuild != NO_BUILD)
								{
									if (bBestBuildRemovesFeature)
									{
										if (eBonus != NO_BONUS && eBestImprovement != NO_IMPROVEMENT
										&& GC.getImprovementInfo(eBestImprovement).isImprovementBonusTrade(eBonus))
										{
											// does the best build provide a bonus
											bCanBeImproved = true;
											bCanProvideBonus = true;
										}
										else if (GC.getFeatureInfo(eFeature).isOnlyBad())
										{
											// does the best build clear a bad feature?
											bCanBeImproved = true;
										}
										else if (eBestImprovement != NO_IMPROVEMENT)
										{
											//Fuyu: count chops too if an improvement is placed instead
											bCanBeImproved = true;
										}
									}
									else if (eBestRoute == NO_ROUTE)
									{
										// any other non-feature-clearing, non-route build is okay
										bCanBeImproved = true;
									}
								}
							}
						}

						if (pSelectedCity->isWorkingPlot(plotX))
						{
							if (bFullyImproved)
							{
								if (bShowWorkingImprovedTile)
								{
									gDLL->getEngineIFace()->addColoredPlot(
										plotX->getViewportX(), plotX->getViewportY(), workingImprovedTile,
										PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
									);
								}
							}
							else if (bCanProvideBonus)
							{
								if (bShowWorkingImprovableBonusTile)
								{
									gDLL->getEngineIFace()->addColoredPlot(
										plotX->getViewportX(), plotX->getViewportY(), workingImprovableBonusTile,
										PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
									);
								}
							}
							else if (bCanBeImproved)
							{
								if (bShowWorkingImprovableTile)
								{
									gDLL->getEngineIFace()->addColoredPlot(
										plotX->getViewportX(), plotX->getViewportY(), workingImprovableTile,
										PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
									);
								}
							}
							else if (bShowWorkingUnimprovableTile)
							{
								gDLL->getEngineIFace()->addColoredPlot(
									plotX->getViewportX(), plotX->getViewportY(), workingUnimprovableTile,
									PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
								);
							}
						}
						else
						{
							if (bFullyImproved)
							{
								if (bShowNotWorkingImprovedTile)
								{
									gDLL->getEngineIFace()->addColoredPlot(
										plotX->getViewportX(), plotX->getViewportY(), notWorkingImprovedTile,
										PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
									);
								}
							}
							else if (bCanProvideBonus)
							{
								if (bShowNotWorkingImprovableBonusTile)
								{
									gDLL->getEngineIFace()->addColoredPlot(
										plotX->getViewportX(), plotX->getViewportY(), notWorkingImprovableBonusTile,
										PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
									);
								}
							}
							else if (bCanBeImproved)
							{
								if (bShowNotWorkingImprovableTile)
								{
									gDLL->getEngineIFace()->addColoredPlot(
										plotX->getViewportX(), plotX->getViewportY(), notWorkingImprovableTile,
										PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
									);
								}
							}
							else if (bShowNotWorkingUnimprovableTile)
							{
								gDLL->getEngineIFace()->addColoredPlot(
									plotX->getViewportX(), plotX->getViewportY(), notWorkingUnimprovableTile,
									PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
								);
							}
						}
					}
				}
			}

			{
				CLLNode<IDInfo>* pSelectedCityNode = gDLL->getInterfaceIFace()->headSelectedCitiesNode();

				while (pSelectedCityNode)
				{
					const CvCity* pSelectedCity = ::getCity(pSelectedCityNode->m_data);
					pSelectedCityNode = gDLL->getInterfaceIFace()->nextSelectedCitiesNode(pSelectedCityNode);

				}
			}
			CLLNode<IDInfo>* pSelectedCityNode = gDLL->getInterfaceIFace()->headSelectedCitiesNode();

			while (pSelectedCityNode)
			{
				const CvCity* pSelectedCity = ::getCity(pSelectedCityNode->m_data);
				pSelectedCityNode = gDLL->getInterfaceIFace()->nextSelectedCitiesNode(pSelectedCityNode);

				for (int i = pSelectedCity->getNumCityPlots() - 1; i > -1; i--)
				{
					const CvPlot* plotX = pSelectedCity->getCityIndexPlot(i);

					if (plotX && plotX->getWorkingCity() == pSelectedCity)
					{
						gDLL->getEngineIFace()->fillAreaBorderPlot(plotX->getX(), plotX->getY(), cHighlightText, AREA_BORDER_LAYER_CITY_RADIUS);
					}
				}
				const CvPlot* pRallyPlot = pSelectedCity->getRallyPlot();
				if (pRallyPlot)
				{
					gDLL->getEngineIFace()->addColoredPlot(
						pRallyPlot->getViewportX(), pRallyPlot->getViewportY(),
						GC.getColorInfo(GC.getCOLOR_BLUE()).getColor(),
						PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_BASE
					);
				}
			}
		}
	}
	else if (pHeadSelectedUnit)
	{
		const PlayerTypes eOwner = pHeadSelectedUnit->getOwner();
		const CvPlot* pPlot = pHeadSelectedUnit->plot();

		if (gDLL->getGraphicOption(GRAPHICOPTION_CITY_RADIUS) && gDLL->getInterfaceIFace()->canSelectionListFound())
		{
			for (int iI = GC.getMap().numPlots() - 1; iI > -1; iI--)
			{
				const CvPlot* plotX = GC.getMap().plotByIndex(iI);

				if (plotX->getOwner() == eOwner && plotX->getWorkingCity())
				{
					gDLL->getEngineIFace()->fillAreaBorderPlot(plotX->getX(), plotX->getY(), cHighlightText, AREA_BORDER_LAYER_CITY_RADIUS);
				}
			}
		}
		if (pHeadSelectedUnit->getGroup()->hasCommander() || pHeadSelectedUnit->getGroup()->canFight() && pPlot->inCommandField(eOwner))
		{
			const std::vector<CvPlot*> plots = GET_PLAYER(eOwner).getCommandFieldPlots();

			NiColorA cField(GC.getColorInfo(GC.getCOLOR_RED()).getColor());
			cField.a = 0.75f;

			// Toffer - Don't replace with foreach_ macro, this is more performance friendly and is as readable if not more so.
			for (int i = plots.size() - 1; i > -1; i--)
			{
				gDLL->getEngineIFace()->fillAreaBorderPlot(plots[i]->getX(), plots[i]->getY(), cField, AREA_BORDER_LAYER_COMMAND_FIELD);
			}
		}

		// Dale - RB: Field Bombard START
		if (GC.isDCM_RANGE_BOMBARD())
		{
			int iMaxAirRange = 0;

			foreach_(const CvUnit* pSelectedUnit, gDLL->getInterfaceIFace()->getSelectionList()->units())
			{
				FAssert(pSelectedUnit);

				iMaxAirRange = std::max(iMaxAirRange, pSelectedUnit->getDCMBombRange());
			}

			if (iMaxAirRange > 0)
			{
				const CvSelectionGroup* pGroup = pHeadSelectedUnit->getGroup();

				foreach_(const CvPlot* plotX, pPlot->rect(iMaxAirRange, iMaxAirRange))
				{
					if (plotX->isVisible(pHeadSelectedUnit->getTeam(), false) && plotDistance(pPlot->getX(), pPlot->getY(), plotX->getX(), plotX->getY()) <= iMaxAirRange)
					{
						NiColorA color(GC.getColorInfo(GC.getCOLOR_WHITE()).getColor());

						if (pGroup->canBombardAtRanged(pPlot, plotX->getX(), plotX->getY()))
						{
							if (plotX->getNumVisibleEnemyCombatUnits(eOwner))
							{
								color.r = 0.0f;
								color.b = 0.0f;
							}
							else color.b = 0.0f;
						}
						else color.a = 0.33f;

						gDLL->getEngineIFace()->addColoredPlot(plotX->getViewportX(), plotX->getViewportY(), color, PLOT_STYLE_TARGET, PLOT_LANDSCAPE_LAYER_BASE);
					}
				}
			}
		}
		// Dale - RB: Field Bombard END

		if (pHeadSelectedUnit->getDomainType() == DOMAIN_AIR)
		{
			int iMaxAirRange = 0;

			foreach_(const CvUnit* pSelectedUnit, gDLL->getInterfaceIFace()->getSelectionList()->units())
			{
				FAssert(pSelectedUnit);

				iMaxAirRange = std::max(iMaxAirRange, pSelectedUnit->airRange());
			}

			if (iMaxAirRange > 0)
			{
				foreach_(const CvPlot* plotX, pPlot->rect(iMaxAirRange, iMaxAirRange))
				{
					if (plotDistance(pHeadSelectedUnit->getX(), pHeadSelectedUnit->getY(), plotX->getX(), plotX->getY()) <= iMaxAirRange)
					{
						NiColorA color(GC.getColorInfo(GC.getCOLOR_YELLOW()).getColor()); color.a = 0.5f;

						gDLL->getEngineIFace()->fillAreaBorderPlot(plotX->getX(), plotX->getY(), color, AREA_BORDER_LAYER_RANGED);
					}
				}
			}
		}
		else if(pHeadSelectedUnit->airRange() > 0) //other ranged units
		{
			const int iRange = pHeadSelectedUnit->airRange();
			foreach_(CvPlot* pTargetPlot, pPlot->rect(iRange, iRange))
			{
				if (pTargetPlot->isVisible(pHeadSelectedUnit->getTeam(), false)
				&& plotDistance(pHeadSelectedUnit->getX(), pHeadSelectedUnit->getY(), pTargetPlot->getX(), pTargetPlot->getY()) <= iRange
				&& pPlot->canSeePlot(pTargetPlot, pHeadSelectedUnit->getTeam()))
				{
					NiColorA color(GC.getColorInfo(GC.getCOLOR_YELLOW()).getColor()); color.a = 0.5f;

					gDLL->getEngineIFace()->fillAreaBorderPlot(pTargetPlot->getX(), pTargetPlot->getY(), color, AREA_BORDER_LAYER_RANGED);
				}
			}
		}

		if (!GET_PLAYER(getActivePlayer()).isOption(PLAYEROPTION_NO_UNIT_RECOMMENDATIONS))
		{
			if (pPlot->getOwner() == eOwner
			&& (pHeadSelectedUnit->AI_getUnitAIType() == UNITAI_WORKER || pHeadSelectedUnit->AI_getUnitAIType() == UNITAI_WORKER_SEA))
			{
				const CvCity* pCity = pPlot->getWorkingCity();
				CvPlot* pBestPlot;

				if (pCity && pHeadSelectedUnit->AI_bestCityBuild(pCity, &pBestPlot))
				{
					FAssert(pBestPlot);

					gDLL->getEngineIFace()->addColoredPlot(
						pBestPlot->getViewportX(), pBestPlot->getViewportY(), cHighlightText,
						PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
					);
					CvPlot* pNextBestPlot;

					if (pHeadSelectedUnit->AI_bestCityBuild(pCity, &pNextBestPlot, NULL, pBestPlot))
					{
						FAssert(pNextBestPlot);

						gDLL->getEngineIFace()->addColoredPlot(
							pNextBestPlot->getViewportX(), pNextBestPlot->getViewportY(), cHighlightText,
							PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
						);
					}
				}
			}

			const int iRange = 5;

			foreach_(const CvPlot* plotX, pPlot->rect(iRange, iRange))
			{
				if (plotX->area() == pHeadSelectedUnit->area() || plotX->isAdjacentToArea(pHeadSelectedUnit->area()))
				{
					if (pHeadSelectedUnit->canFound(plotX) && GET_PLAYER(eOwner).AI_isPlotCitySite(plotX))
					{
						gDLL->getEngineIFace()->addColoredPlot(
							plotX->getX(), plotX->getY(), cHighlightText,
							PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
						);
					}
					else if (plotDistance(pHeadSelectedUnit->getX(), pHeadSelectedUnit->getViewportX(), plotX->getViewportY(), plotX->getY()) <= iRange
					&& plotX->isVisible(pHeadSelectedUnit->getTeam(), false) && pHeadSelectedUnit->isNoBadGoodies()
					&& plotX->isRevealedGoody(pHeadSelectedUnit->getTeam()))
					{
						gDLL->getEngineIFace()->addColoredPlot(
							plotX->getViewportX(), plotX->getViewportY(), cHighlightText,
							PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS
						);
					}
				}
			}
		}

		if (pHeadSelectedUnit->isBlockading())
		{
			const int iBlockadeRange = GC.getDefineINT("SHIP_BLOCKADE_RANGE");

			for (int i = 0; i < MAX_PC_PLAYERS; ++i)
			{
				const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)i);

				if (kPlayer.getTeam() == getActiveTeam())
				{
					foreach_(const CvUnit* unitX, kPlayer.units() | filtered(CvUnit::fn::isBlockading()))
					{
						foreach_(const CvPlot* plotX, unitX->plot()->rect(iBlockadeRange, iBlockadeRange)
						| filtered(CvPlot::fn::isRevealed(getActiveTeam(), false)))
						{
							if (GC.getMap().calculatePathDistance(unitX->plot(),plotX) > iBlockadeRange)
							{
								continue; // No blockading on other side of an isthmus
							}

							if (plotX->isWater() && plotX->area() == unitX->area())
							{
								NiColorA color(GC.getColorInfo((ColorTypes)GC.getPlayerColorInfo(GET_PLAYER(getActivePlayer()).getPlayerColor()).getColorTypePrimary()).getColor());
								color.a = 0.5f;
								gDLL->getEngineIFace()->fillAreaBorderPlot(plotX->getX(), plotX->getY(), color, AREA_BORDER_LAYER_BLOCKADING);
							}
						}
					}
				}
			}
		}
	}
}


void CvGame::updateBlockadedPlots()
{
	PROFILE_FUNC();

	gDLL->getEngineIFace()->clearAreaBorderPlots(AREA_BORDER_LAYER_BLOCKADED);

	const int iNumPlots = GC.getMap().numPlots();
	for (int i = 0; i < iNumPlots; ++i)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(i);

		FAssert(pLoopPlot);

		if (pLoopPlot->getBlockadedCount(getActiveTeam()) > 0 && pLoopPlot->isRevealed(getActiveTeam(), false))
		{
			NiColorA color(GC.getColorInfo(GC.getCOLOR_BLACK()).getColor());
			color.a = 0.35f;
			gDLL->getEngineIFace()->fillAreaBorderPlot(pLoopPlot->getX(), pLoopPlot->getY(), color, AREA_BORDER_LAYER_BLOCKADED);
		}
	}
}


/*DllExport*/ void CvGame::updateSelectionList()
{
	OutputDebugString("Exe wants to update selection list\n");
}

void CvGame::updateSelectionListInternal(int iCycleDelay, bool bSetCamera, bool bAllowViewportSwitch, bool bForceAcceptCurrent)
{
	if (GET_PLAYER(getActivePlayer()).isOption(PLAYEROPTION_NO_UNIT_CYCLING) || GC.getCurrentViewport()->isSelectionInhibitted())
	{
		return;
	}
	{
		const int iMinCycleDelay = getBugOptionINT("MainInterface__CycleDelayMin", 0);
		if (iCycleDelay < iMinCycleDelay) iCycleDelay = iMinCycleDelay;
	}
	const CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if (!pHeadSelectedUnit || !bForceAcceptCurrent && !pHeadSelectedUnit->getGroup()->readyToSelect(true))
	{
		if (iCycleDelay > 0)
		{
			m_iCycleUnitSliceDelay = iCycleDelay;

			if (pHeadSelectedUnit)
			{
				gDLL->getInterfaceIFace()->clearSelectionList();
			}
		}
		else
		{
			const CvPlot* originalPlot = gDLL->getInterfaceIFace()->getOriginalPlot();

			if (!originalPlot || !nextPlotUnit(originalPlot, true, true, gDLL->getInterfaceIFace()->getOriginalPlotCount()))
			{
				const CvPlot* selectionPlot = gDLL->getInterfaceIFace()->getSelectionPlot();

				if (!selectionPlot || !nextPlotUnit(selectionPlot, true, true))
				{
					cycleSelectionGroups(true, true, false, bSetCamera, bAllowViewportSwitch);
				}
			}
			const CvUnit* newSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

			if (newSelectedUnit && !newSelectedUnit->getGroup()->readyToSelect())
			{
				gDLL->getInterfaceIFace()->clearSelectionList();
			}
		}
	}
}


void CvGame::updateTestEndTurn()
{
	PROFILE_FUNC();
	const CvPlayer& player = GET_PLAYER(getActivePlayer());
	FAssertMsg(player.isHumanPlayer(), "Why would exe call this for AI players?")

	if (!player.isTurnActive())
	{
		return;
	}

	if (gDLL->getInterfaceIFace()->isEndTurnMessage())
	{
		if (player.hasReadyUnit(gDLL->getInterfaceIFace()->getHeadSelectedUnit() && !player.isOption(PLAYEROPTION_NO_UNIT_CYCLING)))
		{
			gDLL->getInterfaceIFace()->setEndTurnMessage(false);
		}
	}
	else if (!player.hasBusyUnit()
	&& !player.hasReadyUnit(gDLL->getInterfaceIFace()->getHeadSelectedUnit()
	&& !player.isOption(PLAYEROPTION_NO_UNIT_CYCLING)))
	{
		if (!gDLL->getInterfaceIFace()->isForcePopup())
		{
			gDLL->getInterfaceIFace()->setForcePopup(true);
		}
		else if (!player.hasAutoUnit())
		{
			const bool bDecisionlessTurn = !player.getTurnHadUIInteraction();

			if (isHotSeat() || isPbem()
			|| bDecisionlessTurn && !getBugOptionBOOL("MainInterface__AutoEndDecisionlessTurns", false)
			|| !bDecisionlessTurn && player.isOption(PLAYEROPTION_WAIT_END_TURN))
			{
				gDLL->getInterfaceIFace()->setEndTurnMessage(true);
				return;
			}

			if (gDLL->getInterfaceIFace()->getEndTurnCounter() > 0)
			{
				gDLL->getInterfaceIFace()->changeEndTurnCounter(-1);
			}
			else
			{
				CvMessageControl::getInstance().sendTurnComplete();
				gDLL->getInterfaceIFace()->setEndTurnCounter(2 * getBugOptionINT("MainInterface__AutoEndTurnDelay", 2));
			}
		}
		else if (!gDLL->shiftKey())
		{
			CvMessageControl::getInstance().sendAutoMoves();
		}
	}
}

CvUnit* CvGame::getPlotUnit(const CvPlot* pPlot, int iIndex) const
{
	PROFILE_FUNC();

	if (pPlot)
	{
		const PlayerTypes activePlayer = getActivePlayer();
		const TeamTypes activeTeam = getActiveTeam();
		int iCount = 0;
		for (int iPass = 0; iPass < 2; iPass++)
		{
			foreach_(CvUnit* pLoopUnit1, pPlot->units())
			{
				if (!pLoopUnit1->isInvisible(activeTeam, true) && !pLoopUnit1->isCargo()
				&& (pLoopUnit1->getOwner() == activePlayer) == (iPass == 0))
				{
					if (iCount == iIndex)
					{
						return pLoopUnit1;
					}
					iCount++;

					if (pLoopUnit1->hasCargo())
					{
						foreach_(CvUnit* pLoopUnit2, pPlot->units())
						{
							if (!pLoopUnit2->isInvisible(activeTeam, true) && pLoopUnit2->getTransportUnit() == pLoopUnit1)
							{
								if (iCount == iIndex)
								{
									return pLoopUnit2;
								}
								iCount++;
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

void CvGame::getPlotUnits(const CvPlot* pPlot, std::vector<CvUnit*>& plotUnits) const
{
	PROFILE_FUNC();
	plotUnits.erase(plotUnits.begin(), plotUnits.end());

	if (pPlot)
	{
		const PlayerTypes activePlayer = getActivePlayer();
		const TeamTypes activeTeam = getActiveTeam();

		for (int iPass = 0; iPass < 2; iPass++)
		{
			foreach_(CvUnit* pLoopUnit1, pPlot->units())
			{
				if (!pLoopUnit1->isInvisible(activeTeam, true) && !pLoopUnit1->isCargo()
				&& (pLoopUnit1->getOwner() == activePlayer) == (iPass == 0))
				{
					plotUnits.push_back(pLoopUnit1);

					if (pLoopUnit1->hasCargo())
					{
						foreach_(CvUnit* pLoopUnit2, pPlot->units())
						{
							if (!pLoopUnit2->isInvisible(activeTeam, true) && pLoopUnit2->getTransportUnit() == pLoopUnit1)
							{
								plotUnits.push_back(pLoopUnit2);
							}
						}
					}
				}
			}
		}
	}
}

void CvGame::cycleCities(bool bForward, bool bAdd) const
{
	CvCity* pHeadSelectedCity = gDLL->getInterfaceIFace()->getHeadSelectedCity();
	CvCity* pSelectCity = NULL;
	int iLoop;
	if (pHeadSelectedCity && (pHeadSelectedCity->getTeam() == getActiveTeam() || isDebugMode()))
	{
		iLoop = pHeadSelectedCity->getIndex() + (bForward ? 1 : -1);

		CvCity* pLoopCity = GET_PLAYER(pHeadSelectedCity->getOwner()).nextCity(&iLoop, !bForward);

		if (!pLoopCity)
		{
			pLoopCity = GET_PLAYER(pHeadSelectedCity->getOwner()).firstCity(&iLoop, !bForward);
		}

		if (pLoopCity && pLoopCity != pHeadSelectedCity)
		{
			pSelectCity = pLoopCity;
		}
	}
	else pSelectCity = GET_PLAYER(getActivePlayer()).firstCity(&iLoop, !bForward);

	if (pSelectCity)
	{
		if (GC.getCurrentViewport()->isInViewport(pSelectCity->getX(), pSelectCity->getY(), GC.getVIEWPORT_FOCUS_BORDER()))
		{
			if (bAdd)
			{
				gDLL->getInterfaceIFace()->clearSelectedCities();
				gDLL->getInterfaceIFace()->addSelectedCity(pSelectCity);
			}
			else gDLL->getInterfaceIFace()->selectCity(pSelectCity);

			gDLL->getInterfaceIFace()->lookAtSelectionPlot();
		}
		else GC.getCurrentViewport()->bringIntoView(pSelectCity->getX(), pSelectCity->getY(), NULL, true, gDLL->getInterfaceIFace()->isCityScreenUp(), true, bAdd);
	}
}


void CvGame::cycleSelectionGroups(bool bClear, bool bForward, bool bWorkers, bool bSetCamera, bool bAllowViewportSwitch) const
{
	if (GET_PLAYER(getActivePlayer()).hasIdleCity())
	{
		return;
	}
	CvSelectionGroup* pNextSelectionGroup;
	CvUnit* pCycleUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if (pCycleUnit)
	{
		if (pCycleUnit->getOwner() != getActivePlayer())
		{
			pCycleUnit = NULL;
		}
		bool bWrap;
		pNextSelectionGroup = GET_PLAYER(getActivePlayer()).cycleSelectionGroups(pCycleUnit, bForward, bWorkers, &bWrap, bAllowViewportSwitch);

		if (bWrap && GET_PLAYER(getActivePlayer()).hasAutoUnit())
		{
			CvMessageControl::getInstance().sendAutoMoves();
		}
	}
	else
	{
		const CvPlot* pPlot = gDLL->getInterfaceIFace()->getLookAtPlot();
		pNextSelectionGroup = GC.getMap().findSelectionGroupInternal(pPlot ? pPlot->getX() : 0, pPlot ? pPlot->getY() : 0, getActivePlayer(), true, bWorkers, bAllowViewportSwitch);
	}

	if (pNextSelectionGroup)
	{
		if (!GC.getCurrentViewport()->isInViewport(pNextSelectionGroup->getX(), pNextSelectionGroup->getY(), GC.getVIEWPORT_FOCUS_BORDER()))
		{
			// Switch viewport
			GC.getCurrentViewport()->bringIntoView(pNextSelectionGroup->getX(), pNextSelectionGroup->getY(), pNextSelectionGroup->getHeadUnit());
			return;
		}
		// If we switched viewport then the asynchronous viewport switching will also handle the unit selection
		FAssert(pNextSelectionGroup->getOwner() == getActivePlayer());
		gDLL->getInterfaceIFace()->selectUnit(pNextSelectionGroup->getHeadUnit(), bClear);
	}
	// If we switched viewport then the asynchronous viewport switching will also handle the lookat
	if (bSetCamera
		&&
		(
			pCycleUnit != gDLL->getInterfaceIFace()->getHeadSelectedUnit()
			||
			pCycleUnit && pCycleUnit->getGroup()->readyToSelect()
		)
	) gDLL->getInterfaceIFace()->lookAtSelectionPlot();
}


// Returns true if unit was cycled...
bool CvGame::nextPlotUnit(const CvPlot* pPlot, bool bForward, bool bAuto, int iCount) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit = NULL;

	FAssertMsg(iCount >= -1, "iCount expected to be >= -1");

	if (iCount == -1)
	{
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit->IsSelected())
			{
				break;
			}
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}
	}
	else
	{
		pUnitNode = pPlot->headUnitNode();

		while (pUnitNode)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (iCount == 1)
			{
				break;
			}
			if (iCount > 0)
			{
				iCount--;
			}
			pUnitNode = pPlot->nextUnitNode(pUnitNode);
		}

		if (!pUnitNode)
		{
			pUnitNode = pPlot->tailUnitNode();

			if (pUnitNode)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
			}
		}
	}

	if (pUnitNode)
	{
		CvUnit* pSelectedUnit = pLoopUnit;

		while (true)
		{
			if (bForward)
			{
				pUnitNode = pPlot->nextUnitNode(pUnitNode);
				if (!pUnitNode)
				{
					pUnitNode = pPlot->headUnitNode();
				}
			}
			else
			{
				pUnitNode = pPlot->prevUnitNode(pUnitNode);
				if (!pUnitNode)
				{
					pUnitNode = pPlot->tailUnitNode();
				}
			}

			pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (iCount == -1 && pLoopUnit == pSelectedUnit)
			{
				break;
			}

			if (pLoopUnit->getOwner() == getActivePlayer())
			{
				if (!bAuto)
				{
					gDLL->getInterfaceIFace()->insertIntoSelectionList(pLoopUnit, true, false);
					return true;
				}
				if (pLoopUnit->getGroup()->readyToSelect())
				{
					gDLL->getInterfaceIFace()->selectUnit(pLoopUnit, true);
					return true;
				}
			}

			if (pLoopUnit == pSelectedUnit)
			{
				break;
			}
		}
	}
	return false;
}

/*DllExport*/ bool CvGame::cyclePlotUnits(const CvPlot* pPlot, bool bForward, bool bAuto, int iCount) const
{
	OutputDebugString("exe is cycling units on plot ctrl+MouseWheelRoll\n");
	return nextPlotUnit(pPlot, bForward, bAuto, iCount);
}


bool CvGame::selectCity(CvCity* pSelectCity, bool bCtrl, bool bAlt, bool bShift) const
{
	if (!pSelectCity || !pSelectCity->canBeSelected())
	{
		return false;
	}

	if (!bShift)
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();
	}

	if (bAlt)
	{
		foreach_(CvCity* pLoopCity, GET_PLAYER(pSelectCity->getOwner()).cities())
		{
			gDLL->getInterfaceIFace()->addSelectedCity(pLoopCity);
		}
	}
	else if (bCtrl)
	{
		foreach_(CvCity* pLoopCity, GET_PLAYER(pSelectCity->getOwner()).cities())
		{
			if (pLoopCity->getArea() == pSelectCity->getArea())
			{
				gDLL->getInterfaceIFace()->addSelectedCity(pLoopCity);
			}
		}
	}
	else gDLL->getInterfaceIFace()->addSelectedCity(pSelectCity, bShift);

	return true;
}

void CvGame::selectionListMove(CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const
{
	if (!pPlot)
	{
		return;
	}
	CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if (!pHeadSelectedUnit || pHeadSelectedUnit->getOwner() != getActivePlayer())
	{
		return;
	}

	if (bAlt)
	{
		gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, false, true);
	}
	else if (bCtrl)
	{
		gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, true, false);
	}
	const bool bAskToDeclareWar = getBugOptionBOOL("Actions__AskDeclareWarUnits", true, "BUG_ASK_DECLARE_WAR_UNITS");

	foreach_(const CvUnit* pSelectedUnit, gDLL->getInterfaceIFace()->getSelectionList()->units())
	{
		const TeamTypes eRivalTeam = pSelectedUnit->getDeclareWarMove(pPlot);

		// only ask if option is off or moving into rival territory without open borders
		if (eRivalTeam != NO_TEAM && (pPlot->getTeam() == eRivalTeam || bAskToDeclareWar))
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DECLAREWARMOVE);
			if (pInfo)
			{
				pInfo->setData1(eRivalTeam);
				pInfo->setData2(pPlot->getX());
				pInfo->setData3(pPlot->getY());
				pInfo->setOption1(bShift);
				pInfo->setOption2(pPlot->getTeam() != eRivalTeam);
				gDLL->getInterfaceIFace()->addPopup(pInfo);
			}
			return;
		}
	}

	selectionListGameNetMessageInternal(GAMEMESSAGE_PUSH_MISSION, MISSION_MOVE_TO, pPlot->getX(), pPlot->getY(), 0, false, bShift, false);
}


void CvGame::selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift) const
{
	selectionListGameNetMessageInternal(eMessage, iData2, iData3, iData4, iFlags, bAlt, bShift, true);
}

void CvGame::selectionListGameNetMessageInternal(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift, bool bInViewportCoordinates) const
{
	CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if (!pHeadSelectedUnit || pHeadSelectedUnit->getOwner() != getActivePlayer())
	{
		return;
	}

	switch(eMessage)
	{
		case GAMEMESSAGE_JOIN_GROUP:
		{
			foreach_(const CvUnit* pSelectedUnit, gDLL->getInterfaceIFace()->getSelectionList()->units())
			{
				if (bShift || pSelectedUnit == pHeadSelectedUnit)
				{
					CvMessageControl::getInstance().sendJoinGroup(pSelectedUnit->getID(), FFreeList::INVALID_INDEX);
				}
				else
				{
					CvMessageControl::getInstance().sendJoinGroup(pSelectedUnit->getID(), pHeadSelectedUnit->getID());
				}
			}
			if (bShift)
			{
				gDLL->getInterfaceIFace()->selectUnit(pHeadSelectedUnit, true);
			}
			break;
		}
		case GAMEMESSAGE_DO_COMMAND:
		{
			if (iData2 == COMMAND_DELETE && bAlt)
			{
				const UnitTypes eType = pHeadSelectedUnit->getUnitType();

				foreach_(const CvUnit* pLoopUnit, GET_PLAYER(pHeadSelectedUnit->getOwner()).units())
				{
					if (pLoopUnit->getUnitType() == eType)
					{
						CvMessageControl::getInstance().sendDoCommand(pLoopUnit->getID(), (CommandTypes)iData2, iData3, iData4, bAlt);
					}
				}
			}
			else
			{
				foreach_(const CvUnit* pSelectedUnit, gDLL->getInterfaceIFace()->getSelectionList()->units())
				{
					CvMessageControl::getInstance().sendDoCommand(pSelectedUnit->getID(), (CommandTypes)iData2, iData3, iData4, bAlt);
				}
			}
			break;
		}
		case GAMEMESSAGE_PUSH_MISSION:
		case GAMEMESSAGE_AUTO_MISSION:
		{
			if (!gDLL->getInterfaceIFace()->mirrorsSelectionGroup())
			{
				selectionListGameNetMessage(GAMEMESSAGE_JOIN_GROUP);
			}

			if (eMessage != GAMEMESSAGE_PUSH_MISSION)
			{
				CvMessageControl::getInstance().sendAutoMission(pHeadSelectedUnit->getID());
			}
			else if (bAlt && (iData2 == MISSION_FORTIFY || iData2 == MISSION_SLEEP || iData2 == MISSION_BUILDUP || iData2 == MISSION_AUTO_BUILDUP))
			{
				const UnitTypes eUnit = pHeadSelectedUnit->getUnitType();

				foreach_(const CvSelectionGroup* pLoopSelectionGroup, GET_PLAYER(pHeadSelectedUnit->getOwner()).groups())
				{
					if (pLoopSelectionGroup->allMatch(eUnit))
					{
						CvMessageControl::getInstance().sendPushMission(pLoopSelectionGroup->getHeadUnit()->getID(), (MissionTypes)iData2, iData3, iData4, iFlags, bShift);
					}
				}
			}
			else
			{
				//	For missions that take plot coordinates, convert to map coordinates
				if (bInViewportCoordinates)
				{
					switch(iData2)
					{
						case MISSION_MOVE_TO:
						case MISSION_ROUTE_TO:
						case MISSION_NUKE:
						case MISSION_RECON:
						case MISSION_PARADROP:
						case MISSION_AIRLIFT:
						case MISSION_AIRBOMB:
						case MISSION_RANGE_ATTACK:
						case MISSION_AIRBOMB1:
						case MISSION_AIRBOMB2:
						case MISSION_AIRBOMB3:
						case MISSION_AIRBOMB4:
						case MISSION_AIRBOMB5:
						case MISSION_RBOMBARD:
						case MISSION_FENGAGE:
						case MISSION_CLAIM_TERRITORY:
						{
							iData3 = GC.getCurrentViewport()->getMapXFromViewportX(iData3);
							iData4 = GC.getCurrentViewport()->getMapYFromViewportY(iData4);
							break;
						}
						default: break;
					}
				}
				CvMessageControl::getInstance().sendPushMission(pHeadSelectedUnit->getID(), (MissionTypes)iData2, iData3, iData4, iFlags, bShift);
			}
			break;
		}
		default: FErrorMsg("error");
	}
}


void CvGame::selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl) const
{
	CLLNode<IDInfo>* pSelectedCityNode = gDLL->getInterfaceIFace()->headSelectedCitiesNode();

	while (pSelectedCityNode)
	{
		CvCity* pSelectedCity = ::getCity(pSelectedCityNode->m_data);
		pSelectedCityNode = gDLL->getInterfaceIFace()->nextSelectedCitiesNode(pSelectedCityNode);

		if (pSelectedCity && pSelectedCity->getOwner() == getActivePlayer())
		{
			switch (eMessage)
			{
			case GAMEMESSAGE_PUSH_ORDER:
				cityPushOrder(pSelectedCity, (OrderTypes)iData2, iData3, bAlt, bShift, bCtrl);
				break;

			case GAMEMESSAGE_POP_ORDER:
				if (pSelectedCity->getOrderQueueLength() > 1)
				{
					CvMessageControl::getInstance().sendPopOrder(pSelectedCity->getID(), iData2);
				}
				break;

			case GAMEMESSAGE_DO_TASK:
				CvMessageControl::getInstance().sendDoTask(pSelectedCity->getID(), ((TaskTypes)iData2), iData3, iData4, bOption, bAlt, bShift, bCtrl);
				break;

			default:
				FErrorMsg("error");
				break;
			}
		}
	}
}


bool CvGame::canHandleAction(int iAction, CvPlot* pPlot, bool bTestVisible, bool bUseCache) const
{
	PROFILE_FUNC();

	if (canDoControl((ControlTypes)GC.getActionInfo(iAction).getControlType()))
	{
		return true;
	}

	if (gDLL->getInterfaceIFace()->isCitySelection())
	{
		return false; // XXX hack!
	}

	CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

	if (pHeadSelectedUnit && pHeadSelectedUnit->getOwner() == getActivePlayer()
	&& (isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || GET_PLAYER(pHeadSelectedUnit->getOwner()).isTurnActive()))
	{
		CvSelectionGroup* pSelectedInterfaceList = gDLL->getInterfaceIFace()->getSelectionList();

		if (GC.getActionInfo(iAction).getMissionType() != NO_MISSION)
		{
			CvPlot* pMissionPlot = NULL;

			if (gDLL->getInterfaceIFace()->mirrorsSelectionGroup())
			{
				CvSelectionGroup* pSelectedGroup = pHeadSelectedUnit->getGroup();

				if (pPlot)
				{
					pMissionPlot = pPlot;
				}
				else if (gDLL->shiftKey())
				{
					pMissionPlot = pSelectedGroup->lastMissionPlot();
				}

				if (!pMissionPlot || !pMissionPlot->isVisible(pHeadSelectedUnit->getTeam(), false))
				{
					pMissionPlot = pSelectedGroup->plot();
				}
			}
			else pMissionPlot = pSelectedInterfaceList->plot();


			if (pSelectedInterfaceList->canStartMission(GC.getActionInfo(iAction).getMissionType(), GC.getActionInfo(iAction).getMissionData(), -1, pMissionPlot, bTestVisible, bUseCache))
			{
				return true;
			}
		}

		if (GC.getActionInfo(iAction).getCommandType() != NO_COMMAND
		&& pSelectedInterfaceList->canDoCommand((CommandTypes)GC.getActionInfo(iAction).getCommandType(), GC.getActionInfo(iAction).getCommandData(), -1, bTestVisible, bUseCache))
		{
			return true;
		}

		if (gDLL->getInterfaceIFace()->canDoInterfaceMode((InterfaceModeTypes)GC.getActionInfo(iAction).getInterfaceModeType(), pSelectedInterfaceList))
		{
			return true;
		}
	}

	return false;
}

void CvGame::setupActionCache() const
{
	gDLL->getInterfaceIFace()->getSelectionList()->setupActionCache();
}

void CvGame::handleAction(int iAction)
{
	if (!gDLL->getInterfaceIFace()->canHandleAction(iAction))
	{
		return;
	}
	const bool bAlt = gDLL->altKey();
	const bool bShift = gDLL->shiftKey();
	CvActionInfo& action = GC.getActionInfo(iAction);

	doControl((ControlTypes)action.getControlType());

	if (gDLL->getInterfaceIFace()->canDoInterfaceMode((InterfaceModeTypes)action.getInterfaceModeType(), gDLL->getInterfaceIFace()->getSelectionList()))
	{
		CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();

		if (pHeadSelectedUnit)
		{
			if (GC.getInterfaceModeInfo((InterfaceModeTypes)action.getInterfaceModeType()).getSelectAll())
			{
				gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, false, true);
			}
			else if (GC.getInterfaceModeInfo((InterfaceModeTypes)action.getInterfaceModeType()).getSelectType())
			{
				gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, true, false);
			}
		}
		gDLL->getInterfaceIFace()->setInterfaceMode((InterfaceModeTypes)action.getInterfaceModeType());
	}

	{
		const MissionTypes eMission = (MissionTypes)action.getMissionType();

		if (eMission != NO_MISSION)
		{
			bool bSkip = false;

			if (eMission == MISSION_GOTO)
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_GOTO_CITY);
				if (pInfo)
				{
					bSkip = true;
					gDLL->getInterfaceIFace()->addPopup(pInfo);
				}
			}

			if (eMission == MISSION_DISCOVER)
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_SELECT_DISCOVERY_TECH);
				if (pInfo)
				{
					bSkip = true;
					gDLL->getInterfaceIFace()->addPopup(pInfo);
				}
			}

			if (!bSkip)
			{
				selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, eMission, action.getMissionData(), -1, 0, bAlt, bShift);
			}
		}
	}

	if (action.getCommandType() != NO_COMMAND)
	{
		bool bSkip = false;
		if (action.getCommandType() == COMMAND_LOAD)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_LOADUNIT);
			if (pInfo)
			{
				gDLL->getInterfaceIFace()->addPopup(pInfo);
				bSkip = true;
			}
		}
		if (!bSkip)
		{
			if (action.isConfirmCommand())
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRMCOMMAND);
				if (pInfo)
				{
					pInfo->setData1(iAction);
					pInfo->setOption1(bAlt);
					gDLL->getInterfaceIFace()->addPopup(pInfo);
				}
			}
			else
			{
				selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, action.getCommandType(), action.getCommandData(), -1, 0, bAlt);
			}
		}
	}
}

bool CvGame::canDoControl(ControlTypes eControl) const
{
	switch (eControl)
	{
		case NO_CONTROL:
		{
			return false;
		}
		case CONTROL_SELECTYUNITTYPE:
		case CONTROL_SELECTYUNITALL:
		case CONTROL_SELECT_HEALTHY:
		case CONTROL_SELECTCITY:
		case CONTROL_SELECTCAPITAL:
		case CONTROL_NEXTUNIT:
		case CONTROL_PREVUNIT:
		case CONTROL_CYCLEUNIT:
		case CONTROL_CYCLEUNIT_ALT:
		case CONTROL_CYCLEWORKER:
		case CONTROL_LASTUNIT:
		case CONTROL_AUTOMOVES:
		case CONTROL_SAVE_GROUP:
		case CONTROL_QUICK_SAVE:
		case CONTROL_QUICK_LOAD:
		case CONTROL_ORTHO_CAMERA:
		case CONTROL_CYCLE_CAMERA_FLYING_MODES:
		case CONTROL_ISOMETRIC_CAMERA_LEFT:
		case CONTROL_ISOMETRIC_CAMERA_RIGHT:
		case CONTROL_FLYING_CAMERA:
		case CONTROL_MOUSE_FLYING_CAMERA:
		case CONTROL_TOP_DOWN_CAMERA:
		case CONTROL_TURN_LOG:
		case CONTROL_CHAT_ALL:
		case CONTROL_CHAT_TEAM:
		case CONTROL_GLOBE_VIEW:
		{
			if (!gDLL->getInterfaceIFace()->isFocused())
			{
				return true;
			}
			break;
		}
		case CONTROL_FORCEENDTURN:
		{
			if (!gDLL->getInterfaceIFace()->isFocused()
			&&  !gDLL->getInterfaceIFace()->isInAdvancedStart()
			&&  !gDLL->getInterfaceIFace()->isDiploOrPopupWaiting()
			&& !GET_PLAYER(getActivePlayer()).hasIdleCity())
			{
				return true;
			}
			break;
		}
		case CONTROL_PING:
		case CONTROL_SIGN:
		case CONTROL_GRID:
		case CONTROL_YIELDS:
		case CONTROL_RESOURCE_ALL:
		case CONTROL_UNIT_ICONS:
		case CONTROL_GLOBELAYER:
		case CONTROL_FREE_COLONY:
		{
			if (!gDLL->getInterfaceIFace()->isFocusedWidget())
			{
				return true;
			}
			break;
		}
		case CONTROL_OPTIONS_SCREEN:
		case CONTROL_DOMESTIC_SCREEN:
		case CONTROL_VICTORY_SCREEN:
		case CONTROL_CIVILOPEDIA:
		case CONTROL_RELIGION_SCREEN:
		case CONTROL_CORPORATION_SCREEN:
		case CONTROL_CIVICS_SCREEN:
		case CONTROL_FOREIGN_SCREEN:
		case CONTROL_FINANCIAL_SCREEN:
		case CONTROL_MILITARY_SCREEN:
		case CONTROL_TECH_CHOOSER:
		case CONTROL_DIPLOMACY:
		case CONTROL_HALL_OF_FAME:
		case CONTROL_INFO:
		case CONTROL_DETAILS:
		case CONTROL_SAVE_NORMAL:
		case CONTROL_ESPIONAGE_SCREEN:
		{
			return true;
		}
		case CONTROL_NEXTCITY:
		case CONTROL_PREVCITY:
		{
			if (!gDLL->getInterfaceIFace()->isSpaceshipScreenUp())
			{
				return true;
			}
			break;
		}
		case CONTROL_ADMIN_DETAILS:
		{
			return true;
		}
		case CONTROL_CENTERONSELECTION:
		{
			if (gDLL->getInterfaceIFace()->getLookAtPlot() != gDLL->getInterfaceIFace()->getSelectionPlot())
			{
				return true;
			}
			break;
		}
		case CONTROL_LOAD_GAME:
		{
			if (!isNetworkMultiPlayer())
			{
				return true;
			}
			break;
		}
		case CONTROL_RETIRE:
		{
			if ((getGameState() == GAMESTATE_ON || isGameMultiPlayer()) && GET_PLAYER(getActivePlayer()).isAlive())
			{
				if (!isPbem() && !isHotSeat())
				{
					return true;
				}
				if (!GET_PLAYER(getActivePlayer()).isEndTurn())
				{
					return true;
				}
			}
			break;
		}
		case CONTROL_WORLD_BUILDER:
		{
			if (!isGameMultiPlayer() && !gDLL->getInterfaceIFace()->isInAdvancedStart())
			{
				return true;
			}
			break;
		}
		case CONTROL_ENDTURN:
		case CONTROL_ENDTURN_ALT:
		{
			if (gDLL->getInterfaceIFace()->isEndTurnMessage() && !gDLL->getInterfaceIFace()->isFocused() && !gDLL->getInterfaceIFace()->isInAdvancedStart())
			{
				return true;
			}
			break;
		}
		default:
		{
			FErrorMsg("eControl did not match any valid options");
			break;
		}
	}
	return false;
}


static void selectClosestCity(bool bAdd)
{
	CvCity* pSelectCity = GET_PLAYER(GC.getGame().getActivePlayer()).findClosestCity(gDLL->getInterfaceIFace()->getLookAtPlot());
	if (pSelectCity)
	{
		if (GC.getCurrentViewport()->isInViewport(pSelectCity->getX(), pSelectCity->getY(), GC.getVIEWPORT_FOCUS_BORDER()))
		{
			if (bAdd)
			{
				gDLL->getInterfaceIFace()->clearSelectedCities();
				gDLL->getInterfaceIFace()->addSelectedCity(pSelectCity);
				gDLL->getInterfaceIFace()->lookAtSelectionPlot();
			}
			else gDLL->getInterfaceIFace()->selectCity(pSelectCity);
		}
		else GC.getCurrentViewport()->bringIntoView(pSelectCity->getX(), pSelectCity->getY(), NULL, true, !bAdd, true, bAdd);
	}
}


void CvGame::doControl(ControlTypes eControl)
{
	if (!canDoControl(eControl))
	{
		return;
	}
	switch (eControl)
	{
		case CONTROL_CENTERONSELECTION:
		{
			CvPlot* pPlot = gDLL->getInterfaceIFace()->getSelectionPlot();
			if (pPlot)
			{
				if (!pPlot->isInViewport(GC.getViewportCenteringBorder()))
				{
					GC.getCurrentViewport()->bringIntoView(pPlot->getX(), pPlot->getY(), gDLL->getInterfaceIFace()->getHeadSelectedUnit(), true);
				}
				else gDLL->getInterfaceIFace()->lookAtSelectionPlot();
			}
			break;
		}
		case CONTROL_SELECTYUNITTYPE:
		{
			CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
			if (pHeadSelectedUnit)
			{
				gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, true, false);
			}
			break;
		}
		case CONTROL_SELECTYUNITALL:
		{
			CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
			if (pHeadSelectedUnit)
			{
				gDLL->getInterfaceIFace()->selectGroup(pHeadSelectedUnit, false, false, true);
			}
			break;
		}
		case CONTROL_SELECT_HEALTHY:
		{
			CvUnit* pGroupHead = NULL;
			CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
			gDLL->getInterfaceIFace()->clearSelectionList();
			if (pHeadSelectedUnit)
			{
				CvPlot* pPlot = pHeadSelectedUnit->plot();
				std::vector<CvUnit *> plotUnits;
				getPlotUnits(pPlot, plotUnits);
				gDLL->getInterfaceIFace()->selectionListPreChange();
				for (int iI = 0; iI < (int) plotUnits.size(); iI++)
				{
					CvUnit* pUnit = plotUnits[iI];

					if (pUnit->getOwner() == getActivePlayer()
					&& (!isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || getTurnSlice() - pUnit->getLastMoveTurn() > GC.getDefineINT("MIN_TIMER_UNIT_DOUBLE_MOVES"))
					&& pUnit->isHurt())
					{
						if (pGroupHead)
						{
							CvMessageControl::getInstance().sendJoinGroup(pUnit->getID(), pGroupHead->getID());
						}
						else pGroupHead = pUnit;

						gDLL->getInterfaceIFace()->insertIntoSelectionList(pUnit, false, false, true, true, true);
					}
				}
				gDLL->getInterfaceIFace()->selectionListPostChange();
			}
			break;
		}
		case CONTROL_SELECTCITY:
		{
			if (gDLL->getInterfaceIFace()->isCityScreenUp())
			{
				cycleCities();
			}
			else
			{
				selectClosestCity(false);
				//gDLL->getInterfaceIFace()->selectLookAtCity();
			}
			break;
		}
		case CONTROL_SELECTCAPITAL:
		{
			CvCity* pCapitalCity = GET_PLAYER(getActivePlayer()).getCapitalCity();
			if (pCapitalCity)
			{
				if (GC.getCurrentViewport()->isInViewport(pCapitalCity->getX(), pCapitalCity->getY(), GC.getVIEWPORT_FOCUS_BORDER()))
				{
					gDLL->getInterfaceIFace()->selectCity(pCapitalCity);
				}
				else
				{
					GC.getCurrentViewport()->bringIntoView(pCapitalCity->getX(), pCapitalCity->getY(), NULL, true, gDLL->getInterfaceIFace()->isCityScreenUp());
				}
			}
			break;
		}
		case CONTROL_NEXTCITY:
		{
			if (gDLL->getInterfaceIFace()->isCitySelection())
			{
				cycleCities(true, !(gDLL->getInterfaceIFace()->isCityScreenUp()));
			}
			else
			{
				selectClosestCity(!gDLL->getInterfaceIFace()->isCityScreenUp());
				//gDLL->getInterfaceIFace()->selectLookAtCity(true);
			}
			break;
		}
		case CONTROL_PREVCITY:
		{
			if (gDLL->getInterfaceIFace()->isCitySelection())
			{
				cycleCities(false, !(gDLL->getInterfaceIFace()->isCityScreenUp()));
			}
			else
			{
				selectClosestCity(!gDLL->getInterfaceIFace()->isCityScreenUp());
				//gDLL->getInterfaceIFace()->selectLookAtCity(true);
			}
			break;
		}
		case CONTROL_NEXTUNIT:
		{
			const CvPlot* pPlot = gDLL->getInterfaceIFace()->getSelectionPlot();
			if (pPlot)
			{
				nextPlotUnit(pPlot);
			}
			break;
		}
		case CONTROL_PREVUNIT:
		{
			const CvPlot* pPlot = gDLL->getInterfaceIFace()->getSelectionPlot();
			if (pPlot)
			{
				nextPlotUnit(pPlot, false);
			}
			break;
		}
		case CONTROL_CYCLEUNIT:
		case CONTROL_CYCLEUNIT_ALT:
		{
			const CvUnit* prevUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
			cycleSelectionGroups();
			if (prevUnit && prevUnit == gDLL->getInterfaceIFace()->getHeadSelectedUnit() && prevUnit->getGroup()->getActivityType() != ACTIVITY_AWAKE)
			{
				gDLL->getInterfaceIFace()->clearSelectionList();
			}
			break;
		}
		case CONTROL_CYCLEWORKER:
		{
			cycleSelectionGroups(true, true, true);
			break;
		}
		case CONTROL_LASTUNIT:
		{
			CvUnit* pUnit = gDLL->getInterfaceIFace()->getLastSelectedUnit();

			if (pUnit)
			{
				gDLL->getInterfaceIFace()->selectUnit(pUnit, true);
				gDLL->getInterfaceIFace()->lookAtSelectionPlot();
			}
			else
			{
				cycleSelectionGroups(true, false);
			}
			gDLL->getInterfaceIFace()->setLastSelectedUnit(NULL);
			break;
		}
		case CONTROL_ENDTURN:
		case CONTROL_ENDTURN_ALT:
		{
			if (gDLL->getInterfaceIFace()->isEndTurnMessage())
			{
				CvMessageControl::getInstance().sendTurnComplete();
			}
			break;
		}
		case CONTROL_FORCEENDTURN:
		{
			CvMessageControl::getInstance().sendTurnComplete();
			break;
		}
		case CONTROL_AUTOMOVES:
		{
			CvMessageControl::getInstance().sendAutoMoves();
			break;
		}
		case CONTROL_PING:
		{
			gDLL->getInterfaceIFace()->setInterfaceMode(INTERFACEMODE_PING);
			break;
		}
		case CONTROL_SIGN:
		{
			gDLL->getInterfaceIFace()->setInterfaceMode(INTERFACEMODE_SIGN);
			break;
		}
		case CONTROL_GRID:
		{
			gDLL->getEngineIFace()->SetGridMode(!(gDLL->getEngineIFace()->GetGridMode()));
			break;
		}
		case CONTROL_YIELDS:
		{
			gDLL->getInterfaceIFace()->toggleYieldVisibleMode();
			break;
		}
		case CONTROL_RESOURCE_ALL:
		{
			GC.setResourceLayer(!GC.getResourceLayer());
			//gDLL->getEngineIFace()->toggleResourceLayer();
			break;
		}
		case CONTROL_UNIT_ICONS:
		{
			gDLL->getEngineIFace()->toggleUnitLayer();
			break;
		}
		case CONTROL_GLOBELAYER:
		{
			gDLL->getEngineIFace()->toggleGlobeview();
			break;
		}
		case CONTROL_LOAD_GAME:
		{
			gDLL->LoadGame();
			break;
		}
		case CONTROL_OPTIONS_SCREEN:
		{
			Cy::call("CvScreensInterface", "showOptionsScreen");
			break;
		}
		case CONTROL_RETIRE:
		{
			if (!isGameMultiPlayer() || countHumanPlayersAlive() == 1)
			{
				if (gDLL->GetAutorun())
				{
					GC.getInitCore().setSlotStatus(getActivePlayer(), SS_COMPUTER);
				}
				else
				{
					setGameState(GAMESTATE_OVER);
					gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
				}
			}
			else if (isNetworkMultiPlayer())
			{
				gDLL->sendMPRetire();
				gDLL->getInterfaceIFace()->exitingToMainMenu();
			}
			else
			{
				gDLL->handleRetirement(getActivePlayer());
			}
			break;
		}
		case CONTROL_SAVE_GROUP:
		{
			gDLL->SaveGame(SAVEGAME_GROUP);
			break;
		}
		case CONTROL_SAVE_NORMAL:
		{
			if (GC.getDefineINT("ALWAYS_USE_MAX_COMPAT_SAVES"))
			{
				gDLL->SaveGame(SAVEGAME_NORMAL);
			}
			else
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_GET_SAVE_FORMAT);
				if (pInfo)
				{
					gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true, true);
				}
			}

			break;
		}
		case CONTROL_QUICK_SAVE:
		{
			if (!(isNetworkMultiPlayer()))	// SP only!
			{
				gDLL->QuickSave();
			}
			break;
		}
		case CONTROL_QUICK_LOAD:
		{
			if (!(isNetworkMultiPlayer()))	// SP only!
			{
				gDLL->QuickLoad();
			}
			break;
		}
		case CONTROL_ORTHO_CAMERA:
		{
			gDLL->getEngineIFace()->SetOrthoCamera(!(gDLL->getEngineIFace()->GetOrthoCamera()));
			break;
		}
		case CONTROL_CYCLE_CAMERA_FLYING_MODES:
		{
			gDLL->getEngineIFace()->CycleFlyingMode(1);
			break;
		}
		case CONTROL_ISOMETRIC_CAMERA_LEFT:
		{
			gDLL->getEngineIFace()->MoveBaseTurnLeft();
			break;
		}
		case CONTROL_ISOMETRIC_CAMERA_RIGHT:
		{
			gDLL->getEngineIFace()->MoveBaseTurnRight();
			break;
		}
		case CONTROL_FLYING_CAMERA:
		{
			gDLL->getEngineIFace()->SetFlying(!(gDLL->getEngineIFace()->GetFlying()));
			break;
		}
		case CONTROL_MOUSE_FLYING_CAMERA:
		{
			gDLL->getEngineIFace()->SetMouseFlying(!(gDLL->getEngineIFace()->GetMouseFlying()));
			break;
		}
		case CONTROL_TOP_DOWN_CAMERA:
		{
			gDLL->getEngineIFace()->SetSatelliteMode(!(gDLL->getEngineIFace()->GetSatelliteMode()));
			break;
		}
		case CONTROL_CIVILOPEDIA:
		{
			GC.setIsInPedia(true);
			Cy::call(PYScreensModule, "pediaShow");
			break;
		}
		case CONTROL_RELIGION_SCREEN:
		{
			Cy::call(PYScreensModule, "showReligionScreen");
			break;
		}
		case CONTROL_CORPORATION_SCREEN:
		{
			Cy::call(PYScreensModule, "showCorporationScreen");
			break;
		}
		case CONTROL_CIVICS_SCREEN:
		{
			Cy::call(PYScreensModule, "showCivicsScreen");
			break;
		}
		case CONTROL_FOREIGN_SCREEN:
		{
			Cy::call(PYScreensModule, "showForeignAdvisorScreen", Cy::Args() << -1);
			break;
		}
		case CONTROL_FINANCIAL_SCREEN:
		{
			Cy::call(PYScreensModule, "showFinanceAdvisor");
			break;
		}
		case CONTROL_MILITARY_SCREEN:
		{
			if (GC.viewportsEnabled())
			{
				GC.getCurrentViewport()->setActionState(VIEWPORT_ACTION_STATE_LAUNCH_MILITARY_ADVISOR, true);
			}
			else Cy::call(PYScreensModule, "showMilitaryAdvisor");

			break;
		}
		case CONTROL_TECH_CHOOSER:
		{
			Cy::call(PYScreensModule, "showTechChooser");
			break;
		}
		case CONTROL_TURN_LOG:
		{
			if (!gDLL->GetWorldBuilderMode() || gDLL->getInterfaceIFace()->isInAdvancedStart())
			{
				gDLL->getInterfaceIFace()->toggleTurnLog();
			}
			break;
		}
		case CONTROL_CHAT_ALL:
		{
			if (!gDLL->GetWorldBuilderMode() || gDLL->getInterfaceIFace()->isInAdvancedStart())
			{
				gDLL->getInterfaceIFace()->showTurnLog(CHATTARGET_ALL);
			}
			break;
		}
		case CONTROL_CHAT_TEAM:
		{
			if (!gDLL->GetWorldBuilderMode() || gDLL->getInterfaceIFace()->isInAdvancedStart())
			{
				gDLL->getInterfaceIFace()->showTurnLog(CHATTARGET_TEAM);
			}
			break;
		}
		case CONTROL_DOMESTIC_SCREEN:
		{
			Cy::call(PYScreensModule, "showDomesticAdvisor", Cy::Args() << -1);
			break;
		}
		case CONTROL_VICTORY_SCREEN:
		{
			Cy::call(PYScreensModule, "showVictoryScreen");
			break;
		}
		case CONTROL_INFO:
		{
			Cy::call(PYScreensModule, "showInfoScreen", Cy::Args() << -1 << (getGameState() == GAMESTATE_ON ? 0 : 1));
			break;
		}
		case CONTROL_GLOBE_VIEW:
		{
			gDLL->getEngineIFace()->toggleGlobeview();
			break;
		}
		case CONTROL_DETAILS:
		{
			gDLL->getInterfaceIFace()->showDetails();
			break;
		}
		case CONTROL_ADMIN_DETAILS:
		{
			if (GC.getInitCore().getAdminPassword().empty())
			{
				gDLL->getInterfaceIFace()->showAdminDetails();
			}
			else
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_ADMIN_PASSWORD);
				if (pInfo)
				{
					pInfo->setData1((int)CONTROL_ADMIN_DETAILS);
					gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true);
				}
			}
			break;
		}
		case CONTROL_HALL_OF_FAME:
		{
			Cy::call(PYScreensModule, "showHallOfFame", Cy::Args() << true);
			break;
		}
		case CONTROL_WORLD_BUILDER:
		{
			if (GC.getInitCore().getAdminPassword().empty())
			{
				setWorldBuilder(!gDLL->GetWorldBuilderMode());
			}
			else
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_ADMIN_PASSWORD);
				if (pInfo)
				{
					pInfo->setData1((int)CONTROL_WORLD_BUILDER);
					gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true);
				}
			}
			break;
		}
		case CONTROL_ESPIONAGE_SCREEN:
		{
			Cy::call(PYScreensModule, "showEspionageAdvisor");
			break;
		}
		case CONTROL_FREE_COLONY:
		{
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FREE_COLONY);
				if (pInfo)
				{
					gDLL->getInterfaceIFace()->addPopup(pInfo);
				}
			}
			break;
		}
		case CONTROL_DIPLOMACY:
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DIPLOMACY);
			if (pInfo)
			{
				gDLL->getInterfaceIFace()->addPopup(pInfo);
			}
			break;
		}
		default:
		{
			FErrorMsg("eControl did not match any valid options");
			break;
		}
	}
}

void CvGame::getGlobeLayers(std::vector<CvGlobeLayerData>& aLayers) const
{
	CvGlobeLayerData kUnit(GLOBE_LAYER_UNIT);
	kUnit.m_strName = "UNITS";
	kUnit.m_strButtonHelpTag = "TXT_KEY_GLOBELAYER_UNITS";
	kUnit.m_strButtonStyle = "Button_HUDGlobeUnit_Style";
	kUnit.m_iNumOptions = NUM_UNIT_OPTION_TYPES;
	kUnit.m_bGlobeViewRequired = false;
	aLayers.push_back(kUnit);

	CvGlobeLayerData kResource(GLOBE_LAYER_RESOURCE);
	kResource.m_strName = "RESOURCES";
	kResource.m_strButtonHelpTag = "TXT_KEY_GLOBELAYER_RESOURCES";
	kResource.m_strButtonStyle = "Button_HUDBtnResources_Style";
	kResource.m_iNumOptions = NUM_RESOURCE_OPTION_TYPES;
	kResource.m_bGlobeViewRequired = false;
	aLayers.push_back(kResource);

	CvGlobeLayerData kCulture(GLOBE_LAYER_CULTURE);
	kCulture.m_strName = "CULTURE";
	kCulture.m_strButtonHelpTag = "TXT_KEY_GLOBELAYER_CULTURE";
	kCulture.m_strButtonStyle = "Button_HUDGlobeCulture_Style";
	kCulture.m_bShouldCitiesZoom = true;
	aLayers.push_back(kCulture);

	CvGlobeLayerData kTrade(GLOBE_LAYER_TRADE);
	kTrade.m_strName = "TRADE";
	kTrade.m_strButtonHelpTag = "TXT_KEY_GLOBELAYER_TRADE";
	kTrade.m_strButtonStyle = "Button_HUDGlobeTrade_Style";
	kTrade.m_iNumOptions = 3;
	kTrade.m_bShouldCitiesZoom = true;
	aLayers.push_back(kTrade);

	CvGlobeLayerData kReligion(GLOBE_LAYER_RELIGION);
	kReligion.m_strName = "RELIGION";
	kReligion.m_strButtonHelpTag = "TXT_KEY_GLOBELAYER_RELIGION";
	kReligion.m_strButtonStyle = "Button_HUDGlobeReligion_Style";
	kReligion.m_iNumOptions = GC.getNumReligionInfos();
	kReligion.m_bShouldCitiesZoom = true;
	aLayers.push_back(kReligion);
}

void CvGame::startFlyoutMenu(const CvPlot* pPlot, std::vector<CvFlyoutMenuData>& aFlyoutItems) const
{
	aFlyoutItems.clear();

	bool bUnits = false;
	bool bFortifyUnit = false;
	bool bSleepUnit = false;
	bool bWakeUnit = false;

	foreach_(const CvUnit* pLoopUnit, pPlot->units())
	{
		if (pLoopUnit->getOwner() == getActivePlayer())
		{
			bUnits = true;

			if (pLoopUnit->canFortify())
			{
				bFortifyUnit = true;
			}
			else if (pLoopUnit->canSleep())
			{
				bSleepUnit = true;
			}
			else if (pLoopUnit->isWaiting())
			{
				bWakeUnit = true;
			}
		}
	}
	CvWString szBuffer;
	const CvCity* pCity = pPlot->getPlotCity();

	if (pCity && pCity->getOwner() == getActivePlayer())
	{
		szBuffer = gDLL->getText("TXT_KEY_CHANGE_PRODUCTION");
		aFlyoutItems.push_back(CvFlyoutMenuData(NO_FLYOUT, -1, -1, -1, szBuffer));
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (pCity->canTrain((UnitTypes) iI))
			{
				szBuffer.Format(L"%s (%d)", GC.getUnitInfo((UnitTypes) iI).getDescription(), pCity->getProductionTurnsLeft((UnitTypes) iI, 0));
				aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_TRAIN, (UnitTypes) iI, pPlot->getX(), pPlot->getY(), szBuffer));
			}
		}

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			const BuildingTypes eLoopBuilding = static_cast<BuildingTypes>(iI);
			if (pCity->canConstruct(eLoopBuilding))
			{
				szBuffer.Format(L"%s (%d)", GC.getBuildingInfo(eLoopBuilding).getDescription(), pCity->getProductionTurnsLeft(eLoopBuilding, 0));
				aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_CONSTRUCT, eLoopBuilding, pPlot->getX(), pPlot->getY(), szBuffer));
			}
		}

		for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			ProjectTypes eProject = (ProjectTypes) iI;
			if (pCity->canCreate(eProject))
			{
				szBuffer.Format(L"%s (%d)", GC.getProjectInfo(eProject).getDescription(), pCity->getProductionTurnsLeft(eProject, 0));
				aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_CREATE, eProject, pPlot->getX(), pPlot->getY(), szBuffer));
			}
		}

		for (int iI = 0; iI < GC.getNumProcessInfos(); iI++)
		{
			if (pCity->canMaintain((ProcessTypes)iI))
			{
				szBuffer = GC.getProcessInfo((ProcessTypes)iI).getDescription();
				aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_MAINTAIN, iI, pPlot->getX(), pPlot->getY(), szBuffer));
			}
		}


		aFlyoutItems.push_back(CvFlyoutMenuData(NO_FLYOUT, -1, -1, -1, L""));

		for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
		{
			if (pCity->canHurry((HurryTypes)iI))
			{
				szBuffer = gDLL->getText("TXT_KEY_HURRY_PRODUCTION");

				const int64_t iHurryGold = pCity->getHurryGold((HurryTypes)iI);
				if (iHurryGold > 0)
				{
					szBuffer += gDLL->getText("TXT_KEY_HURRY_PRODUCTION_GOLD", iHurryGold);
				}

				const int iHurryPopulation = pCity->hurryPopulation((HurryTypes)iI);
				if (iHurryPopulation > 0)
				{
					szBuffer += gDLL->getText("TXT_KEY_HURRY_PRODUCTION_POP", iHurryPopulation);
				}

				aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_HURRY, iI, pPlot->getX(), pPlot->getY(), szBuffer));
			}
		}

		if (pCity->canConscript())
		{
			UnitTypes eConscriptUnit = pCity->getConscriptUnit();
			if (eConscriptUnit != NO_UNIT)
			{
				szBuffer = gDLL->getText("TXT_KEY_DRAFT_UNIT", GC.getUnitInfo(eConscriptUnit).getDescription(), pCity->getConscriptPopulation());
				aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_CONSCRIPT, iI, pPlot->getX(), pPlot->getY(), szBuffer));
			}
		}
	}

	const CvUnit* pHeadSelectedUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
	if (pHeadSelectedUnit && !pHeadSelectedUnit->atPlot(pPlot))
	{
		gDLL->getFAStarIFace()->SetData(&GC.getInterfacePathFinder(), gDLL->getInterfaceIFace()->getSelectionList());
		if ((pHeadSelectedUnit->getDomainType() == DOMAIN_AIR) || gDLL->getFAStarIFace()->GeneratePath(&GC.getInterfacePathFinder(), pHeadSelectedUnit->getViewportX(), pHeadSelectedUnit->getViewportY(), pPlot->getViewportX(), pPlot->getViewportY(), false, MOVE_DECLARE_WAR, true))
		{
			if (pHeadSelectedUnit->getDomainType() == DOMAIN_AIR)
			{
				szBuffer = gDLL->getText("TXT_KEY_FLYOUT_MENU_FLY_TO");
			}
			else
			{
				szBuffer = gDLL->getText("TXT_KEY_FLYOUT_MENU_MOVE_TO", gDLL->getFAStarIFace()->GetLastNode(&GC.getInterfacePathFinder())->m_iData2);
			}
			aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_MOVE_TO, 0, pPlot->getX(), pPlot->getY(), szBuffer));
		}
	}

	if (bUnits)
	{
		szBuffer = gDLL->getText("TXT_KEY_FLYOUT_MENU_SELECT_ALL");
		aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_SELECT_ALL, 0, pPlot->getX(), pPlot->getY(), szBuffer));

		if (bWakeUnit)
		{
			szBuffer = gDLL->getText("TXT_KEY_FLYOUT_MENU_WAKE_ALL");
			aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_WAKE_ALL, 0, pPlot->getX(), pPlot->getY(), szBuffer));
		}

		if (bFortifyUnit)
		{
			szBuffer = gDLL->getText("TXT_KEY_FLYOUT_MENU_FORTIFY_ALL");
			aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUR_FORTIFY_ALL, 0, pPlot->getX(), pPlot->getY(), szBuffer));
		}
		else if (bSleepUnit)
		{
			szBuffer = gDLL->getText("TXT_KEY_FLYOUT_MENU_SLEEP_ALL");
			aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUR_SLEEP_ALL, 0, pPlot->getX(), pPlot->getY(), szBuffer));
		}

		std::vector<CvUnit*> plotUnits;
		getPlotUnits(pPlot, plotUnits);
		CvWStringBuffer szTempBuffer;
		for (int iI = 0; iI < (int) plotUnits.size(); iI++)
		{
			CvUnit* pLoopUnit = plotUnits[iI];
			if (pLoopUnit->getOwner() == getActivePlayer())
			{
				CvWStringBuffer szTempBuffer;
				GAMETEXT.setUnitHelp(szTempBuffer, pLoopUnit, true);
				aFlyoutItems.push_back(CvFlyoutMenuData(FLYOUT_SELECT_UNIT, pLoopUnit->getID(), pPlot->getX(), pPlot->getY(), szTempBuffer.getCString()));
			}
		}
	}
}


void CvGame::applyFlyoutMenu(const CvFlyoutMenuData& kItem)
{
	CvPlot* pPlot = GC.getMap().plot(kItem.m_iX, kItem.m_iY);
	if (!pPlot) return;

	switch (kItem.m_eFlyout)
	{
		case FLYOUT_HURRY:
		{
			const CvCity* pCity = pPlot->getPlotCity();

			if (pCity && pCity->getOwner() == getActivePlayer())
			{
				CvMessageControl::getInstance().sendDoTask(pCity->getID(), TASK_HURRY, kItem.m_iID, -1, false, false, false, false);
			}
			break;
		}
		case FLYOUT_CONSCRIPT:
		{
			const CvCity* pCity = pPlot->getPlotCity();

			if (pCity && pCity->getOwner() == getActivePlayer())
			{
				CvMessageControl::getInstance().sendDoTask(pCity->getID(), TASK_CONSCRIPT, -1, -1, false, false, false, false);
			}
			break;
		}
		case FLYOUT_TRAIN:
		{
			const CvCity* pCity = pPlot->getPlotCity();

			if (pCity && pCity->getOwner() == getActivePlayer())
			{
				cityPushOrder(pCity, ORDER_TRAIN, kItem.m_iID);
			}
			break;
		}
		case FLYOUT_CONSTRUCT:
		{
			const CvCity* pCity = pPlot->getPlotCity();

			if (pCity && pCity->getOwner() == getActivePlayer())
			{
				cityPushOrder(pCity, ORDER_CONSTRUCT, kItem.m_iID);
			}
			break;
		}
		case FLYOUT_CREATE:
		{
			const CvCity* pCity = pPlot->getPlotCity();

			if (pCity && pCity->getOwner() == getActivePlayer())
			{
				cityPushOrder(pCity, ORDER_CREATE, kItem.m_iID);
			}
			break;
		}
		case FLYOUT_MAINTAIN:
		{
			const CvCity* pCity = pPlot->getPlotCity();

			if (pCity && pCity->getOwner() == getActivePlayer())
			{
				cityPushOrder(pCity, ORDER_MAINTAIN, kItem.m_iID);
			}
			break;
		}
		case FLYOUT_MOVE_TO:
		{
			selectionListMove(pPlot, false, false, false);
			break;
		}
		case FLYOUT_SELECT_UNIT:
			{
				CvUnit* pUnit = GET_PLAYER(getActivePlayer()).getUnit(kItem.m_iID);

				if (pUnit)
				{
					gDLL->getInterfaceIFace()->selectUnit(pUnit, true);
				}
			}
			break;

		case FLYOUT_SELECT_ALL:
		{
			gDLL->getInterfaceIFace()->selectAll(pPlot);
			break;
		}
		case FLYOUT_WAKE_ALL:
		{
			foreach_(const CvUnit* pLoopUnit, pPlot->units() | filtered(CvUnit::fn::isGroupHead()))
			{
				CvMessageControl::getInstance().sendDoCommand(pLoopUnit->getID(), COMMAND_WAKE, -1, -1, false);
			}
			break;
		}
		case FLYOUR_FORTIFY_ALL:
		case FLYOUR_SLEEP_ALL:
		{
			foreach_(const CvUnit* pLoopUnit, pPlot->units() | filtered(CvUnit::fn::isGroupHead()))
			{
				CvMessageControl::getInstance().sendPushMission(pLoopUnit->getID(), (pLoopUnit->isFortifyable() ? MISSION_FORTIFY : MISSION_SLEEP), -1, -1, 0, false);
			}
			break;
		}
		default:
		{
			FErrorMsg("error");
			break;
		}
	}
}

CvPlot* CvGame::getNewHighlightPlot() const
{
	if (gDLL->GetWorldBuilderMode())
	{
		bool bOK = false;
		std::vector<int> coords = Cy::call< std::vector<int> >(PYScreensModule, "WorldBuilderGetHighlightPlot", Cy::Args() << 0, &bOK);

		if (bOK && !coords.empty())
		{
			FAssertMsg(coords.size() >= 2, "Python callback WorldBuilderGetHighlightPlot didn't return valid coords array");
			return GC.getMap().plot(coords[0], coords[1]);
		}
	}
	else if (GC.getInterfaceModeInfo(gDLL->getInterfaceIFace()->getInterfaceMode()).getHighlightPlot())
	{
		return gDLL->getInterfaceIFace()->getMouseOverPlot();
	}
	return NULL;
}


ColorTypes CvGame::getPlotHighlightColor(CvPlot* pPlot) const
{
	if (!pPlot) return NO_COLOR;

	if (gDLL->GetWorldBuilderMode())
	{
		return (ColorTypes) GC.getInfoTypeForString("COLOR_GREEN");
	}
	switch (gDLL->getInterfaceIFace()->getInterfaceMode())
	{
		case INTERFACEMODE_PING:
		case INTERFACEMODE_SIGN:
		{
			if (pPlot->isRevealed(getActiveTeam(), true))
			{
				return (ColorTypes) GC.getInfoTypeForString("COLOR_GREEN");
			}
		}
		case INTERFACEMODE_DOTMAP: break;

		case INTERFACEMODE_SAVE_PLOT_NIFS:
		{
			return (ColorTypes) GC.getInfoTypeForString("COLOR_DARK_GREY");
		}
		default:
		{
			if (gDLL->getInterfaceIFace()->getSelectionList()->canDoInterfaceModeAt(gDLL->getInterfaceIFace()->getInterfaceMode(), pPlot))
			{
				return (ColorTypes) GC.getInfoTypeForString("COLOR_GREEN");
			}
			return (ColorTypes) GC.getInfoTypeForString("COLOR_DARK_GREY");
		}
	}
	return NO_COLOR;
}

void CvGame::loadBuildQueue(const CvString& strItem) const
{
	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (strItem == GC.getUnitInfo((UnitTypes) iI).getType())
		{
			selectedCitiesGameNetMessage(GAMEMESSAGE_PUSH_ORDER, ORDER_TRAIN, (UnitTypes) iI, -1, false, false, true);
			return;
		}
	}
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (strItem == GC.getBuildingInfo((BuildingTypes)iI).getType())
		{
			selectedCitiesGameNetMessage(GAMEMESSAGE_PUSH_ORDER, ORDER_CONSTRUCT, (BuildingTypes)iI, -1, false, false, true);
			return;
		}
	}
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (strItem == GC.getProjectInfo((ProjectTypes)iI).getType())
		{
			selectedCitiesGameNetMessage(GAMEMESSAGE_PUSH_ORDER, ORDER_CREATE, ((ProjectTypes)iI), -1, false, false, true);
			return;
		}
	}
	for (int iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		if (strItem == GC.getProcessInfo((ProcessTypes)iI).getType())
		{
			selectedCitiesGameNetMessage(GAMEMESSAGE_PUSH_ORDER, ORDER_MAINTAIN, ((ProcessTypes)iI), -1, false, false, true);
			return;
		}
	}
}

void CvGame::cheatSpaceship() const
{
	//add one space project that is still available
	CvTeam& kTeam = GET_TEAM(getActiveTeam());
	for (int i = 0; i < GC.getNumProjectInfos(); i++)
	{
		const ProjectTypes eProject = (ProjectTypes) i;
		const CvProjectInfo& kProject = GC.getProjectInfo(eProject);
		if (kProject.isSpaceship())
		{
			//cheat required projects
			for (int j = 0; j < GC.getNumProjectInfos(); j++)
			{
				ProjectTypes eRequiredProject = (ProjectTypes) j;
				int iNumReqProjects = kProject.getProjectsNeeded(eRequiredProject);
				while (kTeam.getProjectCount(eRequiredProject) < iNumReqProjects)
				{
					kTeam.changeProjectCount(eRequiredProject, 1);
				}
			}

			//cheat required techs
			const TechTypes eRequiredTech = kProject.getTechPrereq();
			if (!kTeam.isHasTech(eRequiredTech))
			{
				kTeam.setHasTech(eRequiredTech, true, getActivePlayer(), true, true);
			}

			//cheat one space component
			if (kTeam.getProjectCount(eProject) < kProject.getMaxTeamInstances())
			{
				kTeam.changeProjectCount(eProject, 1);

				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN, eProject);
				pInfo->setText(L"showSpaceShip");
				gDLL->getInterfaceIFace()->addPopup(pInfo, getActivePlayer());
			}
		}
	}
}

DomainTypes CvGame::getUnitDomain(UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getDomainType();
}

const CvArtInfoBuilding* CvGame::getBuildingArtInfo(BuildingTypes eBuilding) const
{
	return GC.getBuildingInfo(eBuilding).getArtInfo();
}

bool CvGame::isWaterBuilding(BuildingTypes eBuilding) const
{
	return GC.getBuildingInfo(eBuilding).isWater();
}

CivilopediaWidgetShowTypes CvGame::getWidgetShow(BonusTypes eBonus) const
{
	const CvBonusInfo& bonus = GC.getBonusInfo(eBonus);

	// Don't show the widget if it's an abstract bonus that doesn't belong on the map.
	//	I.e. Manufactured bonuses, cultures and such.
	if (bonus.getPlacementOrder() < 0)
	{
		return CIVILOPEDIA_WIDGET_SHOW_NONE;
	}

	for (int i = 0; i < GC.getNumTerrainInfos(); i++)
	{
		if (GC.getTerrainInfo((TerrainTypes) i).isWaterTerrain() && bonus.isTerrain(i))
		{
			return CIVILOPEDIA_WIDGET_SHOW_WATER;
		}
	}
	return CIVILOPEDIA_WIDGET_SHOW_LAND;
}

CivilopediaWidgetShowTypes CvGame::getWidgetShow(ImprovementTypes eImprovement) const
{
	if (GC.getImprovementInfo(eImprovement).isWaterImprovement())
	{
		return CIVILOPEDIA_WIDGET_SHOW_WATER;
	}
	return CIVILOPEDIA_WIDGET_SHOW_LAND;
}

VictoryTypes CvGame::getSpaceVictory() const
{
	VictoryTypes eVictory = NO_VICTORY;
	for (int i=0; i < GC.getNumProjectInfos(); i++)
	{
		const ProjectTypes eProject = (ProjectTypes) i;
		if (GC.getProjectInfo(eProject).isSpaceship())
		{
			eVictory = (VictoryTypes) GC.getProjectInfo(eProject).getVictoryPrereq();
			break;
		}
	}

	FAssertMsg(eVictory != NO_VICTORY, "Invalid space victory type.");
	return eVictory;
}

void CvGame::nextActivePlayer(bool bForward)
{
	int iNewPlayer = getActivePlayer();
	for (int i = 1; i < MAX_PC_PLAYERS; ++i)
	{
		if (bForward)
			iNewPlayer += 1;
		else iNewPlayer += MAX_PC_PLAYERS - 1;

		iNewPlayer %= MAX_PC_PLAYERS;

		const PlayerTypes eNewPlayer = (PlayerTypes) iNewPlayer;

		if (GET_PLAYER(eNewPlayer).isAlive())
		{
			GC.getInitCore().setSlotStatus(getActivePlayer(), SS_COMPUTER);
			GC.getInitCore().setSlotStatus(eNewPlayer, SS_TAKEN);
			GET_PLAYER(getActivePlayer()).setTurnActive(false, false);
			GET_PLAYER(eNewPlayer).setTurnActive(true, false);
			setActivePlayer(eNewPlayer, true);
			break;
		}
	}
}


int CvGame::getNextSoundtrack(EraTypes eLastEra, int iLastSoundtrack) const
{
	const EraTypes eCurEra = GET_PLAYER(getActivePlayer()).getCurrentEra();
	const CvEraInfo& kCurrentEra = GC.getEraInfo(eCurEra);

	if (kCurrentEra.getNumSoundtracks() == 0)
	{
		return -1;
	}
	if (kCurrentEra.getNumSoundtracks() == 1 || (eLastEra != eCurEra && kCurrentEra.isFirstSoundtrackFirst()))
	{
		return kCurrentEra.getSoundtracks(0);
	}
	return kCurrentEra.getSoundtracks(GC.getASyncRand().get(kCurrentEra.getNumSoundtracks(), "Pick Song ASYNC"));
}

int CvGame::getSoundtrackSpace() const
{
	return std::max(1, GC.getEraInfo(GET_PLAYER(getActivePlayer()).getCurrentEra()).getSoundtrackSpace());
}

bool CvGame::isSoundtrackOverride(CvString& strSoundtrack) const
{
	return false;
}

void CvGame::initSelection() const
{
	CvUnit* pSelectionUnit = NULL;

	foreach_(CvUnit* pLoopUnit, GET_PLAYER(getActivePlayer()).units())
	{
		if (pLoopUnit->getGroup()->readyToSelect() && pLoopUnit->canFight())
		{
			pSelectionUnit = pLoopUnit;
			break;
		}
	}

	if (!pSelectionUnit)
	{
		foreach_(CvUnit* pLoopUnit, GET_PLAYER(getActivePlayer()).units())
		{
			if (pLoopUnit->getGroup()->readyToSelect())
			{
				pSelectionUnit = pLoopUnit;
				break;
			}
		}
	}
	if (!pSelectionUnit) // Last resort
	{
		int iLoop;
		pSelectionUnit = GET_PLAYER(getActivePlayer()).firstUnit(&iLoop);
	}

	if (pSelectionUnit)
	{
		GC.getCurrentViewport()->bringIntoView(pSelectionUnit->getX(), pSelectionUnit->getY(), pSelectionUnit);
	}
}

bool CvGame::canDoPing(CvPlot* pPlot, PlayerTypes ePlayer) const
{
	return pPlot && pPlot->isRevealed(getActiveTeam(), false) && GET_PLAYER(ePlayer).getTeam() == getActiveTeam();
}

bool CvGame::shouldDisplayReturn() const
{
	return gDLL->getInterfaceIFace()->isCitySelection();
}

bool CvGame::shouldDisplayEndTurn() const
{
	return !gDLL->getInterfaceIFace()->isCitySelection() && GET_PLAYER(getActivePlayer()).isTurnActive();
}

bool CvGame::shouldDisplayWaitingOthers() const
{
	if (!gDLL->getInterfaceIFace()->isCitySelection())
	{
		if (!GET_PLAYER(getActivePlayer()).isTurnActive())
		{
			return true;
		}
		if (gDLL->getInterfaceIFace()->isInAdvancedStart() && GET_PLAYER(getActivePlayer()).getAdvancedStartPoints() < 0)
		{
			return true;
		}
	}
	return false;
}

bool CvGame::shouldDisplayWaitingYou() const
{
	if (isNetworkMultiPlayer() && shouldDisplayEndTurn())
	{
		if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS) && 1 == countNumHumanGameTurnActive())
		{
			return true;
		}
		if (isSimultaneousTeamTurns() && 1 == GET_TEAM(getActiveTeam()).countNumHumanGameTurnActive() && GET_TEAM(getActiveTeam()).getAliveCount() > 1)
		{
			return true;
		}
	}
	return false;
}

bool CvGame::shouldDisplayEndTurnButton() const
{
	return shouldDisplayEndTurn() && !gDLL->GetWorldBuilderMode();
}

bool CvGame::shouldDisplayFlag() const
{
	if(gDLL->getInterfaceIFace()->isCitySelection()
	|| gDLL->getInterfaceIFace()->getHeadSelectedCity()
	|| gDLL->isDiplomacy()
	|| gDLL->isMPDiplomacyScreenUp()
	|| gDLL->GetWorldBuilderMode())
	{
		return false;
	}
	return true;
}

bool CvGame::shouldDisplayUnitModel() const
{
	if (gDLL->isDiplomacy() || gDLL->isMPDiplomacyScreenUp() || gDLL->GetWorldBuilderMode())
	{
		return false;
	}
	if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() || gDLL->getInterfaceIFace()->isCityScreenUp())
	{
		return true;
	}
	return false;
}

bool CvGame::shouldShowResearchButtons() const
{
	return !gDLL->GetWorldBuilderMode()
		&& GET_PLAYER(getActivePlayer()).isAlive()
		&& !gDLL->getInterfaceIFace()->isCityScreenUp()
		&& GET_PLAYER(getActivePlayer()).getCurrentResearch() == NO_TECH;
}

bool CvGame::shouldCenterMinimap() const
{
	return isDebugMode() || GET_TEAM(getActiveTeam()).isMapCentering();
}

EndTurnButtonStates CvGame::getEndTurnState() const
{
	if (isNetworkMultiPlayer())
	{
		const CvTeamAI& team = GET_TEAM(getActiveTeam());
		if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS) && 1 == countNumHumanGameTurnActive()
		|| !isSimultaneousTeamTurns() && 1 == team.countNumHumanGameTurnActive() && team.getAliveCount() > 1)
		{
			return END_TURN_OVER_HIGHLIGHT;
		}
	}
	return END_TURN_GO;
}

void CvGame::handleCityScreenPlotPicked(CvCity* pCity, CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const
{
	FAssert(pPlot);
	if (pCity && pPlot)
	{
		int iIndex = pCity->getCityPlotIndex(pPlot);
		if (pPlot->getOwner() == getActivePlayer() && pCity->getOwner() == getActivePlayer() && iIndex != -1)
		{
			CvMessageControl::getInstance().sendDoTask(pCity->getID(), TASK_CHANGE_WORKING_PLOT, iIndex, -1, false, bAlt, bShift, bCtrl);

			if (!pCity->isWorkingPlot(iIndex))
			{
				pPlot->setImprovementUpgradeCache(-1);
			}
		}
		else if (GC.getDefineINT("CITY_SCREEN_CLICK_WILL_EXIT"))
		{
			gDLL->getInterfaceIFace()->clearSelectedCities();
		}
	}
}

void CvGame::handleCityScreenPlotDoublePicked(CvCity* pCity, CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const
{
	if (pCity && pCity->plot() == pPlot)
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();
	}
}

void CvGame::handleCityScreenPlotRightPicked(CvCity* pCity, CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const
{
	if (pCity && pPlot
	&& pCity->getOwner() == getActivePlayer() && pPlot->getOwner() == getActivePlayer() && pCity->getCityPlotIndex(pPlot) != -1)
	{
		CvMessageControl::getInstance().sendDoTask(pCity->getID(), TASK_CLEAR_WORKING_OVERRIDE, pCity->getCityPlotIndex(pPlot), -1, false, bAlt, bShift, bCtrl);
	}
}

void CvGame::handleCityPlotRightPicked(CvCity* pCity, CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const
{
	if (pPlot)
	{
		if (pCity && gDLL->getInterfaceIFace()->isCitySelected(pCity))
		{
			selectedCitiesGameNetMessage(GAMEMESSAGE_DO_TASK, TASK_CLEAR_RALLY_PLOT);
		}
		else if (bShift)
		{
			selectedCitiesGameNetMessage(GAMEMESSAGE_DO_TASK, TASK_RALLY_PLOT, pPlot->getX(), pPlot->getY());
		}
		else gDLL->getInterfaceIFace()->clearSelectedCities();
	}
}

void CvGame::handleMiddleMouse(bool bCtrl, bool bAlt, bool bShift)
{
	if (gDLL->getInterfaceIFace()->isCitySelection())
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();
	}
	else if (bAlt)
	{
		doControl(CONTROL_SELECTYUNITALL);
	}
	else if (bCtrl)
	{
		doControl(CONTROL_SELECTYUNITTYPE);
	}
	else
	{
		doControl(CONTROL_CENTERONSELECTION);
	}
}

void CvGame::handleDiplomacySetAIComment(DiploCommentTypes eComment) const
{
	const PlayerTypes eOtherPlayer = (PlayerTypes) gDLL->getDiplomacyPlayer();
	FAssert(eOtherPlayer != NO_PLAYER);
	if (GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACCEPT_ASK") == eComment
	|| GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACCEPT_DEMAND") == eComment)
	{
		if (!GET_TEAM(getActiveTeam()).isAVassal() && !GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).isAVassal())
		{
			CLinkList<TradeData> playerList;
			CLinkList<TradeData> loopPlayerList;
			TradeData kTradeData;
			setTradeItem(&kTradeData, TRADE_PEACE_TREATY);
			playerList.insertAtEnd(kTradeData);
			loopPlayerList.insertAtEnd(kTradeData);

			gDLL->sendImplementDealMessage(eOtherPlayer, &playerList, &loopPlayerList);
		}
	}
}
