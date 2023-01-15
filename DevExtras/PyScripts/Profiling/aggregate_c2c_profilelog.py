
### PARAMETERS ###

output        = "IFP_aggr_log.txt"
ignoreParents = True    # if False, records are aggregated per the (parent, function) pair
                        # if True, records are aggregated only per function
sortby        = "time"  # group name - cf. recpat below except timestamp

##################



### PREAMBULA ###

import os, sys

try: # setting current directory when running in VS interactive mode
    curdir = os.path.dirname(os.popen("VS.CurrentDocPath " + os.getenv("_PTVS_PID")).read())
    os.chdir(curdir)
except: pass

sys.path.append("..")

#################



### SCRIPT ###

import re
import settings

recpat = re.compile(r"\[(?P<timestamp>.+)\] "
                    r"(?P<fn>[^\t]*)\t"
                    r"(?P<time>[^\t]*)\t"
                    r"(?P<mthtime>[^\t]*)\t"  # main thread time
                    r"(?P<avgtime>[^\t]*)\t"  # average
                    r"(?P<calls>[^\t]*)\t"
                    r"(?P<chtime>[^\t]*)\t"   # child time
                    r"(?P<sftime>[^\t]*)\t"   # self time
                    r"(?P<parent>[^\t]*)\t"
                    r"(?P<alttime>[^\t]*)\n") # alternate time



# reading the profiler log

aggregrecs = {}

with open(settings.civ4BtsProfileDir + "\\Logs\\" + settings.c2cProfilerLog, 'r') as log:

    for i, ln in enumerate(log.readlines()):
        if ln.strip() == "": continue

        mres = recpat.fullmatch(ln)
        if not mres: raise ValueError("Invalid line", i, ln)
        if mres.group("fn") in ["Fn", "Root"]: continue

        recdict = mres.groupdict()
        recdict.pop("timestamp")
        fn      = recdict.pop("fn")
        parent  = recdict.pop("parent")
        key     = fn if ignoreParents else (fn, parent)
        stats   = aggregrecs.setdefault(key, { "fn" : fn, "parent" : parent })
        try:
            for group, val in recdict.items():
                stats[group] = stats.setdefault(group, 0) + int(val)
        except ValueError as e:
            raise ValueError("Invalid value", i, ln)


recs = list(aggregrecs.values())
if sortby:
    recs.sort(key = lambda rec: rec[sortby], reverse=True)



# writing the result

outcolumns = ["parent", "fn", "time", "mthtime", "avgtime", "calls", "chtime", "sftime", "alttime"]
if ignoreParents: del outcolumns[0]

with open(settings.civ4BtsProfileDir + "\\Logs\\" + output, 'w') as out:

    if not ignoreParents: out.write("Parent	")
    out.write("Fn	Time (mS)	Main thread time (mS)	Avg time	#calls	Child time	Self time	"
              "Alternate time\n")

    out.writelines(map(lambda rec: "\t".join([str(rec[col]) for col in outcolumns]) + "\n", recs))
