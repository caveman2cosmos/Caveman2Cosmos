"""Execute the production XML relevance builder and getters with Civ4's compiler.

The effect cache is an observable test double; it must not be touched for an
irrelevant target. All numerical modifier sums are compared with the old loop.
"""
from pathlib import Path
import subprocess,os,json
R=Path(__file__).resolve().parents[1];W=R if (R/'Build/deps').exists() else R.parents[1]
P=Path(os.environ.get('C2C_STACK_TEST_OUTPUT',str(R/'Build/StackPromotionsRelevanceInternal')))
P.mkdir(parents=True,exist_ok=True)
s=(R/'Sources/StackPromotions.cpp').read_bytes().decode('latin1').replace('\r\n','\n')
def extract(sig):
 a=s.index(sig);b=s.index('{',a)+1;depth=1
 while depth:depth+=(s[b]=='{')-(s[b]=='}');b+=1
 return s[a:b]
cpp=r'''
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstdio>
typedef int UnitCombatTypes; typedef int FeatureTypes; typedef int PromotionTypes;
struct CvUnit {};
struct CvPromotionInfo {
 int kind; std::vector<int> combat,feature;
 CvPromotionInfo():kind(0),combat(97,0),feature(17,0){}
 int getStackSupportKind()const{return kind;}
 int getUnitCombatModifierPercent(int i)const{return combat[i];}
 int getFeatureAttackPercent(int i)const{return feature[i];}
};
struct Globals {
 std::vector<CvPromotionInfo> promos;
 int getNumUnitCombatInfos()const{return 97;}
 int getNumFeatureInfos()const{return 17;}
 const CvPromotionInfo& getPromotionInfo(int i)const{return promos[i];}
} GC;
struct Effects {int count;int promotions[32];} result;
struct Runtime {std::vector<unsigned char> versusTargets,featureTargets;} data;
bool option=true;int cacheReads=0;
bool enabled(){return option;}
Runtime& runtime(){return data;}
const Effects& effects(const CvUnit&){++cacheReads;return result;}
'''
for sig in ('void recordTargets(','int versus(','int featureAttack('):cpp+='\n'+extract(sig)+'\n'
cpp+=r'''
int main(){
 srand(15431);CvUnit u;unsigned checks=0,skipped=0;
 for(int fixture=0;fixture<300;++fixture){
  GC.promos.assign(70,CvPromotionInfo());
  data.versusTargets.assign(97,0);data.featureTargets.assign(17,0);
  // Include definitions beyond today's XML IDs and negative modifiers. Also
  // exercise non-support promotions, which must never populate the union.
  for(int p=0;p<70;++p){
   CvPromotionInfo& info=GC.promos[p];info.kind=p%3;
   for(int i=0;i<97;++i)if(rand()%43==0)info.combat[i]=(rand()%21-10)*5;
   for(int i=0;i<17;++i)if(rand()%43==0)info.feature[i]=(rand()%21-10)*5;
   recordTargets(data,info);
  }
  result.count=0;
  for(int p=0;p<70&&result.count<32;++p)
   if(GC.promos[p].kind&&rand()%2)result.promotions[result.count++]=p;
  for(int on=0;on<=1;++on){
   option=on!=0;
   for(int i=0;i<97;++i){
    int expected=0;
    if(option)for(int p=0;p<result.count;++p)expected+=GC.promos[result.promotions[p]].combat[i];
    int before=cacheReads;assert(versus(u,i)==expected);++checks;
    if(!option||!data.versusTargets[i]){assert(cacheReads==before);++skipped;}
   }
   for(int i=0;i<17;++i){
    int expected=0;
    if(option)for(int p=0;p<result.count;++p)expected+=GC.promos[result.promotions[p]].feature[i];
    int before=cacheReads;assert(featureAttack(u,i)==expected);++checks;
    if(!option||!data.featureTargets[i]){assert(cacheReads==before);++skipped;}
   }
  }
 }
 option=true;GC.promos.assign(1,CvPromotionInfo());
 GC.promos[0].kind=1;GC.promos[0].combat[96]=35;GC.promos[0].feature[16]=-15;
 data.versusTargets.assign(97,0);data.featureTargets.assign(17,0);recordTargets(data,GC.promos[0]);
 result.count=1;result.promotions[0]=0;
 assert(versus(u,96)==35&&featureAttack(u,16)==-15);
 int before=cacheReads;
 for(int n=0;n<1000000;++n){assert(versus(u,0)==0);assert(featureAttack(u,0)==0);}
 assert(cacheReads==before);
 assert(versus(u,-1)==0&&versus(u,97)==0&&featureAttack(u,-1)==0&&featureAttack(u,17)==0);
 printf("PASS actual relevance builder/getters: %u equivalence checks, %u no-cache checks, 2M irrelevant calls bypassed; future IDs, negative modifiers and option OFF\n",checks,skipped);
}
'''
(P/'relevance.cpp').write_text(cpp)
sdk=W/'Build/deps/Microsoft Visual C++ Toolkit 2003'
env=dict(os.environ);env['PATH']=str(sdk/'bin')+os.pathsep+env['PATH']
subprocess.run([str(sdk/'bin/cl.exe'),'/nologo','/O2','/MD','/EHsc','/I'+str(sdk/'include'),'/Ferelevance.exe','relevance.cpp','/link','/LIBPATH:'+str(sdk/'lib')],cwd=P,env=env,check=True)
subprocess.run([str(P/'relevance.exe')],cwd=P,env=env,check=True)
(P/'relevance-result.json').write_text(json.dumps({'actualProductionFunctions':True,'equivalenceChecks':68400,'irrelevantCallsAvoidCache':2000000,'compiler':'VC++2003','passed':True},indent=2))
