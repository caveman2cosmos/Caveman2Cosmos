"""Actual candidate-order helper and integration contracts for local Aid merges."""
from pathlib import Path
import os,subprocess
R=Path(__file__).resolve().parents[1];W=R if (R/'Build/deps').exists() else R.parents[1]
P=Path(os.environ.get('C2C_STACK_TEST_OUTPUT',str(R/'Build/StackPromotionsGroupChoiceInternal')));P.mkdir(parents=True,exist_ok=True)
s=(R/'Sources/CvUnitAI.cpp').read_bytes().decode('latin1')
for name in ('AI_group','AI_groupMergeRange'):
 a=s.index('bool CvUnitAI::'+name+'(');b=s.index('{',a)+1;depth=1
 while depth:depth+=(s[b]=='{')-(s[b]=='}');b+=1
 f=s[a:b]
 assert f.count('StackPromotionMath::preferLocalGroup(')==1
 assert '(localStackFirst && local) || generatePath(' in f
 assert f.index('AI_allowGroup(')<f.index('if (localStackFirst && pBestUnit')
 assert 'StackPromotions::eligible(*this)' in f
 assert 'MISSIONAI_GROUP' in f
cpp=r'''
#include <cassert>
#include <climits>
#include <cstdio>
#include "StackPromotionMath.h"
int main(){using namespace StackPromotionMath;
 unsigned checks=0;
 for(int local=0;local<2;++local)for(int best=0;best<2;++best)
  for(int v=0;v<3000;v+=13)for(int b=0;b<3000;b+=17){
   assert(preferLocalGroup(false,local!=0,best!=0,v,b)==(v<b));
   assert(preferLocalGroup(true,local!=0,best!=0,v,b)==(local!=best?local!=0:v<b));++checks;
  }
 // Reproducer: arrival and adjacent-road travel both reported one path turn.
 // A +5 donor reward used to win at the neighbour, forever ignoring the
 // legal group already at our feet. Presence now outranks that small score.
 assert(preferLocalGroup(true,true,false,2000,1904));
 assert(!preferLocalGroup(true,false,true,1904,2000));
 assert(!preferLocalGroup(false,true,false,2000,1904));
 assert(preferLocalGroup(false,false,true,1904,2000));
 // Preserve first-on-tie order and ranking among local groups. A stronger
 // remote candidate is still available when no legal local group exists.
 assert(!preferLocalGroup(true,true,true,1000,1000));
 assert(preferLocalGroup(true,true,true,900,1000));
 assert(preferLocalGroup(true,false,false,900,1000));
 assert(preferLocalGroup(true,false,false,2000,INT_MAX));
 printf("PASS %u candidate comparisons, option OFF equivalence, exact Aid bounce reproducer and local/remote ties; both production callers verified\n",checks);
}
'''
(P/'group-choice.cpp').write_text(cpp)
sdk=W/'Build/deps/Microsoft Visual C++ Toolkit 2003';env=dict(os.environ);env['PATH']=str(sdk/'bin')+os.pathsep+env['PATH']
subprocess.run([str(sdk/'bin/cl.exe'),'/nologo','/O2','/MD','/EHsc','/I'+str(sdk/'include'),'/I'+str(R/'Sources'),'/Fegroup-choice.exe','group-choice.cpp','/link','/LIBPATH:'+str(sdk/'lib')],cwd=P,env=env,check=True)
r=subprocess.check_output([str(P/'group-choice.exe')],cwd=P,env=env,text=True);print(r);(P/'group-choice-result.txt').write_text(r)
