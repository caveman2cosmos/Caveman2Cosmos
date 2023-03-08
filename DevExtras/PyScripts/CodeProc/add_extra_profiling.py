### DESCRIPTION ###
#
# The script adds funcExtraProfileMacro at the beginning of methods and functions that contain a loop or a call
# of an iterating function, and are not already profiled.
#
# The iterating functions are functions containing "foreach" in the name - case-insensitive.
#
# The script can be run from Visual Studio: Debug > Execute File in Python Interactive.
#
# Requirements:
# * XiTools CppCrawler 0.1-alpha1 ( https://github.com/klara-zielinska/xitools.py.cppcrawler )
# * run DevExtras\setup.ps1
#
###



### PARAMETERS ###

funcExtraProfileMacro = "PROFILE_EXTRA_FUNC"  # name of the macro to be included in profiled methods

limit = 100000  # max number of methods to add profiling

includedFiles = [ "*.h", "*.cpp" ]

excludedFiles = [
    "cvgamecoredll.h", "cvgamecoredll.cpp", "nipoint.h", "fassert.h", "fassert.cpp", "win32.h", "win32.cpp", 
    "logging.h", "logging.cpp"
    ]   

excludedProts = [ ]  # prototypes of methods not to be profiled - these have to be in the base prototype normal form
                     # described in the Syntax.makeBaseProtRe() documentation of CppCrawler



### PREAMBULA ###

import os, sys

try: # setting current directory when running in VS interactive mode
    curdir = os.path.dirname(os.popen("VS.CurrentDocPath " + os.getenv("_PTVS_PID")).read())
    os.chdir(curdir)
except: pass

sys.path.append("..")



### SCRIPT ###

from xitools.cppcrawler import *



#### Definitions ####

__  = f"(?:{Syntax.commentRe}|\\s)" # used in regular expressions to match code considered a white space in C++
___ = f"{__}*+"                     # like above but many

crawler = CppCrawler(r"..\..\..\Sources", backupDir="..\..\Backup", encoding="utf-8-sig")
sources = crawler.loadSourceFiles(includedFiles)
excludedFiles =  [path.lower() for path in excludedFiles] + ["fprofiler.h"]



#### Utils ####

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

def askYesNo(msg):
    res = input(msg + " (y/n): ").strip().lower()
    while res not in ["y", "n"]:
        res = input("Wrong answer. Try again:").strip().lower()
    return res



#### Source processing globals ####
    
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
sourcesToSave      = set()



#### Include adding ####

srcPrefixPat = regex.compile(
        f"{___}"
        r"(?P<prag>#pragma[ \t]+once\r\n)?"
        f"{___}"
        r"(?P<ifdef>"
        r"#ifndef.*\r\n"
        r"#define.*\r\n)?"
        r"(?P<insert1>(\s*\r\n)?)"
        f"({___}"
        r'|(?P<insert2>)#include[ \t]+"(?P<inc1>[^">]*)"[ \t]*\r\n'
        r'|(?P<insert3>)#include[ \t]+<(?P<inc2>[^">]*)>[ \t]*\r\n'
        r'|#.*\r\n)*+')


# adds #include "incname" in the file (either path or SourceFile) if it is not present
def addInclude(file, incname):
    if isinstance(file, str):
        try:
            src = [ src for src in sources 
                    if src.filepath() == file or crawler.sourceRelPath(src) == file ][0]
        except IndexError:
            warning(f"Cannot add #include, file not on the source list: {crawler.sourceRelPath(file)}")
            return
    else:
        src = file

    matchPref = src.matchUnscoped(srcPrefixPat, 0)
    qIncluded = src.findUnscoped(r'#include[ \t]+"(?i:'f"{incname}"r')"')
    aIncluded = src.findUnscoped(r'#include[ \t]+<(?i:'f"{incname}"r')>')
    included = qIncluded or aIncluded

    if not matchPref:
        if included:
            includesWarn.setdefault(src, []).append(("bad prefix", incname))
            warning(f"Included, but cannot parse the file prefix: {incname}", src)
            return
        else:
            error(f"Not included and cannot parse the file prefix: {incname}", src)
            
    if src.filepath().endswith(".h") and not matchPref.group("prag") and not matchPref.group("ifdef"):
        warning(f"Header not guarded", src)

    if incname.lower() in map(str.lower, matchPref.captures("inc1")):
        info(f"Already included: {incname}", src)

    elif incname.lower() in map(str.lower, matchPref.captures("inc2")):
        includesWarn.setdefault(src, []).append(("in <>", incname))
        warning(f"Included, but in <> brackets: {incname}", src)

    elif included:
        includesWarn.setdefault(src, []).append(("not in prefix", incname))
        warning(f"Included, but not present in the expected file prefix: {incname}", src)

    else:
        if   matchPref.group("inc1"): src.insert(matchPref.starts("insert2")[0], f'#include "{incname}"\r\n')
        elif matchPref.group("inc2"): src.insert(matchPref.starts("insert3")[0], f'#include "{incname}"\r\n')
        else: src.insert(matchPref.start("insert1"), 
                         f'\r\n#include "{incname}"\r\n' + ("" if matchPref.group("insert1") else "\r\n"))
        includesToAdd.setdefault(src, []).append(incname)
        sourcesToSave.add(src)
        info(f"Include directive added: {incname}", src)



#### Code qualification for profiling ####

# macros recognized as a profiling start
profFuncRe = r"\bPROFILE_(?P<newprof>EXTRA_)?FUNC\b|\bPROFILE(?:_BEGIN|_THREAD\b|\b)"
# all macros that are considered a method header (profiled methods are expected to call profiling in the header)
hdMacroRe   = r"(?P<mac>PROXY_TRACK|(?P<prof>"f"{profFuncRe}"r"))[^;\n]*(?:;|\r\n)"
methBlockPat = regex.compile(
    r"\{"
    f"(?:{__}|{hdMacroRe})*+"
        r"(?P<body>(?s:.*?))"
    f"{___}"
    r"}")
profPat  = regex.compile(r"\bPROFILE")
loopPat = regex.compile(r"\bwhile\b|\bfor\b|(?i:foreach\w*\s*\()")


# qualifies a method block for profiling (positive if it contains a loop or a call that is likely an iteration)
# begin - start position of a block (the { character), 
# prot - parsed method prototype,
# ln - line number where the prototype begins
def qualifyBlock(src, begin, prot, ln):
    end  = src.blockEnd(begin)
    code = src.intCode(begin, end + 1)
    match1 = src.fullmatchUnscoped(methBlockPat, begin, end + 1)
    if not match1:
        warning(f"Could not match the method's body: {prot}", src, ln)
        return False

    bodyBegin = match1.start("body")
    bodyEnd   = match1.end("body")

    if match1.group("newprof"):
        alreadyProfiledNew.setdefault(src, []).append((ln, prot, code, begin))
        info(f"Already profiled (extra): {prot}", src, ln)
        return False

    if match1.group("prof"):
        alreadyProfiledStd.setdefault(src, []).append((ln, prot, code, begin))
        info(f"Already profiled (standard): {prot}", src, ln)
        return False

    if src.findUnscoped(profPat, bodyBegin, bodyEnd):
        uncertainProfiling.setdefault(src, []).append((ln, prot, code, begin))
        warning(f"Pofile macros that cannot be qualified in: {prot} (SKIPPING)", src, ln)
        return False

    methname = Syntax.extractMethProtId(prot)[0][-1]
    if (   (match2 := src.findUnscoped(r"(?:\b(\w+)\s*->\s*)?"f"\b{methname}"r"\s*\(", bodyBegin, bodyEnd)) and
            match2.group(1) == "this"):

        unprofiledMaybeRec.setdefault(src, []).append((ln, prot, code, begin))
        warning(f"Possibly recursive unprofiled method: {prot} (SKIPPING)", src, ln)
        return False
        
    if src.findUnscoped(loopPat, bodyBegin, bodyEnd):
        profileToAdd.setdefault(src, []).append((ln, prot, code, begin))
        info(f"Adding profiling to: {prot}", src, ln)
        return True

    else:
        profilingNotQual.setdefault(src, []).append((ln, prot, code, begin))
        return False



#### Files processing ####

# regular expression used to find method/function definitions (sufficient in this project)
methFinderPat = SourceFile.makeSkipBlocksPat(r"(?:\b\w+"f"{___}::{___})*{Syntax.methFinderRe}")

initRe = r"(?::(?:\s|"f"{Syntax.clipRe}"r"|[^{])*+)"
methProtSuffixPat = regex.compile(f"{___}{initRe}?{___}"r"(?:(?P<block>\{)|[;=])") # possible suffixes of
                                                                                   # a method prototype


# following functions take a SourceFile and set search scopes in it
scopeSelectors = [lambda src: True, # leavs the default (total) scope
                  lambda src: src.tryScopeToNamespaceBody(r"\w+", skipBlocks=False),  # scope to all namespaces
                  lambda src: src.tryScopeToClassBody("*", r"\w+", skipBlocks=False)] # scope to all classes

# reads sources and qualifies methods for profiling
def qualify():
    qualified = 0

    for src in sources:
        if qualified == limit: break
        
        if crawler.sourceRelPath(src).lower() in excludedFiles: 
            warning(f"File excluded", src)
            skippedFiles.add(src)
            continue
        
        methBlockBegins = []

        for selectScope in scopeSelectors:
            if not selectScope(src): continue
            
            for (match1, scopeTag) in src.findAll(methFinderPat, skipBlocks=True, scopeTag=True):
                ln = match1.startLoc()[0]
                match Syntax.parseMethPrototype(src.intCode(), match1.intStart()):
                    case (prot, _, pos):
                        pos = src.orgPos(pos)
                        match scopeTag:
                            case (_, name): prot = f"{name}::{prot}" # in this case the scope is a class
                        match2 = src.matchUnscoped(methProtSuffixPat, pos)
                        if not match2:
                            unparsable1.setdefault(src, []).append((ln, prot))
                            continue
                        if match2.group("block"):
                            if prot in excludedProts:
                                warning(f"Prototype excluded: {prot}", src, ln)
                                skippedDefs.setdefault(src, []).append((ln, prot))
                                continue
                            methBlockBegins.append((match2.start("block"), prot, ln))

                    case None:
                        unparsable0.setdefault(src, []).append((ln, match1.group(0)))

            src.resetScopes()
                
        for (begin, prot, ln) in methBlockBegins:
            if qualifyBlock(src, begin, prot, ln): qualified += 1
            if qualified == limit:
                break

    print()
    print("---")
    print("Profilings to be added (variable: profileToAdd):", len(flatten(profileToAdd.values())))
    print("Already profiled methods - standard (variable: alreadyProfiledStd):", 
          len(flatten(alreadyProfiledStd.values())))
    print("Already profiled methods - extra (variable: alreadyProfiledNew):", 
          len(flatten(alreadyProfiledNew.values())))
    print("Methods with profiling that couldn't be analysed (variable: uncertainProfiling):", 
        len(flatten(uncertainProfiling.values())))
    print("Unprofiled methods that maybe be recursive - candidates (variable: unprofiledMaybeRec):", 
        len(flatten(unprofiledMaybeRec.values())))
    print("Other methods not qualified for profiling (variable: profilingNotQual):", 
        len(flatten(profilingNotQual.values())))
    print("Excluded files (variable: skippedFiles):", len(skippedFiles))
    print("Excluded definitions (variable: skippedDefs):", len(flatten(skippedDefs.values())))
    print()


# adds profiling macros to methods stored in profileToAdd
def addProfiling():
    addFProfilerTo = set() # list of source files where to include FProfiler.h

    for src in profileToAdd:
        replMatches = [] # list of pairs (SourceMatch, code), where the former is a 0-width match that points the
                         # place to insert and the latter is the profiling code to be inserted - later used as a list
                         # of tagged matches

        for (ln, prot, code, begin) in profileToAdd[src]:
            (hdspace, pos, tlspace) = src.blockSPrefixInsertPos(begin) # position to insert a single-line block prefix and
                                                              # space to be added behind for indentation preservation
            replMatches.append((SourceRangeMatch(src, (pos, pos), copySource=False), 
                                hdspace + f"{funcExtraProfileMacro}();" + tlspace))

        if replMatches:
            src.replaceMatches(replMatches, lambda tmatch: tmatch[1]) # the method takes a list of possibly tagged 
                                                                      # matches and a function that for such a match
                                                                      # returns the replacement code
            sourcesToSave.add(src)
            addFProfilerTo.add(src.filepath())
    
    print()
    print("PROFILE MACROS ADDED")
    print()

    # the following including procedure is sufficient in this project (in general some .cpp could miss FProfiler.h)
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
    crawler.saveSources(sourcesToSave, encoding="utf-8")
    print("Done")
    print("Backup available in:", crawler.backupBucketDirs()[-1])
    print()
    print("Note: Line numbers and positions in the modified files have changed. If you want to refer to them"
          "the original \nfiles can be found in the backup directory.")



#### Main ####

qualify()

if askYesNo(f"Do you want to continue? You can do it later by calling 'addProfiling()'.\n"
             "You are free to move records from other variables to profileToAdd if you want additional methods to be "
             "profiled.") == 'y':
    addProfiling()

    if not sourcesToSave:
        print("Nothing to save")

    elif askYesNo(f"Do you want to save {len(sourcesToSave)} file(s)? You can do it later by calling 'save()'.\n"
                   "You can verify the content to be saved in the sourcesToSave variable.") == 'y':
        save()

print()
print("SCRIPT FINISHED")
