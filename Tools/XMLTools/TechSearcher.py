import xml.etree.ElementTree as ET
import re
import heapq

defaultData = ['CIV4TechInfos.xml']

class TechNode:

  def find(self,text):
    return self.ET.find('t:' + text,self.ns)
  def loadPreq(self) -> None:
    self.fullPreq = []
    self.partialPreq = []
    ands = self.find('AndPreReqs')
    if ands is not None:
      self.fullPreq = [preq.text for preq in ands]
    ors = self.find('OrPreReqs')
    if ors is not None:
      if len(ors) == 1:
        self.fullPreq.append(ors[0].text)
      else:
        self.partialPreq = [preq.text for preq in ors]

  def __init__(self, node) -> None:
    self.ET = node
    self.ns = {'t': re.search('\{(.*)\}\s*TechInfo',node.tag).group(1)}
    self.name = self.find('Type').text
    self.fullPreqOf = []
    self.partialPreqOf = []
    self.loadPreq()



def loadData(files, prefix = '') -> list:
  trees = []
  for f in files:
    trees.append(ET.parse(f))
  return trees

def generateTechList(trees) -> list:
  techList = []
  for tree in trees:
    techList.extend([TechNode(tech) for tech in tree.getroot()[0]])
  return techList

def generateTechLookup(techList) -> dict:
  return {techList[i].name : i for i in range(len(techList))}

def applyReverseConnections(tList, tDict) -> None:
  for tech in tList:
    for preq in tech.fullPreq:
      if preq in tDict:
        tList[tDict[preq]].fullPreqOf.append(tech.name)
    for preq in tech.partialPreq:
      if preq in tDict:
        tList[tDict[preq]].partialPreqOf.append(tech.name)

def updatePreqsOf(node, isFull, nodeDict, targets) -> None:
  updates = node.node.fullPreqOf if isFull else node.node.partialPreqOf
  for update in updates:
    node = nodeDict[update]
    if isFull:
      node.hasFullPreqs += 1
    else:
      node.hasPartialPreqs += 1
    if not node.represented:
      node.represented = True
      heapq.heappush(targets,node)

def nextNode(targets):
  while targets and targets[0].linksLeft() == 0:
    yield heapq.heappop(targets)

def toposort(tList, tDict) -> list:
  class TopoNode:
    def __init__(self, node) -> None:
      self.node = node
      self.represented = False
      self.hasFullPreqs = 0
      self.hasPartialPreqs = 0
    def fullPreqsLeft(self) -> int:
      return max(len(self.node.fullPreq) - self.hasFullPreqs,0)
    def partialPreqsLeft(self) -> int:
      preqs = len(self.node.partialPreq)
      return 0 if self.hasPartialPreqs else preqs
    def linksLeft(self) -> int:
      return self.fullPreqsLeft() + max(len(self.node.partialPreq) - self.hasPartialPreqs,0)#+ self.partialPreqsLeft()
    def getCompareValue(self) -> tuple:
      return (self.linksLeft(),self.node.find('iCost'),self.node.find('iGridX'),self.node.find('iGridY'))
    def __cmp__(self,other):
      return cmp(self.getCompareValue(),other.getCompareValue())
  nodeDict = {node.name : TopoNode(node) for node in tList}
  results = []
  targets = filter(lambda n: n.linksLeft() == 0, nodeDict.values())

  while targets:
    #print([target.node.name for target in targets])
    heapq.heapify(targets)
    temp = 0
    for targs in nextNode(targets):
      results.append(targs.node)
      updatePreqsOf(targs, True, nodeDict, targets)
      updatePreqsOf(targs, False, nodeDict, targets)
      temp += 1
    # escape even if tech is ill formed
    if temp == 0:
      print('emergencyBreak')
      break
  #print([(target.node.name, target.fullPreqsLeft() , target.hasFullPreqs, target.node.fullPreq, target.hasPartialPreqs, target.node.partialPreq) for target in targets])
  return results


def lookup(dic):
  return lambda keys: map(lambda key: dic[key],keys)
def iterDic(dic):
  for key in dic.keys():
    yield (key, dic[key])
class TechTree:
  def __init__(self, xmlFiles, prefix = '') -> None:
    self.techList = generateTechList(loadData(xmlFiles,prefix))
    self.techLookup = generateTechLookup(self.techList)
    applyReverseConnections(self.techList, self.techLookup)
    self.orderedTech = toposort(self.techList, self.techLookup)
    self.orderDict = {self.orderedTech[i].name : i for i in range(len(self.orderedTech))}
  def orderDifference(self, firstName, secondName) -> int:
    return self.orderDict[secondName] - self.orderDict[firstName]
  def strictDependance(self, firstName, secondName) -> int:
    diff = self.orderDifference(firstName, secondName)
    if self.orderDifference(firstName, secondName) < 0:
      return 0
    base = self.orderDict[firstName]
    deps = [0] * (diff + 1)
    deps[0] = 1 #represent the source
    for i in range(diff+1)[1:]:
      j = i + base
      node = self.orderedTech[j]
      test =  lambda f,l: f( preqs >= base and preqs - base < diff and deps[preqs-base] for preqs in lookup(self.orderDict)(l))
      if test(any,node.fullPreq) or (test(all,node.partialPreq) and len(node.partialPreq) > 0):
        deps[i] = 1
    return deps[-1]
  def dependance(self, firstName, secondName) -> int:
    if self.orderDifference(firstName, secondName) < 0:
      return self.strictDependance(secondName,firstName)
    return self.strictDependance(firstName,secondName)
  def listRedudantDependanciesOf(self, baseTech) -> map:
    fulls = sorted(lookup(self.orderDict)(baseTech.fullPreq))
    partials = sorted(lookup(self.orderDict)(baseTech.partialPreq))
    gName = lambda i: self.orderedTech[i].name
    redundantPartials = set(filter(lambda s: any(map(lambda o: o < s and
           self.strictDependance(gName(o),gName(s)),
           partials)),partials))
    partials = list(set(partials) - redundantPartials)
    alls = sorted(fulls + partials)
    return map(gName, list(set(
           filter(lambda s: any(map(lambda o: o > s and
           self.strictDependance(gName(s),gName(o)),
           fulls)),alls)).union(
           set(filter(lambda s: len(partials) > 0 and all(map(lambda o: o > s and
           self.strictDependance(gName(s),gName(o)),
           partials)),fulls))). union(redundantPartials)))
  def listRedudantDependancies(self) -> dict:
    def dictCons():
       for baseTech in self.orderedTech:
         yield (baseTech.name, self.listRedudantDependanciesOf(baseTech))
    return { k: v for k, v in dictCons() if len(v) > 0}

with open('techOut.txt','w') as f:
  tree = TechTree(defaultData)
  for k, v in iterDic(tree.listRedudantDependancies()):
    f.write(k + ': ' + ''.join([v[0]] + map(lambda s: ', ' + s, v[1:])) + '\n')