#pragma once

class CvPlot;

// Optional native module. Roads keep native border/domain legality, while their
// speed is public. Rail speed is team-only. Freight gates belong to routeRev.
namespace RailRoadRevolution
{
    bool enabled();
    RouteTypes bestRoad(TeamTypes team);
    TechTypes effectiveRoadObsoleteTech(RouteTypes route, TechTypes authoredTech);
    void resetCaches();
    void migrateLegacyRoutes();
    const std::vector<BuildTypes>& routeBuilds();
    bool usefulRailPlot(const CvPlot& plot, PlayerTypes owner);
    void logTurn();
}
