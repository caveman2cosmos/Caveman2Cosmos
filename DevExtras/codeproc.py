from xitools.cppcrawler import *
import numpy
import regex

c2cSourceDir = r"..\Sources"
crawler = CppCrawler(c2cSourceDir, backupDir=c2cSourceDir+"/backup")

# ommits lines starting with @`
def loadLines(filename):
    with open(filename, "r", encoding="utf-8") as f:
        return list(filter(lambda ln: not ln.startswith("@`"), 
                           f.read().splitlines()))

def escapeMSRegEx():
    global filename
    content = None
    with open(filename, "r", encoding="utf-8") as f:
        content = (f.read()
            .replace("(", "\\(")
            .replace(")", "\\)")
            .replace("|", "\\|")
            .replace("*", "\\*")
            .replace("+", "\\+")
            
            .replace("?", "\\?")
            .replace(".", "\\.")
            .replace("^", "\\^")
            .replace("$", "\\$")
            .replace("#", "\\#")

            .replace("\\", "\\\\")
            .replace("[", "\\[")
            .replace("{", "\\{"))
    with open(filename, "w", encoding="utf-8") as f:
        f.write(content)
            

def unescapeMSRegEx():
    global filename
    content = None
    with open(filename, "r", encoding="utf-8") as f:
        content = (f.read()
            .replace("\\(", "(")
            .replace("\\)", ")")
            .replace("\\|", "|")
            .replace("\\*", "*")
            .replace("\\+", "+")
            
            .replace("\\?", "?")
            .replace("\\.", ".")
            .replace("\\^", "^")
            .replace("\\$", "$")
            .replace("\\#", "#")

            .replace("\\\\", "\\")
            .replace("\\[", "[")
            .replace("\\{", "]"))
    with open(filename, "w", encoding="utf-8") as f:
        f.write(content)
        

def loadCvUnitCallRegexs(n=1):
    with open(r"CvUnitAssets\CvUnitMethodNames_noFunct.txt", "r", encoding="utf-8") as fin:
        lines = list(filter(lambda ln: not ln.startswith("#"),
                            fin.read().splitlines()))
        lineLists = numpy.array_split(lines, n)
        return list(map(lambda lines: Syntax.makeDirectCallRe(lines), lineLists))

def assembleCvUnitCallRegexs():
    with open(r"CvUnitAssets\CvUnitCallRegex_remark.txt", "w", encoding="utf-8") as frem:
        frem.write("Target code has to be normalised: \n" +
                   "* there should be no '->\s' or '.\s' in statements")
    with open(r"CvUnitAssets\CvUnitCallRegex.txt", "w", encoding="utf-8") as fout:
        regexs = loadCvUnitCallRegexs(6)
        for regex in regexs:
            fout.write(regex + "\r\n")


# prot - prototype
def checkPresentDecls(protsTable, file):
    errors = []
    for rec in protsTable:
        [prot, pat, _] = rec
        for mres in regex.finditer(pat, file.read()):
            if mres:
                rec[2] = True if not rec[2] else errors.append(f"duplicate:{mres.start()}: {prot}")


def replaceF(regex, repl):
    global filename
    content = None
    with open(filename, "r", encoding="utf-8") as f:
        content = regex.sub(regex, repl, f.read())
    with open(filename, "w", encoding="utf-8") as f:
        f.write(content)
