### PARAMETERS ###

funcExtraProfileMacro = "PROFILE_EXTRA_FUNC"

limit = 100000      # max number of profilings to add

filesToProcess = [ 
    #"CheckSum.h"
    #"CvGame.cpp"
    "*.h", "*.cpp"  
    ]

excludedFiles = [
    "cvgamecoredll.h", "cvgamecoredll.cpp", "nipoint.h", "fassert.h", "fassert.cpp", "win32.h", "win32.cpp", 
    "logging.h", "logging.cpp"
    ] 

excludedProts = [ ] # prototypes of methods not to profile



### PREAMBULA ###

import os, sys

try: # setting current directory when running in VS interactive mode
    curdir = os.path.dirname(os.popen("VS.CurrentDocPath " + os.getenv("_PTVS_PID")).read())
    os.chdir(curdir)
except: pass

sys.path.append("..")



### SCRIPT ###

from xitools.cppcrawler import *



### Definitions ###

__  = f"(?:\\s|{Syntax.commentRe})"
___ = f"{__}*+"

crawler = CppCrawler(r"..\..\..\Sources", backupDir="Backup", encoding="utf-8-sig")
sources = crawler.loadSourceFiles(filesToProcess)

excludedFiles += ["fprofiler.h"]



### Emergency handling ###

def info(msg, src=None, ln=None):
    lnPart = f"[{ln}]" if ln else ""
    fnamePart = f"{crawler.sourceRelPath(src)}{lnPart}: " if src else ""
    print(f"{fnamePart}Info: {msg}")

def warning(msg, src=None, ln=None):
    lnPart = f"[{ln}]" if ln else ""
    fnamePart = f"{crawler.sourceRelPath(src)}{lnPart}: " if src else ""
    print(f"{fnamePart}Warning: {msg}")

def error(msg, src=None, ln=None):
    lnPart = f"[{ln}]" if ln else ""
    fnamePart = f"{crawler.sourceRelPath(src)}{lnPart}: " if src else ""
    msg = f"{fnamePart}Error: {msg}"
    print(msg)
    raise Exception



### Source processing globals ###
    
alreadyProfiledStd = {}
alreadyProfiledNew = {}
uncertainProfiling = {}
unprofiledMaybeRec = {}
unparsable0        = {}
unparsable1        = {}
profilingNotQual   = {}
profileToAdd       = {}
skippedFiles       = set()
skippedDefs        = {}
includesToAdd      = {}
includesWarn       = {}
modifiedSrcs       = set()



### Includes adding ###

srcPrefixPat = regex.compile(
        f"{___}"
        r"(?P<prag>#pragma[ \t]+once\r\n)?"
        f"{___}"
        r"(?P<ifdef>"
        r"#ifndef.*\r\n"
        r"#define.*\r\n)?"
        r"(?P<insert1>(\s*\r\n)?)"
        f"({__}"
        r'|(?P<insert2>)#include[ \t]+"(?P<inc1>[^">]*)"[ \t]*\r\n'
        r'|(?P<insert3>)#include[ \t]+<(?P<inc2>[^">]*)>[ \t]*\r\n'
        r'|#.*\r\n)*+')


def addInclude(file, incname):
    global sources

    if isinstance(file, str):
        try:
            src = [ src for src in sources 
                    if src.filepath() == file or crawler.sourceRelPath(src) == file ][0]
        except IndexError:
            warning(f"Cannot add #include, file not on the source list: {crawler.sourceRelPath(file)}")
            return
    else:
        src = file

    mresPref = src.matchUnscoped(srcPrefixPat)
    qIncluded = src.findUnscoped(r'#include[ \t]+"(?i:'f"{incname}"r')"')
    aIncluded = src.findUnscoped(r'#include[ \t]+<(?i:'f"{incname}"r')>')
    included = qIncluded or aIncluded

    if not mresPref:
        if included:
            includesWarn.setdefault(src, []).append(("bad prefix", incname))
            warning(f"Included, but cannot parse the file prefix: {incname}", src)
            return
        else:
            error(f"Not included and cannot parse the file prefix: {incname}", src)
            
    if src.filepath().endswith(".h") and not mresPref.group("prag") and not mresPref.group("ifdef"):
        warning(f"Header not guarded", src)

    if incname.lower() in map(str.lower, mresPref.captures("inc1")):
        info(f"Already included: {incname}", src)

    elif incname.lower() in map(str.lower, mresPref.captures("inc2")):
        includesWarn.setdefault(src, []).append(("in <>", incname))
        warning(f"Included, but in <> brackets: {incname}", src)

    elif included:
        includesWarn.setdefault(src, []).append(("not in prefix", incname))
        warning(f"Included, but not present in the expected file prefix: {incname}", src)

    else:
        if   mresPref.group("inc1"): src.insert(mresPref.starts("insert2")[0], f'#include "{incname}"\r\n')
        elif mresPref.group("inc2"): src.insert(mresPref.starts("insert3")[0], f'#include "{incname}"\r\n')
        else: src.insert(mresPref.start("insert1"), 
                         f'\r\n#include "{incname}"\r\n' + ("" if mresPref.group("insert1") else "\r\n"))
        includesToAdd.setdefault(src, []).append(incname)
        modifiedSrcs.add(src)
        info(f"Include directive added: {incname}", src)



### Qualifying for profiling ###

profFunc0Re = r"\bPROFILE_(?P<newprof>EXTRA_)?FUNC\b|\bPROFILE(?:_BEGIN|_THREAD\b|\b)"
returnRe    = r"(?P<ret>\breturn\b(?:"f"{Syntax.clipRe}"r"|[^;])*+;)"
assignRe    = r"(?P<assign>\((?:"f"{Syntax.clipRe}"r"|[^;)=])*\)"f"{___}"r")?[^;(\n]+=[^;]+;)"
hdmacroRe   = r"(?P<mac>PROXY_TRACK|(?P<prof>"f"{profFunc0Re}"r"))[^;\n]*(?:;|\r\n)"
condppRe    = r"(?P<condpp>#if\b|#ifdef\b|#ifndef\b).*\r\n"
methBlockPat = regex.compile(
    r"\{"
    f"(?:{__}|{hdmacroRe})*+"
        r"(?P<body>(?s:.*?))" # the last token can be a macro or a directive thus \w
    f"{___}"
    r"}")
profPat  = regex.compile(r"\bPROFILE")
loop0Pat = regex.compile(r"\bwhile\b|\bfor\b|(?i:foreach\w*\s*\()")


def qualifyBlock(src, begin, prot, ln):
    end  = src.blockEnd(begin)
    code = src.intCode(begin, end + 1)
    mres = src.fullmatchUnscoped(methBlockPat, begin, end + 1)
    if not mres:
        warning(f"Could not match the method's body: {prot}", src, ln)
        return False

    bodyBegin = mres.start("body")
    bodyEnd   = mres.end("body")

    if mres.group("newprof"):
        alreadyProfiledNew.setdefault(src, []).append((ln, prot, code, begin))
        info(f"Already profiled (extra): {prot}", src, ln)
        return False

    if mres.group("prof"):
        alreadyProfiledStd.setdefault(src, []).append((ln, prot, code, begin))
        info(f"Already profiled (standard): {prot}", src, ln)
        return False

    if src.findUnscoped(profPat, bodyBegin, bodyEnd):
        uncertainProfiling.setdefault(src, []).append((ln, prot, code, begin))
        warning(f"Pofile macros that cannot be qualified in: {prot} (SKIPPING)", src, ln)
        return False

    methname = Syntax.methProtName(prot)
    if (   (mres2 := src.findUnscoped(r"(?:\b(\w+)\s*->\s*)?"f"\b{methname}"r"\s*\(", bodyBegin, bodyEnd)) and
            mres2.group(1) == "this"):

        unprofiledMaybeRec.setdefault(src, []).append((ln, prot, code, begin))
        warning(f"Possibly recursive unprofiled method: {prot} (SKIPPING)", src, ln)
        return False
        
    if src.findUnscoped(loop0Pat, bodyBegin, bodyEnd):
        profileToAdd.setdefault(src, []).append((ln, prot, code, begin))
        info(f"Adding profiling to: {prot}", src, ln)
        return True

    else:
        profilingNotQual.setdefault(src, []).append((ln, prot, code, begin))
        return False



### Files proccessing ###

initRe = r"(?::(?:\s|"f"{Syntax.clipRe}"r"|[^{])*+)"
methDef0Pat = SourceFile.makeSkipBlocksPat(Syntax.methFinderRe)
methDef1Pat = regex.compile(f"{___}{initRe}?{___}"r"(?:(?P<block>\{)|[;=])")

scopeSelectors = [lambda src: True, 
                  lambda src: src.tryScopeToNamespaceBody("\w+", skipBlocks=False),
                  lambda src: src.tryScopeToStructBody("\w+", skipBlocks=False),
                  lambda src: src.tryScopeToClassBody("\w+", skipBlocks=False)]
def qualify():
    global alreadyProfiledCount, skippedDefs, skippedFiles, sources
    qualified = 0

    for src in sources:
        if qualified == limit: break

        basename = os.path.basename(src.filepath()).lower()
        if basename in excludedFiles: 
            warning(f"File excluded", src)
            skippedFiles.add(src)
            continue
        
        methBlockBegins = []

        for selectScope in scopeSelectors:
            if selectScope(src):
                matches = list(src.findAll(methDef0Pat, skipBlocks=True, scopeTag=True))
                for (mres, scopeTag) in matches:
                    ln = mres.startLoc()[0]
                    match Syntax.parseMethPrototype(src.intCode(), mres.intStart()):
                        case (prot, _, pos):
                            pos = src.orgPos(pos)
                            match scopeTag:
                                case (_, name): prot = f"{name}::{prot}"
                            mres2 = src.matchUnscoped(methDef1Pat, pos)
                            if not mres2:
                                unparsable1.setdefault(src, []).append((ln, prot))
                                continue
                            if mres2.group("block"):
                                if prot in excludedProts:
                                    warning(f"Prototype excluded: {prot}", src, ln)
                                    skippedDefs.setdefault(src, []).append((ln, prot))
                                    continue
                                methBlockBegins.append((mres2.start("block"), prot, ln))

                        case None:
                            unparsable0.setdefault(src, []).append((ln, mres.group(0)))

                src.resetScopes()
                
        for (begin, prot, ln) in methBlockBegins:
            if qualifyBlock(src, begin, prot, ln): qualified += 1
            if qualified == limit:
                break

    print()
    print("---")
    print("Profilings to be added (variable: profileToAdd):", len(flatten(profileToAdd.values())))
    print("Already profiled methods - standard (variable: alreadyProfiledStd):", len(flatten(alreadyProfiledStd.values())))
    print("Already profiled methods - extra (variable: alreadyProfiledNew):", len(flatten(alreadyProfiledNew.values())))
    print("Methods with profiling that couldn't be analised (variable: uncertainProfiling):", 
        len(flatten(uncertainProfiling.values())))
    print("Unprofiled methods that maybe be recursive - candidates (variable: unprofiledMaybeRec):", 
        len(flatten(unprofiledMaybeRec.values())))
    print("Other methods not qualified for profiling (variable: profilingNotQual):", 
        len(flatten(profilingNotQual.values())))
    print("Excluded files (variable: skippedFiles):", len(skippedFiles))
    print("Excluded definitions (variable: skippedDefs):", len(flatten(skippedDefs.values())))
    print()


def addProfiling():
    addFProfilerTo = set()

    for src in profileToAdd:
        replMatches = []

        for (ln, prot, code, begin) in profileToAdd[src]:
            (pos, space) = src.posToInsertBlockSPrefix(begin)
            replMatches.append((SourceRangeMatch(src, (pos, pos), copySource=False), 
                                f"{funcExtraProfileMacro}();" + space))

        if replMatches:
            src.replaceMatches(replMatches, lambda tmres: tmres[1])
            modifiedSrcs.add(src)
            addFProfilerTo.add(src.filepath())
    
    print()
    print("PROFILE MACROS ADDED")
    print()

    for filepath in addFProfilerTo:
        if not (filepath.endswith(".cpp") and filepath[:-3] + "h" in addFProfilerTo):
            addInclude(filepath, "FProfiler.h")
    
    print()
    print("---")
    print("Profile macros added (variable: profileToAdd):", len(flatten(profileToAdd.values())))
    print("Include directives added (variable: includesToAdd):", len(flatten(includesToAdd.values())))
    print()


def save():
    print("Saving...", end=" ")
    crawler.saveSources(modifiedSrcs)
    print("Done")
    print("Backup available in:", crawler.backupBucketDirs()[-1])
    print()
    print("Note: Line numbers and positions in the modified files have changed. If you want to refer to them"
          "the original \nfiles can be found in the backup directory.")



qualify()

if input(f"Do you want to continue? You can do it later by calling 'addProfiling()'.\n"
          "You are free to move records from other variables to profileToAdd if you want additional methods to be "
          "profiled. (y/n):").strip().lower() == 'y':
    addProfiling()

    if not modifiedSrcs:
        print("Nothing to save")

    elif input(f"Do you want to save {len(modifiedSrcs)} file(s)? You can do it later by calling 'save()'. "
                "(y/n):").strip().lower() == 'y':
        save()

print()
print("SCRIPT FINISHED")
