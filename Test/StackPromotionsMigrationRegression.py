"""Compile actual load detection, popup eligibility, launcher and Yes/No handler.

No game launch or player saves. XML and lifecycle wiring are checked separately.
"""
from pathlib import Path
import json
import os
import re
import subprocess
import xml.etree.ElementTree as E

R = Path(__file__).resolve().parents[1]
W = R if (R / 'Build/deps').exists() else R.parents[1]
P = Path(os.environ.get('C2C_STACK_TEST_OUTPUT', str(W / 'Build/StackPromotionsMigration_2026-10-26/internal')))
P.mkdir(parents=True, exist_ok=True)


def source(path):
    return (R / path).read_bytes().decode('latin1').replace('\r\n', '\n')


def block(text, start):
    a = text.index('{', start)
    b = a + 1
    depth = 1
    while depth:
        depth += (text[b] == '{') - (text[b] == '}')
        b += 1
    return text[start:b]


def function(text, signature):
    return block(text, text.index(signature))


init = source('Sources/CvInitCore.cpp')
popup = source('Sources/CvDLLButtonPopup.cpp')
game = source('Sources/CvGame.cpp')
read = function(init, 'void CvInitCore::read(FDataStreamBase*')
assert read.index('WRAPPER_READ_CLASS_ARRAY_ALLOW_MISSING') < read.index('readStackPromotionsOption(wrapper);')
assert 'm_bStackPromotionsMissingFromSave = false;' in function(init, 'void CvInitCore::resetGame()')
assert 'm_bStackPromotionsMissingFromSave = pSource->m_bStackPromotionsMissingFromSave;' in function(init, 'void CvInitCore::resetGame(CvInitCore *')
assert 'm_bStackPromotionsMissingFromSave' not in function(init, 'void CvInitCore::write(')
loaded = function(game, 'void CvGame::onFinalInitialized(')
new_game = block(loaded, loaded.index('if (bNewGame)'))
assert 'checkStackPromotionsOption' not in new_game
else_part = block(loaded, loaded.index('else', loaded.index(new_game) + len(new_game)))
assert 'checkVersions();' in else_part and 'checkStackPromotionsOption();' in else_part
assert game.count('checkStackPromotionsOption();') == 1
assert popup.count('case BUTTONPOPUP_STACK_PROMOTIONS_MIGRATION:') == 2
launch_case = popup.split('case BUTTONPOPUP_STACK_PROMOTIONS_MIGRATION:', 2)[2]
assert 'return launchStackPromotionsMigrationPopup(pPopup, info);' in block(launch_case, 0)
handler = block(popup, popup.index('case BUTTONPOPUP_STACK_PROMOTIONS_MIGRATION:'))
assert 'recalculate' not in handler.lower()
header = source('Sources/CvDLLButtonPopup.h')
assert 'bool launchStackPromotionsMigrationPopup(CvPopup* pPopup, CvPopupInfo &info);' in header
enums = source('Sources/CvEnums.h')
old_enums = subprocess.check_output(['git', 'show', 'b2964d75b:Sources/CvEnums.h'], cwd=R).decode('latin1')
def popup_ids(text):
    return re.findall(r'^\s*(BUTTONPOPUP_\w+)', function(text, 'enum ButtonPopupTypes'), re.M)
assert popup_ids(enums) == popup_ids(old_enums) + ['BUTTONPOPUP_STACK_PROMOTIONS_MIGRATION']
texts = E.parse(R / 'Assets/XML/GameText/StackPromotions_CIV4GameText.xml').getroot()
for e in texts.iter():
    e.tag = e.tag.split('}')[-1]
for key in ('TXT_KEY_POPUP_STACK_PROMOTIONS_MIGRATION', 'TXT_KEY_POPUP_STACK_PROMOTIONS_MIGRATION_BODY'):
    matches = [e for e in texts if e.findtext('Tag') == key]
    assert len(matches) == 1
    for lang in ('English', 'French', 'German', 'Italian', 'Polish', 'Russian'):
        assert matches[0].findtext(lang)

cpp = r'''
#include <cassert>
#include <cstdio>
#include <vector>
#include <string>
#include <cstddef>
typedef int PlayerTypes;
const int NO_PLAYER=-1, GAMEOPTION_COMBAT_STACK_PROMOTIONS=10;
const int REMAPPED_CLASS_TYPE_GAMEOPTIONS=20, BUTTONPOPUP_STACK_PROMOTIONS_MIGRATION=30;
const int InfoPane_DIRTY_BIT=40, SelectionButtons_DIRTY_BIT=41;
const int WIDGET_GENERAL=50, POPUPSTATE_IMMEDIATE=60;
struct Player {bool alive,human;bool isAlive()const{return alive;}bool isHumanPlayer()const{return human;}} players[1];
Player& getPlayer(int id){assert(id==0);return players[id];}
#define GET_PLAYER(id) getPlayer(id)
struct Game {
 bool network,option;int player,writes;
 bool isNetworkMultiPlayer()const{return network;}
 int getActivePlayer()const{return player;}
 bool isOption(int id)const{assert(id==GAMEOPTION_COMBAT_STACK_PROMOTIONS);return option;}
 void setOption(int id,bool value){assert(id==GAMEOPTION_COMBAT_STACK_PROMOTIONS);option=value;++writes;}
} game;
struct Globals {Game& getGame(){return game;}} GC;
struct CvPopup {};
struct CvPopupInfo {int type;CvPopupInfo(int t):type(t){}};
struct PopupReturn {int button;int getButtonClicked()const{return button;}};
struct Interface {
 int queued,launches,dirty;std::vector<int> buttons;std::string title,body;
 void clear(){queued=launches=dirty=0;buttons.clear();title.clear();body.clear();}
 void addPopup(CvPopupInfo* info,int player,bool front,bool immediate){
  assert(info->type==BUTTONPOPUP_STACK_PROMOTIONS_MIGRATION&&player==0&&!front&&immediate);++queued;delete info;
 }
 void setDirty(int bit,bool value){assert(value&&(bit==InfoPane_DIRTY_BIT||bit==SelectionButtons_DIRTY_BIT));++dirty;}
 void popupSetHeaderString(CvPopup*,const char* text){title=text;}
 void popupSetBodyString(CvPopup*,const char* text){body=text;}
 void popupAddGenericButton(CvPopup*,const char*,const char*,int id,int widget){assert(widget==WIDGET_GENERAL);buttons.push_back(id);}
 void popupLaunch(CvPopup*,bool immediate,int state){assert(!immediate&&state==POPUPSTATE_IMMEDIATE);++launches;}
} ui;
struct DLL {Interface* getInterfaceIFace(){return &ui;}const char*getText(const char*key){return key;}} dll,*gDLL=&dll;
namespace StackPromotions {int resets=0;void reset(){++resets;}}
struct CvTaggedSaveFormatWrapper {
 bool tagged;std::vector<int> mapping;int reads;
 bool isUsingTaggedFormat()const{return tagged;}
 int getNumClassEnumValues(int type)const{assert(type==REMAPPED_CLASS_TYPE_GAMEOPTIONS);return (int)mapping.size();}
 int getNewClassEnumValue(int type,int old,bool missing){assert(type==REMAPPED_CLASS_TYPE_GAMEOPTIONS&&missing);++reads;return mapping[old];}
};
struct CvInitCore {
 bool m_bStackPromotionsMissingFromSave;
 bool getOption(int id)const{return game.isOption(id);}
 void setOption(int id,bool value){game.setOption(id,value);}
 void readStackPromotionsOption(CvTaggedSaveFormatWrapper&);
 void checkStackPromotionsOption();
};
struct CvDLLButtonPopup {bool launchStackPromotionsMigrationPopup(CvPopup*,CvPopupInfo&);};
'''
cpp += function(init, 'void CvInitCore::readStackPromotionsOption(') + '\n'
cpp += function(init, 'void CvInitCore::checkStackPromotionsOption(') + '\n'
cpp += function(popup, 'bool CvDLLButtonPopup::launchStackPromotionsMigrationPopup(') + '\n'
cpp += 'void answer(PopupReturn* pPopupReturn){ switch(BUTTONPOPUP_STACK_PROMOTIONS_MIGRATION){' + handler + '} }\n'
cpp += r'''
int main(){
 unsigned checks=0;
 for(int tagged=0;tagged<2;++tagged)for(int count=0;count<=6;++count)
 for(int position=-1;position<count;++position)for(int savedOn=0;savedOn<2;++savedOn)
 for(int network=0;network<2;++network)for(int player=-1;player<=0;++player)
 for(int alive=0;alive<2;++alive)for(int human=0;human<2;++human){
  game.network=network!=0;game.player=player;game.option=savedOn!=0;game.writes=0;
  players[0].alive=alive!=0;players[0].human=human!=0;ui.clear();
  CvTaggedSaveFormatWrapper wrapper;wrapper.tagged=tagged!=0;wrapper.reads=0;
  // Removed/unknown entries are harmless, as is reordering the saved option.
  wrapper.mapping.assign(count,-1);if(position>=0)wrapper.mapping[position]=GAMEOPTION_COMBAT_STACK_PROMOTIONS;
  CvInitCore core;core.m_bStackPromotionsMissingFromSave=true;core.readStackPromotionsOption(wrapper);
  const bool legacy=tagged&&position<0;
  assert(core.m_bStackPromotionsMissingFromSave==legacy);
  assert(game.option==(legacy?false:savedOn!=0));assert(game.writes==(legacy?1:0));
  if(!tagged)assert(wrapper.reads==0);
  const bool offer=legacy&&!network&&player!=NO_PLAYER&&alive&&human;
  core.checkStackPromotionsOption();assert(ui.queued==(offer?1:0));
  core.checkStackPromotionsOption();assert(ui.queued==(offer?1:0));++checks;
 }
 CvPopup p;CvPopupInfo info(BUTTONPOPUP_STACK_PROMOTIONS_MIGRATION);CvDLLButtonPopup popup;
 for(int network=0;network<2;++network)for(int on=0;on<2;++on){
  game.network=network!=0;game.option=on!=0;ui.clear();
  bool shown=popup.launchStackPromotionsMigrationPopup(&p,info);
  assert(shown==(!network&&!on));
  if(shown){assert(ui.launches==1&&ui.buttons.size()==2&&ui.buttons[0]==1&&ui.buttons[1]==0);
   assert(ui.title=="TXT_KEY_POPUP_STACK_PROMOTIONS_MIGRATION");
   assert(ui.body=="TXT_KEY_POPUP_STACK_PROMOTIONS_MIGRATION_BODY");}
  else assert(ui.launches==0&&ui.buttons.empty());++checks;
 }
 for(int network=0;network<2;++network)for(int on=0;on<2;++on)for(int clicked=-1;clicked<=2;++clicked){
  game.network=network!=0;game.option=on!=0;game.writes=0;StackPromotions::resets=0;ui.clear();
  PopupReturn result;result.button=clicked;answer(&result);
  bool accept=clicked==1&&!network;
  assert(game.option==(accept||on));assert(game.writes==(accept?1:0));
  assert(StackPromotions::resets==(accept?1:0));assert(ui.dirty==(accept?2:0));++checks;
 }
 // Saving either answer records the option name. On a subsequent read, OFF
 // remains OFF without another invitation; ON stays ON. No new save tag.
 for(int accepted=0;accepted<2;++accepted){
  game.option=accepted!=0;CvTaggedSaveFormatWrapper wrapper;wrapper.tagged=true;wrapper.reads=0;
  wrapper.mapping.push_back(GAMEOPTION_COMBAT_STACK_PROMOTIONS);
  CvInitCore core;core.m_bStackPromotionsMissingFromSave=true;core.readStackPromotionsOption(wrapper);
  assert(!core.m_bStackPromotionsMissingFromSave&&game.option==(accepted!=0));++checks;
 }
 printf("PASS %u actual native migration cases: legacy detection, saved OFF/ON, remapping, player eligibility, duplicate prevention, Yes/No/close, UI and cache reset\n",checks);
}
'''
(P / 'migration.cpp').write_text(cpp)
sdk = W / 'Build/deps/Microsoft Visual C++ Toolkit 2003'
env = dict(os.environ)
env['PATH'] = str(sdk / 'bin') + os.pathsep + env['PATH']
subprocess.run([str(sdk / 'bin/cl.exe'), '/nologo', '/O2', '/MD', '/EHsc', '/I' + str(sdk / 'include'), '/Femigration.exe', 'migration.cpp', '/link', '/LIBPATH:' + str(sdk / 'lib')], cwd=P, env=env, check=True)
result = subprocess.check_output([str(P / 'migration.exe')], cwd=P, env=env, text=True)
print(result)
(P / 'migration-result.txt').write_text(result)
(P / 'migration-result.json').write_text(json.dumps(dict(passed=True, actualNativeFunctions=True, xml=True, lifecycleWiring=True, oldPopupIdsPreserved=True, gameRun=False), indent=2))
