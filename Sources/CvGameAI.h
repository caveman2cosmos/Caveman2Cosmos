#pragma once

// gameAI.h

#ifndef CIV4_GAME_AI_H
#define CIV4_GAME_AI_H

#include "CvGame.h"

class CvGameAI : public CvGame
{

public:

  CvGameAI();
  virtual ~CvGameAI();

  /**
   * Initializes the AI game state.
   * Steps:
   *   1. Calls AI_reset to clear previous state.
   *   2. Initializes other game data as needed.
   */
  void AI_init();

  /**
   * Uninitializes the AI game state.
   * Steps:
   *   1. Clears or resets any AI-specific data.
   */
  void AI_uninit();

  /**
   * Resets the AI game state.
   * Steps:
   *   1. Calls AI_uninit to clear previous state.
   *   2. Resets internal variables (e.g., m_iPad).
   *   3. Clears unit AI caches.
   */
	void AI_reset();

  /**
   * Marks all players as needing to assign work.
   * Steps:
   *   1. Loops through all players.
   *   2. If the player is alive, calls AI_makeAssignWorkDirty for that player.
   */
	void AI_makeAssignWorkDirty();

  /**
   * Updates work assignment for all human players.
   * Steps:
   *   1. Loops through all players.
   *   2. If the player is human and alive, calls AI_updateAssignWork for that player.
   */
	void AI_updateAssignWork();

  /**
   * Returns the combat value of a unit type for AI evaluation.
   * Steps:
   *   1. Gets base value (100).
   *   2. Multiplies by air or land combat strength.
   *   3. Applies modifiers for first strikes and other attributes.
   *   4. Normalizes by best land unit combat value.
   *   5. Returns final value.
   */
	int AI_combatValue(const UnitTypes eUnit) const;

  /**
   * Reads AI game data from a stream.
   * Steps:
   *   1. Calls CvGame::read.
   *   2. Reads m_iPad value.
   */
	virtual void read(FDataStreamBase* pStream);

  /**
   * Writes AI game data to a stream.
   * Steps:
   *   1. Calls CvGame::write.
   *   2. Writes m_iPad value.
   */
	virtual void write(FDataStreamBase* pStream);

protected:

  int m_iPad;

};

#endif
