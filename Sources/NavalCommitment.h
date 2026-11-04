#pragma once

// An accepted task, not a cached path or danger result. All fields are saved as
// tagged integers; old saves start with NONE. Tactical safety is checked live.
namespace NavalCommitment {
enum Kind { NONE, REPAIR, REFUGE, PICKUP, DELIVERY, BOARD };
enum Field { KIND, X, Y, MAP, ROLE, GROUP, MEMBERS, CARGO, FLAGS,
             LAST_PLOT, PROGRESS_TURN, TARGET_OWNER, TARGET_UNIT, MISSION, TARGET_ROLE, COUNT };
inline bool compatible(const int* state, int map, int role, int group, int members, int cargo)
{
    return state[KIND] > NONE && state[KIND] <= BOARD
        && state[MAP] == map && state[ROLE] == role && state[GROUP] == group
        && state[MEMBERS] == members && state[CARGO] == cargo;
}
// A temporary obstruction may recover next turn. Never retry forever, or spin
// through the unit-update loop in the same turn when a move cannot execute.
inline bool stalled(int* state, int position, int turn)
{
    if (state[LAST_PLOT] != position) {
        state[LAST_PLOT] = position;
        state[PROGRESS_TURN] = turn;
    }
    return turn - state[PROGRESS_TURN] > 2;
}
}
