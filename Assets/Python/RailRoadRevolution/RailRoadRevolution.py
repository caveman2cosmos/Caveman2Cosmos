## Native Rail Road Revolution module: UI helpers only, no per-turn Python.
from CvPythonExtensions import CyGlobalContext, GameOptionTypes

def isEnabled():
    return CyGlobalContext().getGame().isOption(GameOptionTypes.GAMEOPTION_MAP_RAIL_ROAD_REVOLUTION)

def getLayers(plot):
    return (plot.getRoadRouteType(), plot.getRailRouteType())
