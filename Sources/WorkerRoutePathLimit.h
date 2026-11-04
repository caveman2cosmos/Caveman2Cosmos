#pragma once

// AI_routeTerritory scores floor(10000/(turns+1))*priorityPercent/100.
// The bound preserves strict > and traversal-order ties, including both
// integer roundings. Beyond this turn count a candidate cannot win.
inline int workerRoutePathLimit(int priorityPercent, int bestValue)
{
 if(priorityPercent<=0)return -1;
 const __int64 needed=(((__int64)bestValue+1)*100+priorityPercent-1)/priorityPercent;
 if(needed>10000)return -1;
 return 10000/(int)needed-1;
}
