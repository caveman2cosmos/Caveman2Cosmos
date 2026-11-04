#pragma once
class CvPlot;
class CvUnit;
namespace UnitTravelConnectivity {
    enum { LAND=1, SEA=2 };
    void invalidate();
    void reset();
    bool relevant(const CvPlot* plot);
    bool relevantImprovement(int type);
    bool relevantRoute(int type);
    void invalidatePlot(const CvPlot* plot,int newKinds,bool accessOnly);
    bool landUnitCanReach(const CvUnit* unit,const CvPlot* target,bool potential=false);
    bool seaUnitCanReach(const CvUnit* unit,const CvPlot* target,bool potential=false);
    bool seaUnitCanReachPickup(const CvUnit* unit,const CvPlot* target);
    // Potential mode is an optimistic physical filter for attack/declare-war
    // planning. It never grants passage: native unit path legality follows.
    bool land(int player,const CvPlot* from,const CvPlot* to,bool potential=false);
    bool sea(int player,const CvPlot* from,const CvPlot* to,bool potential=false);
    bool landOrAdjacent(int player,const CvPlot* from,const CvPlot* to,bool potential=false);
    struct Change {
        bool active;
        explicit Change(bool affects=true):active(affects){if(active)invalidate();}
        ~Change(){if(active)invalidate();}
    private:Change(const Change&);Change&operator=(const Change&);
    };
    struct PlotChange {
        const CvPlot* plot;int newKinds;bool accessOnly,active;
        PlotChange(const CvPlot* p,int kinds=0,bool access=false,bool enabled=true)
            :plot(p),newKinds(kinds),accessOnly(access),active(enabled){if(active)invalidatePlot(plot,newKinds,accessOnly);}
        ~PlotChange(){if(active)invalidatePlot(plot,newKinds,accessOnly);}
    private:PlotChange(const PlotChange&);PlotChange&operator=(const PlotChange&);
    };
}
