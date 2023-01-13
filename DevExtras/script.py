import os

os.chdir(r"C:\Projects\Caveman2Cosmos\DevExtras")

from codeproc import *
from copy import copy
import os
import subprocess

found = 0
#results = crawler.findAll(
#    r"(?:^|\n)(?P<begLn>).*(?P<prot>\w+\s*\([^\(\)]*\)(?:\s*const)?)(?=\s*\{)", 
#    "*.cpp", skipBlocks=True)
#found = len(flatten(results.values()))


re = r"(?:^|\n)(?P<begLn>)[^\n<]*\bCvPlayer(?:AI)?\b(?!::).*(?P<prot>\b\w+\s*\([^\(\)]*\)(?:\s*const)?)(?:\s*\{|\s*;)"
results = []
for src in crawler.loadSourceFiles(["*.h", "*.hpp", "*.cpp", "*.c"]):

    results += src.findAllGen(re, skipBlocks=True)

    while src.tryScopeToBlockByPrefix(r"namespace\s+\w+\s*\{"
                                      r"|(?:class|struct)\s+\w+\s*[^\{;]*\{"):
        results += src.findAllGen(re, skipBlocks=True)

for mres in results:
    print(mres.filename(), mres.startLoc(), mres.group(0))


#src = crawler.loadSourceFile("CvArtFileMgr.h")
#src.tryScopeToClassBody("CvGlobals", (None, None))

#subprocess.run([r"C:\Projects\VS.Edit.Goto\VS.Edit.Goto\bin\Debug\VS.Edit.Goto.exe", 
#                f"VisualStudio.DTE.{os.environ['VISUALSTUDIOVERSION']}", 
#                r"C:\Projects\Caveman2Cosmos\Sources\CvStructs.h", "244"])


#def dictmap(fn, d):
#    return { fn(k): fn(v) for k, v in d.items() }


#src0 = crawler.loadSourceFile("algorithm2.h")
#d = SourceMatchDict({ src0 : [], None : ["x"] })

#src = crawler.loadSourceFile("CvBonusInfo.h")
#re = Syntax.makeMethProtRe("copyNonDefaults(const%CvBonusInfo*)", resultType=True)
#mres = src.find(re)
#print(mres)

#md = SourceMatchDict({ src: [mres] })
#md2 = md.precededWith(r"/\*DllExport\*/[ \t]*")
#print(md2)

#prots = loadLines("DllExportAssets/DllExport_decsepdef_functions.txt")
#res = crawler.findMethDefinitions(prots, "*.cpp")

#res = [ (prot, mres and (src, mres)) for src, results in res.items() for mres, prot in results ]
#res.sort(key=lambda rec: (rec[1] is None, rec[0]))

#for rec in res:
#    print(rec[0], (rec[1][0].filename(), rec[1][1].startLoc()) if rec[1] else None)

#print(len(res))

#c2cSourceDir = r"..\Sources"
#crawler = CppCrawler(c2cSourceDir, backupDir="backup")

#sources = crawler.loadSourceFiles(["algorithm2.h", "CvAllocator.h"])
#results = crawler.findAll("namespace (map_fun_details|algo|detail|MemTrack)", sources)
#scopes = results.scopesBehind()
#results2 = crawler.matchScopes(r"\s*\{", scopes)
#results3 = crawler.find(r"}", results2.scopesBehind(), perScope=True, skipBlocks=True)
#crawler.replaceSourceMatches(results2 + results3, 
#                             lambda mres: "/*close*/}" if mres.group(0) == "}" else f"{mres.group(0)}/*open*/")
#crawler.saveSources(sources)
