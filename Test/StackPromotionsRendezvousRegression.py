"""Run the actual location-aware regroup policy, plus cache mutation contracts."""
from pathlib import Path
import subprocess,os
R=Path(__file__).resolve().parents[1];W=R if (R/'Build/deps').exists() else R.parents[1];P=Path(os.environ.get('C2C_STACK_TEST_OUTPUT',str(R/'Build/StackPromotionsRendezvousInternal')));P.mkdir(parents=True,exist_ok=True)
s=(R/'Sources/CvUnitAI.cpp').read_bytes().decode('latin1').replace('\r\n','\n')
def function(s,sig):
 a=s.index(sig);b=s.index('{',a)+1;depth=1
 while depth:depth+=(s[b]=='{')-(s[b]=='}');b+=1
 return s[a:b]
body=function(s,'bool CvUnitAI::AI_canGroupWithAIType(')
old=subprocess.check_output(['git','show','a59e4c4f348cd2c361c995f12fffc27219e7ec55:Sources/CvUnitAI.cpp'],cwd=R).decode('latin1')
before=function(old,'bool CvUnitAI::AI_canGroupWithAIType(').replace('CvUnitAI::AI_canGroupWithAIType','CvUnitAI::before',1)
allow=function(s,'bool CvUnitAI::AI_allowGroup(')
assert 'StackPromotions::enabled() && !AI_canGroupWithAIType(eUnitAI, pPlot)'in allow
assert allow.index('AI_canGroupWithAIType(eUnitAI, pPlot)')<allow.index('canJoinGroup(')
u=(R/'Sources/CvUnit.cpp').read_bytes().decode('latin1').replace('\r\n','\n')
setter=function(u,'void CvUnit::setHasPromotion(PromotionTypes eIndex, bool bNewValue, bool bFree')
assert setter.index('eIndex == NO_PROMOTION')<setter.index('kPromotion.getStackSupportKind()')
assert setter.count('if (stackSupportChanged && plot()) plot()->invalidateStackSupport();')==2
assert 'setAfflictionTurnCount'in setter and 'setPromotionFreeCount'in setter and 'setPromotionFromTrait'in setter
classes=function(u,'void CvUnit::setHasUnitCombat(');process=function(u,'void CvUnit::processUnitCombat(')
assert 'invalidateStackSupport();'not in classes and process.count('invalidateStackSupport();')==2
cpp=r'''
#include <cassert>
#include <cstdio>
#include <cstddef>
typedef int UnitAITypes;const int UNITAI_ATTACK_CITY=5;
struct CvCity {int acquired;int getGameTurnAcquired()const{return acquired;}};
struct CvPlot {bool city;CvCity data;bool isCity()const{return city;}const CvCity* getPlotCity()const{return &data;}};
struct Game {int getGameTurn()const{return 50;}};
struct Globals {Game game;const Game&getGame()const{return game;}}GC;
struct CvUnitAI {
 int role;CvPlot* location;
 int AI_getUnitAIType()const{return role;}
 const CvPlot* plot()const{return location;}
 bool before(UnitAITypes)const;
 bool AI_canGroupWithAIType(UnitAITypes,const CvPlot*at=NULL)const;
};
'''+before+'\n'+body+r'''
int main(){CvPlot source,target;CvUnitAI u;u.location=&source;unsigned checks=0;
 for(int role=0;role<51;++role)for(int wanted=0;wanted<51;++wanted)
  for(int city=0;city<2;++city)for(int age=-2;age<5;++age){
   u.role=role;source.city=city!=0;source.data.acquired=50-age;
   assert(u.before(wanted)==u.AI_canGroupWithAIType(wanted));
   target.city=city!=0;target.data.acquired=50-age;
   assert(u.AI_canGroupWithAIType(wanted,&target)==!(role!=wanted&&wanted==5&&city&&age<=1));++checks;
  }
 u.role=4;source.city=false;target.city=true;target.data.acquired=50;
 assert(u.AI_canGroupWithAIType(5));assert(!u.AI_canGroupWithAIType(5,&target));
 // Waiting out the captured-city cooldown restores the same rendezvous.
 target.data.acquired=48;assert(u.AI_canGroupWithAIType(5,&target));
 // Same-role armies are not blocked, and ordinary field targets stay legal.
 target.data.acquired=50;u.role=5;assert(u.AI_canGroupWithAIType(5,&target));
 u.role=4;target.city=false;assert(u.AI_canGroupWithAIType(5,&target));
 printf("PASS %u actual regroup-policy comparisons, source compatibility, captured-city arrival, cooldown and same-role exceptions; real promotion/class invalidation contracts\n",checks);
}
'''
(P/'rendezvous.cpp').write_text(cpp)
sdk=W/'Build/deps/Microsoft Visual C++ Toolkit 2003';env=dict(os.environ);env['PATH']=str(sdk/'bin')+os.pathsep+env['PATH']
subprocess.run([str(sdk/'bin/cl.exe'),'/nologo','/O2','/MD','/EHsc','/I'+str(sdk/'include'),'/Ferendevous.exe','rendezvous.cpp','/link','/LIBPATH:'+str(sdk/'lib')],cwd=P,env=env,check=True)
out=subprocess.check_output([str(P/'rendevous.exe')],cwd=P,env=env,text=True);print(out);(P/'rendezvous-result.txt').write_text(out)
