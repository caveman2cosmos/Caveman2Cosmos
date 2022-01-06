#pragma once

// Examlple:
//
// INSTALLATION OF MODES
//
// this file:
//	enum Mode { NoVicinityBonusDebug, DisappearingUnitsDebug };
//
// SCyDebug.cpp:
//	void SCyDebug::installInPython()
//	{
//		...
//		python::enum_<SCvDebug::Mode>("SCyDebug_Mode")
//		.value("ShrimpsDebug", SCvDebug::NoVicinityBonusDebug)
//		.value("ShrimpsDebug", SCvDebug::DisappearingUnitsDebug)
//		.export_values();
//	}
//
// USAGE
//
// CvCity.cpp:
//	CvCity::hasVicinityBonus(...)
//	{
//		...
//		std::ostringstream msg
//			<< pLoopPlot->isHasValidBonus() << "\n"
//			<< pLoopPlot->isConnectedTo(this);
//
//		if (SCvDebug::checkMode(NoVicinityBonusDebug)
//			gDLL->MessageBox(msg, "");
//		...
//	}
//
// in-game python console:
// >> p = gc.getActivePlayer()
// >> c = p.nthCity(1)
// >> shrimpsId = 70
// >> SCyDebug().setMode(SCyDebug_Mode.NoVicinityBonus, true)
// >> c.hasVicinityBonus(shrimpsId)
// (The massege box appears)
// >> SCyDebug().setMode(SCyDebug_Mode.NoVicinityBonus, false)
//

class SCvDebug
{
public:
	// REMARKS
	// - Number of Mode elements <= 64
	// - Remember to remove the mode after fixing a bug
	enum Mode {};

	static bool checkMode(Mode mode);
	static void setMode(Mode mode, bool on);

private:
	static uint64_t m_mode;
};
