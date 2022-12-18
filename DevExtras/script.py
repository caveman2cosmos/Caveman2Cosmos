from codeproc import *

prots = loadLines("DllExportAssets/DllExport_decsepdef_functions.txt")
res = crawler.findMethDeclarations(prots, "*.h")
res.sort(key=lambda rec: (rec[1] is None, rec[0]))

for rec in res:
    print(rec[0], (rec[1][0], rec[1][1].startLoc()) if rec[1] else None)

print(len(res))