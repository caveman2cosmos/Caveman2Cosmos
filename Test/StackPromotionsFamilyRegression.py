"""Compare the actual old/new family-selection functions, including >32 profiles.

The fixture supplies immutable rule/unit/pool data; both complete production
calculate bodies run against the same inputs, with the real threshold helper.
"""
from pathlib import Path
import subprocess,os,json
R=Path(__file__).resolve().parents[1];W=R if (R/'Build/deps').exists() else R.parents[1]
P=Path(os.environ.get('C2C_STACK_TEST_OUTPUT',str(R/'Build/StackPromotionsFamilyInternal')));P.mkdir(parents=True,exist_ok=True)
def extract(s,sig):
 s=s.replace('\r\n','\n');a=s.index(sig);b=s.index('{',a)+1;depth=1
 while depth:depth+=(s[b]=='{')-(s[b]=='}');b+=1
 return s[a:b]
before=subprocess.check_output(['git','show','a59e4c4f348cd2c361c995f12fffc27219e7ec55:Sources/StackPromotions.cpp'],cwd=R).decode('latin1')
after=(R/'Sources/StackPromotions.cpp').read_bytes().decode('latin1')
cpp=r'''
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "StackPromotionMath.h"
#define PROFILE(x)
typedef int PromotionTypes; typedef int TechTypes; typedef int DomainTypes;
const int NO_PROMOTION=-1,NO_TECH=-1,DOMAIN_SEA=1;
struct CvPlot {} plotA,plotB;
struct CvUnit {
 int strength;bool home;
 const CvPlot* plot()const{return home?&plotA:&plotB;}
 int baseCombatStr()const{return strength;}
 int getTeam()const{return 0;}
 int getDomainType()const{return 0;}
};
struct Profile {int levels[3];bool self;};
struct CvPromotionInfo {
 int family,tech,effect;bool accepts;
 int getStackSupportFamily()const{return family;}
 int getTechPrereq()const{return tech;}
};
struct Globals {
 std::vector<CvPromotionInfo> promos;
 const CvPromotionInfo& getPromotionInfo(int i)const{return promos[i];}
} GC;
struct Team {bool techs[100];bool isHasTech(int t)const{return techs[t];}} team;
#define GET_TEAM(t) team
struct Pool {std::vector<long long> strength;int count[2];};
struct Runtime {std::vector<Profile> profiles;int crowded[4];unsigned unitBuilds;Pool totals;};
struct Effects {int count;int promotions[32];Effects():count(0){}};
Pool& pool(Runtime& r,const CvPlot&,int){return r.totals;}
bool recipient(const CvUnit&,const CvPromotionInfo& p){return p.accepts;}
bool provider(const CvUnit&,const Profile& p,const Runtime&){return p.self;}
int weight(const CvPromotionInfo& p){return p.effect;}
int limit(int,const CvPlot&,int){return 15;}
void add(Effects& e,int p){if(p>=0&&e.count<32)e.promotions[e.count++]=p;}
'''
cpp+='\n'+extract(before,'Effects calculate(').replace('Effects calculate(','Effects calculateBefore(',1)+'\n'
cpp+=extract(after,'Effects calculate(')+'\n'
cpp+=r'''
volatile unsigned checksum=0;
int main(){
 srand(15432);Runtime r;r.unitBuilds=0;CvUnit u;unsigned checks=0;
 const int sizes[]={0,1,22,32,33,60,100};
 for(int size=0;size<7;++size)for(int trial=0;trial<1000;++trial){
  int n=sizes[size];r.profiles.resize(n);r.totals.strength.resize(n);GC.promos.resize(n*3+4);
  for(int t=0;t<100;++t)team.techs[t]=rand()%2!=0;
  for(int i=0;i<n;++i){
   int family=trial%3==0?i*97-10000:rand()%12-6;
   r.profiles[i].self=rand()%2!=0;r.totals.strength[i]=rand()%800;
   for(int level=0;level<3;++level){
    int id=i*3+level;r.profiles[i].levels[level]=rand()%20==0?NO_PROMOTION:id;
    CvPromotionInfo& p=GC.promos[id];p.family=family;p.tech=rand()%11-1;p.effect=rand()%31-15;p.accepts=rand()%3!=0;
   }
  }
  for(int i=0;i<4;++i){r.crowded[i]=n*3+i;GC.promos[n*3+i].effect=-5*(i+1);}
  r.totals.count[0]=rand()%40;r.totals.count[1]=0;u.strength=rand()%200;u.home=rand()%2!=0;
  Effects a=calculateBefore(r,u,plotA),b=calculate(r,u,plotA);
  assert(a.count==b.count);
  for(int i=0;i<a.count;++i)assert(a.promotions[i]==b.promotions[i]);
  ++checks;
 }
 // Isolated allocation benchmark: fixed real 22-profile size; wall numbers
 // describe this stub workload only, never the cost of a whole game turn.
 r.profiles.resize(22);r.totals.strength.assign(22,1000);GC.promos.resize(70);u.strength=100;u.home=false;
 for(int t=0;t<100;++t)team.techs[t]=true;
 for(int i=0;i<22;++i){r.profiles[i].self=false;
  for(int j=0;j<3;++j){int id=i*3+j;r.profiles[i].levels[j]=id;CvPromotionInfo& p=GC.promos[id];p.family=i/2;p.tech=-1;p.effect=j+1;p.accepts=true;}}
 for(int i=0;i<4;++i)r.crowded[i]=66+i;
 const int repeats=200000;clock_t start=clock();
 for(int n=0;n<repeats;++n)checksum+=calculateBefore(r,u,plotA).count;
 clock_t middle=clock();
 for(int n=0;n<repeats;++n)checksum+=calculate(r,u,plotA).count;
 clock_t end=clock();
 printf("PASS %u full production calculate comparisons; old map vs ordered buffer, negative/sparse families, ties, missing tiers, tech gating, zero strength, crowding, >32 profiles\n",checks);
 printf("isolated repeats=%d oldSeconds=%.6f newSeconds=%.6f checksum=%u\n",repeats,double(middle-start)/CLOCKS_PER_SEC,double(end-middle)/CLOCKS_PER_SEC,checksum);
}
'''
(P/'families.cpp').write_text(cpp)
sdk=W/'Build/deps/Microsoft Visual C++ Toolkit 2003';env=dict(os.environ);env['PATH']=str(sdk/'bin')+os.pathsep+env['PATH']
subprocess.run([str(sdk/'bin/cl.exe'),'/nologo','/O2','/MD','/EHsc','/I'+str(sdk/'include'),'/I'+str(R/'Sources'),'/Fefamilies.exe','families.cpp','/link','/LIBPATH:'+str(sdk/'lib')],cwd=P,env=env,check=True)
result=subprocess.check_output([str(P/'families.exe')],cwd=P,env=env,text=True)
print(result);(P/'families-result.txt').write_text(result)
