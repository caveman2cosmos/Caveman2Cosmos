#pragma once

#ifndef PLOT_SNAPSHOT_H
#define PLOT_SNAPSHOT_H

// Emit a CSV snapshot of every plot's static state to
//   Documents/My Games/Beyond The Sword/Logs/PlotSnapshot_<tag>_t<turn>.csv
//
// Intended companion to BuildEvaluation.log: the worker AI's [WAI/*] log lines
// reference plots by (x,y) and plotIdx; this file lets an analyst join those
// log lines against the full plot state at that point in the game.
//
// Schema and call-site conventions are documented in Sources/docs/PlotSnapshot.md.
//
// Called from CvGame at:
//   - onFinalInitialized(bNewGame=true)  tag="start"
//   - onFinalInitialized(bNewGame=false) tag="load"
//   - end of regenerateMap                tag="regen"
//   - top of doTurn                       tag="turn"  (one file per game turn)
void writePlotSnapshot(const char* tag);

#endif // PLOT_SNAPSHOT_H
