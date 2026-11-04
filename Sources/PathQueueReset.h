#pragma once

// A node-pool reset invalidates the meaning of ALL queued node pointers.
// Clear before reusing pool storage. Assignment discards entries without
// comparing/dereferencing stale nodes, unlike repeatedly popping the heap.
// The pathfinder's comparator is stateless; this also retains vector capacity.
template<class Queue> inline void resetPathQueue(Queue& queue)
{
    if(!queue.empty()) queue=Queue();
}
