# Civ4 Python 2.4: temporary in-memory contract scenario, never saves.
def stackScenario(directory):
 from CvPythonExtensions import CyGlobalContext, CyMap, UnitAITypes, DirectionTypes, GameOptionTypes, CyGameTextMgr
 import os
 gc=CyGlobalContext();game=gc.getGame();m=CyMap();p=gc.getPlayer(game.getActivePlayer())
 option=GameOptionTypes.GAMEOPTION_COMBAT_STACK_PROMOTIONS
 created=[];checks=[]
 def check(label,condition):
  if not condition:raise AssertionError(label)
  checks.append(label)
  progress=open(os.path.join(directory,'stack-contract-progress.txt'),'a');progress.write(label+'\n');progress.close()
 def spawn(name,plot):
  kind=gc.getInfoTypeForString(name)
  if kind<0:raise AssertionError('missing '+name)
  u=p.initUnit(kind,plot.getX(),plot.getY(),UnitAITypes.NO_UNITAI,DirectionTypes.NO_DIRECTION)
  created.append(u);return u
 def stat(u,index):return u.getStackSupportStat(index)
 land=[];sea=[]
 for i in range(m.numPlots()):
  t=m.plotByIndex(i)
  if t.getNumUnits() or t.isCity() or t.isPeak() or t.isImpassable():continue
  if t.getOwner()!=p.getID():continue
  if t.isWater():sea.append(t)
  else:land.append(t)
 check('empty owned test plots',len(land)>1 and len(sea)>1)
 game.setOption(option,True)
 oldSizeMatters=game.isOption(GameOptionTypes.GAMEOPTION_COMBAT_SIZE_MATTERS)
 game.setOption(GameOptionTypes.GAMEOPTION_COMBAT_SIZE_MATTERS,False)
 try:
  a=spawn('UNIT_AXEMAN',land[0]);a.setBaseCombatStr(100);check('no self aid',stat(a,5)==0)
  b=spawn('UNIT_ARCHER',land[0]);b.setBaseCombatStr(100)
  check('archer gives first strike chances',stat(a,5)>0)
  check('melee gives defense',stat(b,2)>0)
  # Shared C++ help is what the Pedia and both technology splash windows display.
  aid=gc.getInfoTypeForString('PROMOTION_STACK_ARROW1')
  tech=gc.getPromotionInfo(aid).getTechPrereq()
  techName=gc.getTechInfo(tech).getDescription()
  for name in ('PROMOTION_STACK_ARROW1','PROMOTION_STACK_ARROW2','PROMOTION_STACK_ARROW3'):
   desc=gc.getPromotionInfo(gc.getInfoTypeForString(name)).getDescription()
   check('tech Pedia lists '+name,desc in CyGameTextMgr().getTechHelp(tech,True,False,False,False,-1))
   check('tech discovery lists '+name,desc in CyGameTextMgr().getTechHelp(tech,True,False,False,True,-1))
  check('promotion Pedia links tech',techName in CyGameTextMgr().getPromotionHelp(aid,True))
  team=gc.getTeam(p.getTeam());hadTech=team.isHasTech(tech)
  check('fixture has Archery',hadTech)
  try:
   team.setHasTech(tech,False,p.getID(),False,False)
   check('missing tech removes aid from cached unit',stat(a,5)==0)
   team.setHasTech(tech,True,p.getID(),False,False)
   check('discovery restores aid',stat(a,5)>0)
  finally:team.setHasTech(tech,hadTech,p.getID(),False,False)
  recon=[];baseForest=a.stackBenchmarkValues()[7]
  for name in ('UNIT_SCOUT','UNIT_EXPLORER'):
   scout=spawn(name,land[0]);scout.setBaseCombatStr(201);recon.append(scout)
   check(name+' provides Recon aid',a.stackBenchmarkValues()[7]==baseForest+30)
   check(name+' has no crowd allowance',scout.getStackSupportLimit()==0)
   scout.kill(False,-1);created.remove(scout);recon.remove(scout)
   check('Recon disappears after '+name+' death',a.stackBenchmarkValues()[7]==baseForest)
  before=[stat(a,i)for i in range(9)]
  b.finishMoves();check('zero moves retains aid',before==[stat(a,i)for i in range(9)])
  # Optional diagnostic-only mutation bridge: never part of the playable API.
  if hasattr(a,'stackBenchmarkJoin'):
   revision=a.stackBenchmarkRevision()
   a.stackBenchmarkJoin(b)
   check('join orders preserve plot support revision',a.stackBenchmarkRevision()==revision)
   check('join orders preserve aid',before==[stat(a,i)for i in range(9)])
   a.stackBenchmarkJoin(b)
   check('noop join preserves plot support revision',a.stackBenchmarkRevision()==revision)
   a.stackBenchmarkSplit()
   check('split orders preserve plot support revision',a.stackBenchmarkRevision()==revision)
   check('split orders preserve aid',before==[stat(a,i)for i in range(9)])

  untouched=(game.getMapRand().getSeed(),game.getSorenRand().getSeed(),p.getGold(),a.getMoves(),b.getMoves(),p.getNumUnits())
  for n in range(100):
   CyGameTextMgr().getSpecificUnitHelp(a,False,False)
   check('read-only hover %d'%n,before==[stat(a,i)for i in range(9)])
  check('hover preserves RNG, treasury, moves and unit count',untouched==(game.getMapRand().getSeed(),game.getSorenRand().getSeed(),p.getGold(),a.getMoves(),b.getMoves(),p.getNumUnits()))
  b.setXY(land[1].getX(),land[1].getY(),False,True,False)
  check('move removes aid immediately',stat(a,5)==0)
  b.setXY(land[0].getX(),land[0].getY(),False,True,False)
  check('return restores aid',stat(a,5)>0)
  gun=spawn('UNIT_RIFLEMAN',land[0]);gun.setBaseCombatStr(100)
  check('equivalent defense sources do not stack',stat(b,2)==5)
  gun.kill(False,-1);created.remove(gun)
  b.kill(False,-1);created.remove(b)
  check('death removes aid immediately',stat(a,5)==0)
  soft=a.getStackSupportLimit();check('technology soft limit',soft>=3)
  civilian=spawn('UNIT_GATHERER',land[0])
  check('civilian excluded',[stat(civilian,i)for i in range(9)]==[0]*9)
  for n in range(soft-1):spawn('UNIT_AXEMAN',land[0])
  for n in range(5):recon.append(spawn('UNIT_SCOUT',land[0]))
  check('scouts do not consume crowd allowance',stat(a,0)==0)
  for n in range(10):spawn('UNIT_AXEMAN',land[0])
  check('scouts do not receive crowd penalty',stat(recon[0],0)==0)
  check('crowding IV combat',stat(a,0)==-30)
  check('crowding IV healing',stat(a,8)==-15)
  oldImprovement=land[0].getImprovementType()
  land[0].setImprovementType(gc.getInfoTypeForString('IMPROVEMENT_FORT'))
  check('fort adds one soft-limit place',a.getStackSupportLimit()==soft+1)
  check('fort change refreshes derived crowding',stat(a,0)==-20)
  land[0].setImprovementType(oldImprovement)
  check('removing fort restores crowding',stat(a,0)==-30)
  promo=gc.getInfoTypeForString('PROMOTION_STACK_OVERCROWDED4')
  check('virtual icon visible',a.isHasPromotion(promo))
  a.setHasPromotion(promo,True)
  a.setDamage(a.getMaxHP()-1,-1)
  actual=a.stackBenchmarkValues()
  expected=[stat(a,i)for i in (0,1,2,5,6,7)]
  game.setOption(option,False)
  base=a.stackBenchmarkValues()
  check('actual combat getters use derived modifiers',[actual[i]-base[i]for i in range(6)]==expected)
  expectedHeal=0
  if base[6]>0:expectedHeal=max(1,base[6]-15)
  check('actual healing applies crowd penalty once',actual[6]==expectedHeal)
  check('off switch removes all modifiers',[stat(a,i)for i in range(9)]==[0]*9)
  check('off switch removes virtual icon',not a.isHasPromotion(promo))
  check('disabled option hides technology unlocks',gc.getPromotionInfo(aid).getDescription() not in CyGameTextMgr().getTechHelp(tech,True,False,False,True,-1))
  game.setOption(option,True)
  check('on switch restores valid effects',stat(a,0)==-30)
  ship=spawn('UNIT_TRIREME',sea[0]);other=spawn('UNIT_FRIGATE',sea[0]);ship.setBaseCombatStr(100);other.setBaseCombatStr(100)
  check('naval escort defense',stat(other,2)>0)
  check('naval maneuver withdrawal',stat(ship,6)>0)
  check('land crowding separate from sea',stat(ship,0)==0)
  carrier=spawn('UNIT_CARRIER',sea[1]);escort=spawn('UNIT_DESTROYER',sea[1]);carrier.setBaseCombatStr(100);escort.setBaseCombatStr(100)
  check('empty carrier gives no air cover',stat(escort,0)==0)
  plane=spawn('UNIT_FIGHTER',sea[1]);plane.setTransportUnit(carrier,True)
  check('loaded carrier supplies air cover',stat(escort,0)>0)
  check('carrier does not support itself',stat(carrier,0)==0)
  check('cargo has no outside aid',[stat(plane,i)for i in range(9)]==[0]*9)
  plane.kill(False,-1);created.remove(plane)
  check('aircraft death removes carrier cover',stat(escort,0)==0)
  aa=spawn('UNIT_CRUISER',sea[0]);aa.setBaseCombatStr(201)
  unarmed=spawn('UNIT_TORPEDO_BOAT',sea[0]);unarmed.setBaseCombatStr(100)
  game.setOption(option,False)
  # Spawned units inherit the loaded empire's free promotions. Normalize this
  # fixture only, so zero-interception is an actual precondition, not a guess.
  for i in range(gc.getNumPromotionInfos()):
   if unarmed.isHasPromotion(i):unarmed.setHasPromotion(i,False)
  f=open(os.path.join(directory,'aa-fixture.txt'),'w');f.write(str(unarmed.stackBenchmarkValues()));f.close()
  check('AA fixture has no extra interception',unarmed.stackBenchmarkValues()[5]==0)
  game.setOption(option,True)
  check('AA cannot create interception ability',stat(unarmed,7)==0)
  interception=gc.getInfoTypeForString('PROMOTION_BUILD_UP_ANTI_AIR1')
  unarmed.setHasPromotion(interception,True)
  supported=stat(unarmed,7)
  game.setOption(option,False)
  f=open(os.path.join(directory,'aa-fixture.txt'),'a')
  f.write('\ntech=%s promo=%s own=%s provider=%s supported=%s'%(team.isHasTech(gc.getInfoTypeForString('TECH_ARTILLERY')),unarmed.isHasPromotion(interception),str(unarmed.stackBenchmarkValues()),str(aa.stackBenchmarkValues()),supported));f.close()
  game.setOption(option,True)
  check('promotion-enabled interception receives AA aid',supported>0)
  unarmed.setHasPromotion(interception,False)
  check('removing interception promotion removes AA aid',stat(unarmed,7)==0)
 finally:
  for u in created:
   if u:u.kill(False,-1)
  game.setOption(GameOptionTypes.GAMEOPTION_COMBAT_SIZE_MATTERS,oldSizeMatters)
 f=open(os.path.join(directory,'stack-contracts.txt'),'w')
 try:f.write('PASS %d checks\n%s\n'%(len(checks),'\n'.join(checks)))
 finally:f.close()
