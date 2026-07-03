// PlotSnapshot.cpp -- see Sources/docs/PlotSnapshot.md for schema and conventions.
//
// Emits one CSV file per call:
//   PlotSnapshot_<tag>_t<turn>.csv
// in the BTS Logs directory.
//
// We write the file directly via fopen/fprintf/fclose rather than gDLL->logMsg.
// gDLL keeps log file handles open for the lifetime of the process (so the same
// filename can be appended cheaply), but that means remove() against a previous
// snapshot fails with "file in use by another process" -- the engine still
// holds the handle. By owning the open/close ourselves, the file is closed
// before we return and the rotation logic below can actually delete old files.
// As a bonus, one fopen replaces ~9600 logMsg calls per turn.
//
// To prevent flooding the disk, we keep only the last 3 "turn" snapshots --
// every time we write turn N, we delete PlotSnapshot_turn_t(N-3).csv. The
// "start" / "load" / "regen" snapshots are never auto-deleted; they're one-offs.
// We resolve the path via %USERPROFILE%\Documents rather than the
// SHGetFolderPath shell API to avoid pulling in shlobj.h (which clashes with
// C2C's CATEGORY_INFO macro). This works for default Windows setups; if
// Documents is redirected (e.g. via OneDrive) writes will fail entirely.

#include "CvGameCoreDLL.h"
#include "Utils/PlotSnapshot.h"

#include "CvArea.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvUnit.h"

#include <cstdio>   // remove()
#include <cstdlib>  // getenv()
#include <cstring>  // strcmp()
#include <io.h>     // _findfirst, _findnext, _findclose

namespace {

// Resolve a TerrainTypes/FeatureTypes/ImprovementTypes/RouteTypes/BonusTypes
// to its XML type-string, or "NONE" when the value is the canonical sentinel.
// Keeping these inline so the call sites stay readable.

const char* terrainName(TerrainTypes e)
{
	return (e == NO_TERRAIN) ? "NONE" : GC.getTerrainInfo(e).getType();
}
const char* featureName(FeatureTypes e)
{
	return (e == NO_FEATURE) ? "NONE" : GC.getFeatureInfo(e).getType();
}
const char* improvementName(ImprovementTypes e)
{
	return (e == NO_IMPROVEMENT) ? "NONE" : GC.getImprovementInfo(e).getType();
}
const char* routeName(RouteTypes e)
{
	return (e == NO_ROUTE) ? "NONE" : GC.getRouteInfo(e).getType();
}
const char* bonusName(BonusTypes e)
{
	return (e == NO_BONUS) ? "NONE" : GC.getBonusInfo(e).getType();
}

// Build the absolute path to a file in the BTS Logs directory. Returns false
// if %USERPROFILE% isn't set (essentially never on Windows). Assumes the
// default Documents-under-USERPROFILE layout; OneDrive-redirected Documents
// are NOT handled (the delete will silently fail in that case).
bool buildLogsAbsolutePath(const char* filename, char* out, size_t outSize)
{
	const char* userProfile = std::getenv("USERPROFILE");
	if (userProfile == NULL)
	{
		return false;
	}
	const int written = _snprintf(out, outSize - 1,
		"%s\\Documents\\My Games\\Beyond The Sword\\Logs\\%s", userProfile, filename);
	if (written < 0 || (size_t)written >= outSize - 1)
	{
		return false;
	}
	out[outSize - 1] = '\0';
	return true;
}

// Remove PlotSnapshot_turn_t<turn>.csv if it exists. Silent on failure --
// missing files are normal at game start and on the first few turns.
void deleteTurnSnapshot(int turn)
{
	char filename[64];
	_snprintf(filename, sizeof(filename) - 1, "PlotSnapshot_turn_t%d.csv", turn);
	filename[sizeof(filename) - 1] = '\0';

	char fullPath[512];
	if (buildLogsAbsolutePath(filename, fullPath, sizeof(fullPath)))
	{
		// remove() returns non-zero if the file doesn't exist; we don't care.
		std::remove(fullPath);
	}
}

// Delete every PlotSnapshot_*.csv in the Logs directory except (optionally)
// one. Called from writePlotSnapshot when a non-"turn" tag fires -- i.e. at
// game start / save load / map regen -- to wipe leaked files from previous
// sessions. Uses _findfirst/_findnext which is in <io.h>; no shell32 needed.
void deleteAllPlotSnapshots(const char* exceptFilename)
{
	char searchPath[512];
	if (!buildLogsAbsolutePath("PlotSnapshot_*.csv", searchPath, sizeof(searchPath)))
	{
		return;
	}

	struct _finddata_t fd;
	const intptr_t hSearch = _findfirst(searchPath, &fd);
	if (hSearch == -1)
	{
		// No matching files (or directory missing). Nothing to clean up.
		return;
	}

	do
	{
		// fd.name is just the filename (no directory). Skip the file we want
		// to preserve (the snapshot we just wrote).
		if (exceptFilename != NULL && std::strcmp(fd.name, exceptFilename) == 0)
		{
			continue;
		}

		char fullPath[512];
		if (buildLogsAbsolutePath(fd.name, fullPath, sizeof(fullPath)))
		{
			std::remove(fullPath);
		}
	} while (_findnext(hSearch, &fd) == 0);

	_findclose(hSearch);
}

// Copy a city name into a fixed-size buffer, replacing characters that would
// confuse a CSV reader (commas, quotes, newlines) with underscores. Output is
// never quoted -- we sanitise instead so the row is unambiguous to split on `,`.
void copyCityNameSanitized(const CvCity* city, char* out, size_t outSize)
{
	if (city == NULL || outSize == 0)
	{
		if (outSize > 0) out[0] = '\0';
		return;
	}
	// CvCity::getName() returns a CvWString. Narrow to ASCII; non-ASCII chars
	// become '?' which is fine for the diagnostic purpose of this file.
	const CvWString wName = city->getName();
	const int wLen = (int)wName.length();
	size_t o = 0;
	for (int i = 0; i < wLen && o + 1 < outSize; ++i)
	{
		wchar_t wc = wName[i];
		char c;
		if (wc < 0x20 || wc > 0x7E) c = '?';
		else if (wc == ',' || wc == '"' || wc == '\n' || wc == '\r') c = '_';
		else c = (char)wc;
		out[o++] = c;
	}
	out[o] = '\0';
}

// Build a `|`-separated list of the animals standing on a plot, one token per
// animal unit:  <UnitType>@o<owner>c<combat>a<aggression>e<enemyOfActiveTeam>
//   o = owner player id
//   c = base combat strength from XML (iCombat; in-game "strength" is c/100)
//   a = iAggression (0 = passive prey -- the case hunters tend to ignore)
//   e = 1 if the unit is at war with the active player's team, 0 if not, -1 if
//       there is no active team. This is the field that explains why a hunter
//       does/doesn't recognise the animal as a target: AI_huntRange only treats
//       at-war (or huntable-target) units as candidates.
// Output is CSV-safe (commas/newlines replaced); empty when no animals present.
void appendAnimalsField(const CvPlot* pPlot, char* out, size_t outSize)
{
	if (outSize == 0) return;
	out[0] = '\0';
	size_t o = 0;
	const TeamTypes eActiveTeam = GC.getGame().getActiveTeam();
	bool bFirst = true;

	foreach_(const CvUnit* pUnit, pPlot->units())
	{
		if (pUnit == NULL || !pUnit->isAnimal())
		{
			continue;
		}
		const bool bKnownType = (pUnit->getUnitType() != NO_UNIT);
		const char* szType = bKnownType ? pUnit->getUnitInfo().getType() : "UNKNOWN";
		const int iCombat   = bKnownType ? pUnit->getUnitInfo().getCombat() : 0;
		const int iAggr     = bKnownType ? pUnit->getUnitInfo().getAggression() : 0;
		const int iEnemy    = (eActiveTeam == NO_TEAM) ? -1 : (pUnit->isEnemy(eActiveTeam) ? 1 : 0);

		char buf[96];
		_snprintf(buf, sizeof(buf) - 1, "%s%s@o%dc%da%de%d",
			bFirst ? "" : "|", szType, (int)pUnit->getOwner(), iCombat, iAggr, iEnemy);
		buf[sizeof(buf) - 1] = '\0';
		for (char* p = buf; *p != '\0'; ++p)
		{
			if (*p == ',' || *p == '\n' || *p == '\r') *p = '_';
		}

		const size_t len = std::strlen(buf);
		if (o + len + 1 >= outSize)
		{
			break; // truncate rather than overflow
		}
		std::strcpy(out + o, buf);
		o += len;
		bFirst = false;
	}
}

} // namespace

void writePlotSnapshot(const char* tag)
{
	if (tag == NULL) tag = "unknown";

	const CvMap& kMap = GC.getMap();
	const int iNumPlots = kMap.numPlots();
	const int iGridW    = kMap.getGridWidth();
	const int iGridH    = kMap.getGridHeight();
	const int iTurn     = GC.getGame().getGameTurn();

	// Build the per-snapshot filename and the full path.
	char filename[128];
	_snprintf(filename, sizeof(filename) - 1,
		"PlotSnapshot_%s_t%d.csv", tag, iTurn);
	filename[sizeof(filename) - 1] = '\0';

	char fullPath[512];
	if (!buildLogsAbsolutePath(filename, fullPath, sizeof(fullPath)))
	{
		// USERPROFILE not set or path overflow. Bail silently -- this is a
		// diagnostic file, not load-bearing.
		return;
	}

	// Open in "w" mode (truncating): we always write a fresh file per snapshot
	// call. The file is closed explicitly at the end of this function so the
	// rotation step below can delete previous snapshots without "file in use"
	// errors.
	FILE* fp = std::fopen(fullPath, "w");
	if (fp == NULL)
	{
		return;
	}

	// Header: a `#`-prefixed comment line carrying the schema version and
	// capture metadata, then the column-name row. CSV parsers that don't
	// know about `#` comments can be told to skip the first line.
	std::fprintf(fp,
		"# PlotSnapshot schema=2 tag=%s turn=%d mapW=%d mapH=%d numPlots=%d\n",
		tag, iTurn, iGridW, iGridH, iNumPlots);

	std::fprintf(fp,
    		"plotIdx,x,y,terrain,feature,improvement,route,bonus,"
    		"isWater,isHills,isPeak,isCity,isCityRadius,owner,"
    		"workingCityId,workingCityName,area,improvementCurrentValue,"
    		"numUnits,animals\n");

	// One row per plot. plotIdx is the loop counter; it matches the iI used
	// as the key in CvWorkerAI's bonus-evaluation cache and the position
	// returned by GC.getMap().plotByIndex(i).
	for (int i = 0; i < iNumPlots; ++i)
	{
		const CvPlot* pPlot = kMap.plotByIndex(i);
		if (pPlot == NULL) continue; // defensive; shouldn't happen for i < numPlots

		const CvCity* pWorkingCity = pPlot->getWorkingCity();
		char cityName[64];
		copyCityNameSanitized(pWorkingCity, cityName, sizeof(cityName));

		const CvArea* pArea = pPlot->area();
		const int iAreaId = (pArea != NULL) ? pArea->getID() : -1;

		// Read-only access to improvementCurrentValue: log whatever the field
		// currently holds (may be 0 for plots that predate the lazy-init).
		// Deliberately NOT calling setImprovementCurrentValue() here -- the
		// planner's hot path handles that and we don't want this diagnostic
		// dump to mutate game state.
		const int iCurrentValue = pPlot->getImprovementCurrentValue();

		char animals[256];
        appendAnimalsField(pPlot, animals, sizeof(animals));

		std::fprintf(fp,
			"%d,%d,%d,%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%s,%d,%d,%d,%s\n",
			i,
			pPlot->getX(), pPlot->getY(),
			terrainName(pPlot->getTerrainType()),
			featureName(pPlot->getFeatureType()),
			improvementName(pPlot->getImprovementType()),
			routeName(pPlot->getRouteType()),
			bonusName(pPlot->getBonusType()),
			pPlot->isWater() ? 1 : 0,
			pPlot->isHills() ? 1 : 0,
			pPlot->isAsPeak() ? 1 : 0,
			pPlot->isCity() ? 1 : 0,
			pPlot->isCityRadius() ? 1 : 0,
			(int)pPlot->getOwner(),
			(pWorkingCity != NULL) ? pWorkingCity->getID() : -1,
			cityName,
			iAreaId,
			iCurrentValue,
            pPlot->getNumUnits(),
            animals);
	}

	std::fclose(fp);

	// Cleanup after fclose so the file we just wrote is on disk and any old
	// file we might delete is no longer the same file we just wrote.
	if (std::strcmp(tag, "turn") == 0)
	{
		// Mid-game rotation: keep the last 3 turn snapshots, delete the one
		// that just rolled out of the window.
		if (iTurn >= 3)
		{
			deleteTurnSnapshot(iTurn - 3);
		}
	}
	else
	{
		// Game start / save load / map regen: wipe every other PlotSnapshot_*.csv
		// in the Logs dir. This handles leaked files from a previous session
		// (old turn snapshots that didn't get rotated because the game crashed
		// or was killed) and old start/load/regen one-offs the user didn't
		// hand-clean. The just-written file is preserved by passing its name
		// as the exception.
		deleteAllPlotSnapshots(filename);
	}
}
