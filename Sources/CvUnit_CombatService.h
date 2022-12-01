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
		CvUnit* m_unit;

		bool m_bQuick; 
		CvUnit* m_selectedDefender; 
		bool m_bSamePlot;
		bool m_bStealth;
		bool m_bNoCache;

		bool m_bFinish;
		CvPlot* m_plot;
		CvUnit* m_defender;
		const bool m_bHuman;           // const after initialization
		const bool m_bHumanDefender;   // const after initialization
		const bool m_bVisible;         // const after initialization
		const PlayerTypes m_eAttacker; // const after initialization
		const PlayerTypes m_eDefender; // const after initialization
		const bool m_bStealthDefense;  // const after initialization

	public:
		CombatService(CvUnit* unit, bool bQuick, CvUnit* pSelectedDefender, bool bSamePlot, bool bStealth, 
			bool bNoCache);

		void updateCombat();

	private:
		void init();
		void fight(bool& abort);
		void finishingPrelude();
		void onWin();
		void onLoss();
		void onAttackerWithraw();
		void onDefenderWithraw();
		void onAttackerRepelled();
		void onDefenderKnockedBack();
		void onOtherResult();
	};
}

#endif