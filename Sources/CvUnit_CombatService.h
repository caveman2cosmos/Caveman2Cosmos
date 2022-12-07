#pragma once

#ifndef CV_UNIT_COMBAT_SERVICE_H
#define CV_UNIT_COMBAT_SERVICE_H

class CvUnit;

///
/// Internal components of CvUnit (which should rather not be used outside it, unless you can't find another solution)
/// 
namespace CvUnitNS
{
	class CombatService
	{
		CvUnit* unit;

		bool bQuick; 
		CvUnit* pSelectedDefender; 
		bool bSamePlot;
		bool bStealth;
		bool bNoCache;

		bool bFinish;
		CvPlot* pPlot;
		CvUnit* pDefender;
		const bool bHuman;           // const after initialization
		const bool bHumanDefender;   // const after initialization
		const bool bVisible;         // const after initialization
		const PlayerTypes eAttacker; // const after initialization
		const PlayerTypes eDefender; // const after initialization
		const bool bStealthDefense;  // const after initialization

	public:
		CombatService(CvUnit* unit, bool bQuick, CvUnit* pSelectedDefender, bool bSamePlot, bool bStealth, 
			bool bNoCache);

		void updateCombat();
	//	void updateCombat2();

	//private:
	//	void init();
	//	void fight(bool& abort);
	//	void finishingPrelude();
	//	void onWin();
	//	void onLoss();
	//	void onAttackerWithraw();
	//	void onDefenderWithraw();
	//	void onAttackerRepelled();
	//	void onDefenderKnockedBack();
	//	void onOtherResult();
	};
}

#endif