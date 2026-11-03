"""Definition/linkage audit and execution of actual threshold code with Civ4's compiler."""
from pathlib import Path
import xml.etree.ElementTree as E
import subprocess,os,json,shutil
R=Path(__file__).resolve().parents[1]; W=R if (R/'Build/deps').exists() else R.parents[1]
P=Path(os.environ.get('C2C_STACK_TEST_OUTPUT',str(W/'Build/StackPromotionsImplementation_2026-10-26')));P.mkdir(parents=True,exist_ok=True)
def xml(n):
 root=E.parse(R/n).getroot()
 for e in root.iter():e.tag=e.tag.split('}')[-1]
 return root
promos=xml('Assets/XML/Units/StackPromotions_CIV4PromotionInfos.xml').findall('.//PromotionInfo')
assert len(promos)==70
uc={e.findtext('Type')for e in xml('Assets/XML/Units/CIV4UnitCombatInfos.xml').findall('.//UnitCombatInfo')}
unitInfos=[e for n in ('U_Land','U_Sea','U_Air','U_LandFly','U_Workers')for e in xml('Assets/XML/Units/'+n+'_CIV4UnitInfos.xml').findall('.//UnitInfo')]
units={e.findtext('Type')for e in unitInfos}
techs=xml('Assets/XML/Technologies/CIV4TechInfos.xml').findall('.//TechInfo');techNames={e.findtext('Type')for e in techs}
texts={e.findtext('Tag')for e in xml('Assets/XML/GameText/StackPromotions_CIV4GameText.xml').findall('TEXT')}
schema=xml('Assets/XML/Schema/C2C_CIV4UnitSchema.xml');definition=next(e for e in schema if e.get('name')=='PromotionInfo')
order=[e.get('type')for e in definition if e.tag=='element']
ids=set();profiles={};size=0
for p in promos:
 name=p.findtext('Type');assert name not in ids;ids.add(name)
 assert p.findtext('Description')in texts and p.findtext('Help')in texts
 assert [order.index(e.tag)for e in p]==sorted(order.index(e.tag)for e in p),name
 for e in p.findall('.//UnitCombatType'):assert e.text in uc,(name,e.text)
 for u in p.findtext('StackSupportProviderUnits','').split():assert u in units,(name,u)
 t=p.findtext('TechPrereq');assert not t or t in techNames
 if p.findtext('iStackSupportKind')=='1':assert t in techNames,(name,'missing technology unlock')
 b=R/'Assets'/p.findtext('Button');assert b.exists(),str(b)
 from PIL import Image
 with Image.open(b)as im:assert im.size==(64,64),(b,im.size)
 size+=b.stat().st_size
 if p.findtext('iStackSupportKind')=='1':profiles.setdefault(int(p.findtext('iStackSupportProfile')),[]).append(int(p.findtext('iStackSupportTier')))
assert len(profiles)==22 and all(sorted(v)==[1,2,3]for v in profiles.values())
coverage={}
for p in promos:
 if p.findtext('iStackSupportKind')!='1' or p.findtext('iStackSupportTier')!='1':continue
 names=set(p.findtext('StackSupportProviderUnits','').split())
 anyClass={e.text for e in p.findall('StackSupportProviders/UnitCombatType')}
 allClass={e.text for e in p.findall('StackSupportProviderAll/UnitCombatType')}
 deny={e.text for e in p.findall('StackSupportProviderNot/UnitCombatType')}
 matches=[]
 for u in unitInfos:
  if u.findtext('Domain')!=p.findtext('StackSupportDomain'):continue
  classes={u.findtext('Combat')}|{e.text for e in u.findall('SubCombatTypes/SubCombatType')}
  if not ({'UNITCOMBAT_COMBATANT','UNITCOMBAT_NAVAL_COMBATANT'}&classes):
   if p.findtext('Type')!='PROMOTION_STACK_RECON1' or not ({'UNITCOMBAT_RECON','UNITCOMBAT_EXPLORER'}&classes):continue
  carrier=p.findtext('bStackSupportCarrier')=='1' and u.findtext('DomainCargo')=='DOMAIN_AIR'
  if u.findtext('Type')not in names and not carrier and not (anyClass&classes or not anyClass and allClass):continue
  if not allClass<=classes or deny&classes:continue
  if p.findtext('bStackSupportProviderIntercept')=='1' and int(u.findtext('iInterceptionProbability','0'))<=0:continue
  matches.append(u.findtext('Type'))
 assert matches,('support profile has no existing provider',p.findtext('Type'))
 coverage[p.findtext('Type')]=matches
(P/'provider-coverage.json').write_text(json.dumps(coverage,indent=2))
assert 'UNIT_SCOUT'in coverage['PROMOTION_STACK_RECON1'] and 'UNIT_EXPLORER'in coverage['PROMOTION_STACK_RECON1']
for profile in profiles:
 unlocks={p.findtext('TechPrereq')for p in promos if int(p.findtext('iStackSupportProfile','0'))==profile}
 assert len(unlocks)==1
# Both discovery screen variants and the Civilopedia use the tested C++ helper.
for ui in ('Contrib/TechWindow.py','Contrib/TechWindowWide.py','Screens/Pedia/PediaTech.py'):
 source=subprocess.check_output(['git','show','HEAD:Assets/Python/'+ui],cwd=R,text=True)
 assert '.getTechHelp('in source,ui
assert sum(int(t.findtext('iStackSupportLimitChange','0'))for t in techs)==39
assert all(len(t.findall('iStackSupportLimitChange'))<=1 for t in techs)
src=(R/'Sources/CvUnit.cpp').read_text()
assert 'getMoves'not in (R/'Sources/StackPromotions.cpp').read_text()
assert src.count('StackPromotions::healingRate(*this, pPlot, std::max(1, iTotalHeal))')==2
assert 'if (promo.getStackSupportKind() != 0) return false;'in src
assert 'if (kPromotion.getStackSupportKind() != 0) return;'in src
assert 'persistentModifier = getExtraUnitCombatModifier' in src
for binding in ('CyEnumsInterface.cpp','CvPythonEnumLoader.cpp'):
 assert '.value("GAMEOPTION_COMBAT_STACK_PROMOTIONS", GAMEOPTION_COMBAT_STACK_PROMOTIONS)'in(R/'Sources'/binding).read_text()
cpp=r'''
#include <cassert>
#include <cstdio>
#include "StackPromotionMath.h"
int main(){using namespace StackPromotionMath;
 for(int receiver=1;receiver<1000;++receiver){
  assert(aidTier(0,receiver)==0);
  assert(aidTier(receiver/2,receiver)==0);
  assert(aidTier(receiver/2+1,receiver)>=1);
  assert(aidTier(receiver,receiver)==1);
  assert(aidTier(receiver+1,receiver)==2);
  assert(aidTier(2*receiver,receiver)==2);
  assert(aidTier(2*receiver+1,receiver)==3);
 }
 assert(aidTier(2147483647LL,2147483647LL)==1);
 assert(aidTier(4294967295LL,2147483647LL)==3);
 for(int limit=1;limit<=50;++limit){
  assert(overcrowdedTier(limit,limit)==0);
  assert(overcrowdedTier(limit+1,limit)==1);
  assert(overcrowdedTier(limit+2,limit)==1);
  assert(overcrowdedTier(limit+3,limit)==2);
  assert(overcrowdedTier(limit+5,limit)==2);
  assert(overcrowdedTier(limit+6,limit)==3);
  assert(overcrowdedTier(limit+9,limit)==3);
  assert(overcrowdedTier(limit+10,limit)==4);
 }
 assert(healWithCongestion(0,-15)==0);
 assert(healWithCongestion(-5,-15)==0);
 assert(healWithCongestion(5,-15)==1);
 assert(healWithCongestion(30,-15)==15);
 puts("PASS actual C++ thresholds: strength, overflow, crowding and healing");
}
'''
(P/'thresholds.cpp').write_text(cpp)
sdk=W/'Build/deps/Microsoft Visual C++ Toolkit 2003';env=dict(os.environ);env['PATH']=str(sdk/'bin')+os.pathsep+env['PATH']
subprocess.run([str(sdk/'bin/cl.exe'),'/nologo','/O2','/MD','/EHsc','/I'+str(sdk/'include'),'/I'+str(R/'Sources'),'/Fethresholds.exe','thresholds.cpp','/link','/LIBPATH:'+str(sdk/'lib')],cwd=P,env=env,check=True)
subprocess.run([str(P/'thresholds.exe')],cwd=P,env=env,check=True)
psdk=W/'Build/deps/Microsoft SDKs/Windows/v6.0';py=W/'Build/deps/Python24'
shutil.copyfile(py/'libs/python24.dll',P/'python24.dll')
subprocess.run([str(sdk/'bin/cl.exe'),'/nologo','/EHsc','/MT','/I'+str(sdk/'include'),'/I'+str(psdk/'Include'),'/I'+str(py/'include'),str(R/'Test/StackPromotionsPython24.cpp'),'/Fesyntax24.exe','/link','/LIBPATH:'+str(sdk/'lib'),'/LIBPATH:'+str(psdk/'Lib'),'/LIBPATH:'+str(py/'libs')],cwd=P,env=env,check=True)
subprocess.run([str(P/'syntax24.exe'),str(R/'Test/StackPromotionsInGame.py')],cwd=P,env=env,check=True)
print('PASS XML/XDR order, 70 promotions, 22 profiles, references, texts, 40-unit tech limit, 64px DDS')
(P/'internal-definitions.json').write_text(json.dumps(dict(promotions=len(promos),profiles=len(profiles),thresholds='passed',xml='passed'),indent=2))
