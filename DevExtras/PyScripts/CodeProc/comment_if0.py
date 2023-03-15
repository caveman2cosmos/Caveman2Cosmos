### DESCRIPTION ###
#
# The script comments out #if 0 blocks. 
# 
# It supports #else. #if 0 blocks with #elif are not commented (there are none in C2C).
#
# The script can be run from Visual Studio: Debug > Execute File in Python Interactive.
#
# Requirements:
# * XiTools CppCrawler 0.1-alpha1 ( https://github.com/klara-zielinska/xitools.py.cppcrawler )
# * run DevExtras\setup.ps1
#
###


### PARAMETERS ###

includedSources = ["*.h", "*.cpp"]
excludedSources = []


### PREAMBULA ###

import os, sys

try: # setting current directory when running in VS interactive mode
    curdir = os.path.dirname(os.popen("VS.CurrentDocPath " + os.getenv("_PTVS_PID")).read())
    os.chdir(curdir)
except: pass

sys.path.append("..")


### SCRIPT ###


from xitools.cppcrawler import *
crawler = CppCrawler("../../../Sources", backupDir="../../Backup")
excludedSources = { path.lower() for path in excludedSources }

def srcMatchMsg(src, match, msg): 
    return f"{src.filepath()}:{match.startLoc()[0]}: {msg}"

# given an #if... directive position, finds the corresponding #else, #elif or #endif (whichever comes first)
def findEndifElse(src, pos, *, findElse=True):
    elseRe = r"|else|elif" if findElse else ""
    preprocRe = r"(?<=^|\n)[ \t]*#(?P<dir>if|endif"f"{elseRe}).*\n?" # matches #ifdef, #ifndef
    while match := src.findUnscoped(preprocRe, pos):
        if match.group("dir") == "if":
            match2 = findEndif(src, match.end())
            assert match2, srcMatchMsg(src, match, "#if has no #endif")
            pos = match2.end()
        else:
            return match
    return None

def findEndif(src, pos):
    return findEndifElse(src, pos, findElse=False)

toSave = set()

for src in crawler.loadSourceFiles(includedSources):
    if crawler.sourceRelPath(src).lower() in excludedSources: continue

    pos = 0
    while match := src.findUnscoped(r"(?<=^|\n)[ \t]*#if[ \t]+0\b.*\n?", pos):
        match2 = findEndifElse(src, match.end())
        assert match2, srcMatchMsg(src, match, "#if has no #endif")
        pos = match2.end()

        match match2.group("dir"):
            case "elif":
                print(srcMatchMsg(src, match2, "#elif not supported"))
                continue

            case "else":
                match3 = findEndif(src, match2.end())
                assert match3, srcMatchMsg(src, match, "#if has no #endif")
                src.insert(match3.start(), "//") # code modifications invalidate matched positions behind, thus
                                                 # we need to perform them latter first

        src.setScope(match.start(), match2.start())
        src.replace(r"(?<=^|\n)", "//")
        toSave.add(src)
        # after inserting // pos can be inside a comment - this is ok, because findUnscoped omits comments

crawler.saveSources(toSave, encoding="utf-8")

print("SCRIPT FINISHED")
