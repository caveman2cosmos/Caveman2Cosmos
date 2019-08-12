[B]RevolutionDCM mod download:[/B]
[url]http://forums.civfanatics.com/downloads.php?do=file&id=8322[/url]

[B]RevolutionDCM mod sources download:[/B]
Select the "RevDCM Source" optional add on when installing RevDCM, the source files will be installed into the main mod's folder

[B]RevolutionDCM SVN SourceForge home:[/B]
https://revolutiondcm.svn.sourceforge.net/svnroot/revolutiondcm/Trunk/RevolutionDCM/
https://revolutiondcm.svn.sourceforge.net/svnroot/revolutiondcm/Trunk/SourceCode/


[B]Current Version: 2.71[/B] 

[B]Authors[/B]
-------
Jdog5000, Glider1, phungus420

[B]Concept:[/B]
--------
To integrate high quality mods onto the back of the brilliant Revolutions mod that add strategic richness to BTS, compliment Revolutions and yet still make for a compact download size. At all times basic BTS game experience and stability is it's reference guiding development. 

Any combination of mods within RevolutionDCM are controllable in a user interface. Any mod can be turned on or off as desired within this interface. In this way, RevolutionDCM can be hypothetically reduced down to a base BTS 3.19 core running Better BTS and BUG. RevolutionDCM uses the Better BTS AI to incorporate these high quality mods:
Revolutions 			(SDK, Python, XML)
Dales Combat Mod 		(SDK, XML)
Influence Driven War		(SDK, XML)
Revolution Inquisitions 	(SDK, XML)
Super Spies 			(SDK, XML)
BUG, BULL Mod 			(SDK, Python, XML)
WoC Mod 			(SDK, Python, XML)
Better BTS AI 			(SDK, Python)
RevolutionDCM is now capable of recognising and installing WoC modules, so long as they have been converted over to the RevolutionDCM schema and do not conflict with the mods already present in RevolutionDCM.

[B]Forum:[/B]
-------
[url]http://forums.civfanatics.com/showthread.php?t=262937[/url]

[B]Acknowledgements:[/B] 
Big thanks to:
Jdog5000 and Dom Pedro II for Revolutions:
[url]http://forums.civfanatics.com/showthread.php?t=171127[/url]
 
Dale for DCM (uniquely ported to RevDCM):
[url]http://forums.civfanatics.com/showthread.php?t=257210[/url]

Moctezuma for IDW (uniquely ported to RevDCM):
[url]http://forums.civfanatics.com/showthread.php?t=188007[/url]

Solver, Dresdon and Jdog5000 for the BTS unofficial patch:
[url]http://forums.civfanatics.com/showthread.php?t=246057[/url]

Better BTS AI Team and Jdog5000:
[url]http://forums.civfanatics.com/showthread.php?t=280857[/url]

Bug Mod Team and EmperorFool (uniquely ported to RevDCM):
[url]http://forums.civfanatics.com/forumdisplay.php?f=268[/url]

Trojan Sheep for Super Spies (uniquely ported to RevDCM):
[url]http://forums.civfanatics.com/showthread.php?t=259221[/url]

WoC Mod Team (uniquely ported to RevDCM):
[url]http://forums.civfanatics.com/forumdisplay.php?f=276[/url]

These people are some seriously brilliant modders and thinkers. 

Also thanks for these important contributions as well:
Phungus420 for ideas, coding, testing and forum assistance
Johny Smith for his high woc-lite concept
Faichele for his help with the woc code
Lord Tirian for super spies updates
PieceOfMind for the Advanced Combat Mod!
Smeagolheart for Examine City at Conquest mod
OrionVeteran for some core algorithms in Revolutions Inquisitions
Cephalo for the PerfectWorld map!
Thomas SG for the "Positions of Scores" scrollbar
Minor Annoyance for Global Warming Mod
Kael for Barbarian World game option
DaveMcW for Show Hidden Attitude Mod
Phungus420, Afforess, Duuk for excellent beta testing
Mamba for WoC contributions like java utilities and forum help

If I forgot anyone, sorry!


[B]Installation:[/B]
-------------
Simply double click the installer to install

[B]WoC Modules:[/B]
-----------------
WoC module forum: [url]http://forums.civfanatics.com/showthread.php?t=320428[/url]


[B]Operation:[/B]
----------
The only change in the user interface from BUG is that all RevolutionDCM game options are accessed via
The BUG user interface option control system. The user presses the standard key sequence that BUG uses
CTRL-ALT-O to access this option control and navigates to the RevDCM tab, next to the General options
tab. To see which options have been enabled as secondary feedback to the option screen itself,
the user presses CTRL-SHIFT-Q which brings up the game configuration popup. WoC modules that do not
conflict with the RevolutionDCM core mods, are dropped into the modules folder and removed when finished. 
Modular loading does not need to be enabled in the ini files and custom assets have been disabled by
default so as not to conflict with woc installations (can be re-enabled in RevolutionDCM.ini)

Modders:
--------
General options are no longer controlled by editing GlobalDefinesAlt.xml. There are still detailed game
state variables in that file that can be edited manually, so long as those state variables are not being
controlled by the user interface. Option control follows this flow:
1) RevolutionDCM.ini is checked for option state
2) RevDCM.xml stores default option states
3) RevDCMOptionsTab.py is the user interface code for options
4) RevDCM.py is the option handler for python option states and SDK options states. This code will modify 
	GlobalDefinesAlt.xml automatically.

WoC Module Conversion:
----------------------
WoC modules allows for a non programming method to add a simple mod containing XML definitions and art, without requiring programming effort and without having to alter the mod's own core XML. There are some "rules" that modders should follow to convert WoC modules to RevolutionDCM:
1) Ensure that all the schema xml in the module has at least the same schema definitions that are in the RevolutionDCM schema files found under assets/xml/etc etc. 
2) The two default MLF loading control files in assets/modules, define a default directory structure for modules and is equivalent to the WoC definition. It may still undergo revisions to the structure if modders do not like it.
3) If the default MLF loading control file system is used, loading control files must also be used in the modules subdirectory as well.
4) Modules can work without MLF loading control definitions (nothing but folders with xml), but this will limit the ability to add and combine other modules into your installation.

Other documentation:
--------------------
For a more detailed version log with extra information, see RevolutionDCM_readme.txt
For a more detailed version log for Revolutions, see /docs/Revolution/*
For a merge guide useful for tips on how RevolutionDCM was merged, see /docs/RevolutionDCM_merge.txt

Known Issues:
-------------
General:
-

Multiplayer mode:
- Revolutions multiplayer issues in the latest build:
	Here are the test conditions:
	1) Direct IP
	2) Barb Civ off (may not need to be)
	3) Barbarian World off' (may not need to be)
	4) Start as minors off (may not need to be)
	5) Revolutions on
	6) Dynamic civ on
	7) Tech diffusion on

	This should work:
	1) A player chooses to recognise independence
	2) A player chooses to reject terms
	3) A player chooses to bribe
	4) AI suffers revolts of any type

	This is what will cause OOS (If OOS, just get the host to reload the last save game):
	1) A player dies
	2) A player chooses accept and lead the rebels
	3) A player chooses to hand over control (LeaderRevolution and HumanLeaderRevolution could be turned off in Revolution.ini for MP games)

IDW:
-

Spies:
- Spies get to move after they have returned from successful espionage if they are given a logistics promotion immediately upon their return.

DCM:
- Stack attack interface control can be lost but re-enabled by clicking on the minimap. Still can possibly CTD although very uncommon.
- Any DCM option other than ranged bombardment and battle effects, have not been stability or game balance tested in a long time. There have been no reports of issues on the forum so the assumption is that all is ok.
- Archer bombard behaviour not changed from DCM 1.7 despite DCM hint that it has.
- Bomb shelters should require Manhattan national wonder to be compatible with the vanilla BTS Manhattan project but only require Fission.
- Apparent loss of movement bonus in battle effected terrain (reported Avain).

Revolutions:
-

Inquisitions:
-

General BTS:
- The standard BTS collateral damage model will inflict ZERO damage to a stack consisting of any number of seige accompanied by a single unit of any other type, despite reporting successful collateral attack to the player.

WoC:
- WoC xml with white space in file name causes problems with BUG 4.1
- Missing MLF control files will stop module from loading at all as of version 2.51. This is not correct behaviour. Missing MLF files should simply induce a random load order disregarding dependencies.

Random Events:
- The "overwhelm doctrine" has been disabled for the sake of DCM compatibility with the Manhattan national wonder.

Interface:
- Revolution Graph in the info screen should not display if the Revolutions mod is off.
- Alignment of the revolution watch advisor numbers are not centred in the column space.

Maps:
-

Scenarios:
- The RevolutionDCM mod is not compatible with some scenario creations as reported by Johny Smith possibly because of DCM Battle Effect code.


Build notes:
------------
v1.431
	- Fix to reported XML error in feature growth.  Updated installer in the SVN.
	- Add Audio3DScripts needed otherwise the WoC audio installer application crashes
	- Another BUG/RevDCM interrelationship bug fix for CvExoticForeignAdvisor change screen.
	- Small bug with reporting the Inquisitions option
	- Add the install script code and instructions under docs/utils/
	- Turn off Revolution debug info by default


v1.412
	- Updated to BUG 4.3 & BULL 1.1
	- Update to BBAI 0.84 & UP 1.4
	- Added Scenario Modcomp that allows loading of BtS scenarios that have less defined players then the dll (default BtS scenarios now playable with RevDCM)  -Thanks Tamudjin!
	- Added Afforess's Python callbacks (should improve performance)
	- Added many, many translations; RevDCM should be nearly entirely translated in German, and well polished in Italian (Thanks Snophru & Gaplus)
	- Swapped out old Civic Screen with RoM's Civic Screen, removes needed SDK functions and is much simpler
	- Normalized Loading of Revolution.ini with other BUG components (fixes user specific loss of interface bug)
	- Movies defined for any building now work correctly. Designed with big mods in mind:  Wonder splash only occurs for buildings that have a movie defined, and World Wonders.  -updated to be a BUG module (thanks Lemon Merchant)
	- Packed art into fpk
	- Added HasMet check to Worst Enemy trading penalty
	- AI razing decisions greatly improved and AI takes into account the effects of IDW; it may now raze cities with IDW enabled
	- Added new value to the GlobalDefinesAlt file "IDW_CITY_TILE_MULTIPLIER", which allows changing the effects of IDW on city tiles; default is 0.2 which means that in cities IDW effect is 1/5th of normal
	- Updated Credits screen in the BUG options tab to reflect this is RevDCM, and not BUG
	- Fixed bug with tech diffusion and AI's trade denials (thanks Afforess)
	- Fixed Python Error "newCividx is not defined"
	- Removed WoC UnitResource Icons
	- Improved Tech grant for Barb civs, new World Barbarian civs shouldn't be so advanced
	- Changed all inquisitor code to a Softcoded format handled in the XML (see UnitInfos for relevant tags).  Fixed many bugs in Inquisitions code.
	- Inquisitions moved entirely into the SDK (thanks Afforess)
	- Multiplayer code from the RevolutionMP project ported over, MP is still somewhat buggy and may not connect properly for all users, but should at least "work" in general
		*Direct IP connections should work for most players (but some players are unable to connect for some unkown reason), Gamespy connections have not been tested
		*Players must ensure that they have set the same options in the RevDCM options tab, any different options selected in this tab will cause OOS issues
		*BarbarianCiv, BarbarianWorld, Dynamic Civ Names, Tech diffusion, and Start as Minors options have been tested and seem to function without issue
		*Revolutions will work for the most part, however if a player chooses to switch leaders or take control of Rebels due to a Revolution pop up, an instantanious OOS will occur
		*When a human player dies the game will go OOS; all living players will need to reload the game and the dead player must drop to correct
	- Wrote install script for RevDCM
		*Allows easy installation for all users, and insures correct intallation
		*Elinates need for "launch as admin", and elimnates some rare user specific crash to desktops
		*automatically tests for 3.19 patch
		*allows creation of easy to use shortcuts for easy accessibility to the mod
	- Created New Installer options, moved a few RevDCM default components into being optional
		*Dale's Nuclear changes, such as the chain reactor and atomic bomber; brings RevDCM in line with default BtS
		*Better Ship Scale WoC module; makes it so launching as admin is not necessary with new installer, since WoC modules require admin privlages to work correctly
		*Choose Religions; multiple confirmed bug reports caused by conflicts with the Pick Religions gameoption, and this python component
	- Added a few XML tags to make things easier on mod modders and necessary for new inquisitions code to work
		*FeatureInfos:
			GrowthSound:  allows modders to set a specific sound for feature growth, rather then always getting the forest sound
		*CivicInfos:
			bUpgradeAnywhere:  allows units to upgrade outside national borders (Cloned from TraitInfos, introduced in previous RevDCM version)
			bAllowInquisitions:  Defines a civic as enabling inquisitions
			bDisallowInquisitions:  Defines a civic as disabling inquisitions (overrides all other conditions)
		*UnitInfos:
			bForceObsoleteTech: makes a unit untrainable when a defined tech is learned
			bInquisitor: Unit can run inquisitions missions if inquisitions conditions are met
			bStateReligion:  State relgion required in city to train unit
			PrereqGameOption:  If set, gameoption required to be enabled in the game for the unit to be trained
			NotGameOption:  If set, the unit may not be trained if the specified gameoption is selected
			PrereqOrCivics:  If set, the unit my only be trained if the player is running one of the specified civics
			PrereqBuildingClasses:  If set, the unit may only be trained if specified buildingclasses have been built in the city




v1.277
	- Updated to Better BTS AI 0.82J and Unofficial Patch 1.31 (Thanks Jdog5000!)
	- Fixed issue with combat odds display (Thanks Jdog5000)
	- Barbarian Civ settling now scales better with game speed (Thanks Jdog5000!)
	- Fixed a couple bugs in RevInquisition (Thanks Glider1)
	- Fixed a couple issues with BUG and RevDCM (Thanks Glider1 and Jdog5000)
	- Updated to BUG 4.2 (Thanks Glider1)
	- Updated to BULL 1.00 official (Thanks Glider1)
	- Fixed project interdependency bug (Thanks DavidAllen and Xienwolf!!!)
	- Fix to choose religion option (Thanks OrionVeteran)
	- Fixed plot combat help issue, now shows modifiers correctly (Thanks Jdog5000)
	- Added Rom's extra inquisitor checking code (Thanks Zappara)
	- Allow the sevopedia to show multiple civs if the leader is set to be a leader for multiple civs (Thanks Phungus420)
	- Fix bug in the RevDCM info screen where RevDCM was returning -1 for a playerID (Thanks Phungus420)
	- Formatting changes for code comments (Thanks Jdog5000)
	- modified makefile for BULL compiler options (Thanks Glider1)
	- New organization of GlobalAltDefines.xml for BBAI 0.82 options (Thanks Phungus420)
	- integer division issues fixed in Revolutions calculations (Thanks Jdog5000!)
	- Python and XML changes from UP and BBAI (Thanks Jdog5000!)
	- Remove duplicate xml tag, to fix "give up their independence" bug (Thanks Jdog5000!)
	- "Financial stability" text displayed when revolution watch finances are positive (Thanks Glider1)
	- Updated to maps to Perfectworld 2.05 (Thanks Cephalo!)
	- Added Duuk's private map scenarios customized for RevDCM (Thanks Duuk)


v1.242
	- Updated to Better BTS AI 0.81M and the latest Unofficial Patch (thanks Jdog5000)
	- AI will start a new war if it has a huge power advantage in its existing wars (thanks Jdog5000)
	- Adds rev index info to city bar text on the main map (thanks Glider1)
	- Super spies AI promotion values adjusted to discourage promotions the AI is unlikely to benefit from, and the AI will now use logistics (Thanks Glider1)
	- Remove need for Python callbacks to speed turn times except for the inquisitor unit (thanks Jdog5000)
	- Updated to BUG 4.1, and BULL 1.0 (Thanks EmperorFool, Glider1)
	- Add Barbarian World, Rev Traits, Civilopedia civics effects, etc (Thanks Jdog5000, Phungus420 and Kael!)
	- Interdependent random event fix woc (thanks faichele)
	- Added the Global Warming mod by Minor Annoyance (thanks Jdog5000)
	- Added missing revolutions copynondefaults in CvCivicInfo (thanks Afforess)
	- Show Hidden Attitude mod merged and made a controllable interface option (thanks DaveMcW)
	- The AI now is more Revolution aware and will keep its cities happier to avoid revolts (thanks Jdog5000)
	- Revolution Civics effects now show in the Civilopedia (thanks Phungus420)
	- Small civics modifier fix (thanks Jdog5000, Phungus420)
	- domestic advisor liberate city fix (thanks Jdog5000)
	- Re-balanced BarbarianCiv for early barb settling as well as Barbarian World option (thanks Jdog5000)
	- Converted four Earth maps to Revolution DCM compatibility (thanks Jdog5000)
	- Fixed issue with "X have chosen to give up their independence" text (thanks Jdog5000)
	- RevolutionDCM options can no longer be changed in a multiplayer game session (thanks Glider1)
	- RevolutionDCM options from the options screen now update immediately (thanks Glider1)
	- RevolutionDCM "debug" option becomes obsolete (thanks Glider1)
	- Revolution watch advisor always shows national effects even if city is not selected (thanks Glider1)
	- Revolution national effects now show in the city screen rev bar mouseover (thanks Glider1)
	- Revolution index displays in players city bar on the main map when mousing over (thanks Glider1)
	- Religion control code (limited and choose religion) that was in CvGameUtils converted to BUG game utils format and retested (thanks Glider1)	
	- Revolution inquisition code ported to BUG 4.0 standard plus a few small adjustments including a bug fix for inquisition gold rewards for non revolution mod players, which have been brought more in line with OrionVeteran's Inquisition mod. (thanks Glider1)
	- Fixed inquisitor motion AI which would sometimes lead to asserts/slowness (thanks Jdog5000)
	- Various BUG 4.1 fixes to civlerts and autolog to work with Revolutions (thanks Jdog5000, Glider1)
	- Prototype automated language merger add to doc/utils/text_installer. (thanks Glider1)
	- Added in new German translation (thanks Snofru1)
	- Finnish Translations from LoR (thanks Phungus)
	- Spannish Translations (thanks Chechu)
	- documentation updated (thanks Glider1)
	- Disabled overwhelm doctrine (thanks Glider1)

v1.132
	- Fixed long standing missing Naval bombard mission in the SDK AI code due to DCM merge that reduced the AI's range of bombard behaviour (thanks Arian good find!)
	- Disabled defective events (8 out of 300) by resetting their weights to zero, until the event independency issue is sorted:
		EVENTTRIGGER_CITY_RUINS_ANCIENT
		EVENTTRIGGER_SLAVE_REVOLT (just annoyingly problematic)
		EVENTTRIGGER_AIRLINER_CRASH
		EVENTTRIGGER_HORTICULTURE
		EVENTTRIGGER_FUGITIVE
		EVENTTRIGGER_LOOTERS
		EVENTTRIGGER_HEALING_PLANT
		EVENTTRIGGER_DEFECTING_AGENT
	- Fixed DCM battle effects loss of terrain bonuses (thanks Hephaistion).
	- Fixed missing popup import in RevEvents (reported Phungus)
	- Fixed Revolution.ini typo (reported Afforess)
	- "Better Ship Scale v1.0 for BTS" as a woc module to improve naval unit scale visuals (Delete the module if not needed).
	- "TXT_KEY_REV_AND" bug fixed. Was not an algorithm error, but simply a quirk of python itself in TextUtils.py.
	- "TXT_KEY_MISC_SHIFT_ALT_PREPARE_WAR" text fixed (reported Ripple01)
	- 52x32 earth map for 50 civs (Craterus22)

v1.125
	- Updated to BTS 3.19 (Thanks Jdog5000!)
	- Updated Better BTS AI and docs to 0.78c (Thanks Jdog5000!)
	- Revolutions mod bug fixes - see Revolution docs (Thanks Jdog5000!)
	- Bug fix to barbarian civ component - see Revolution docs (Thanks Jdog5000)
	- Fixed rare potential crash in IDW (Thanks Jdog5000)
	- Revolutions new city distance calculations - see Revolution docs (Thanks Jdog5000/Phungus420)
	- Changed financial/tax calculations that determine rev index and civ stability - see Revolution docs (Thanks Jdog5000/Phungus420)
	- If in rare situations the AI manages to build more than the XML limit of inquisitors(3), extra AI inquisitors will kill themselves when the AI switches out of inquisitions conditions (reported Zappara)
	- Ranged bombard help text updated and tested (reported Nudden). NOTE: The standard BTS collateral damage model will inflict ZERO damage to a stack consisting of any number of seige accompanied by a single unit of any other type, despite reporting successful collateral attack to the player. This is an issue within the standard BTS collateral code.
	- Bomb shelter bug since DCM 1.7 fixed. Previously could not build them since DCM changed the Manhattan project to a national wonder.  Now bomb shelters require only Fission tech which lines them up with vanilla BTS but without the requirement for Manhattan. The dependency of bomb shelters on the Manhattan national wonder has not yet been reinstated. Downside is that the AI may build bomb shelters even if nuclear war is not possible. (Thanks Zappara for reporting)
	- The "overwhelm doctrine" has been disabled for the sake of DCM compatibility with the Manhattan national wonder. This random event has been disabled in the past, but re-enabled by merge error since version 1.00.
	- Updated Perfect World map to version 2.03 (thanks Cephalo!)
	- Python code in-place already since 1.110 to accept the WoC Lite interface module which adds a BUG options icon to the main interface removing the need to execute the keyboard stroke CTRL-ALT-O.

v1.110
	- Fixed an asserts error to naval bombardment reported by Phungus420 (thanks Jdog5000)
	- Fixed the loss of random events which occured in the SDK at release 2.00 (thanks Johny Smith).
	- Rearranged the main interface buttons to fit the Revolution info button in-line.
	- Merged in Thomas SG's scoreboard scroller (thanks!)
	- Update to Better BTS AI 0.75f
	- Addition of /assets/modules/woc_installer.jar application for installing optional "extra" woc functionality like audio. Removes the need to do manual merging for woc modules and restores the default installation xml to core RevolutionDCM. Consult the woc_installer readme under assets/modules for more information. The installer does not have to be used for basic woc functionality.
	- Update to game fonts (thanks to Johny Smith)
	- German spy text translations (thanks to Chips2150)

v1.100:
	- WoC Extra Lite which is a cutdown version of WoC Lite. A huge thanks to Johny Smith for his skill and contributions.
	- Better BTS AI 0.70H
	- Perfect World 2.01 converted for RevDCM and better corp balance. For the best experience of this map, generate a map with default settings and observe the PythonDbg.log file before you start your perfect world game (so long as logging is enabled). Old world starts are ok but there has been reports that with a greater than default number of civs, some civs get terrible starting locations. The debug log entries of interest for the best maps are when the ratio of wheat to sugar is >=1.0 and sea to corn+wheat/sugar ratios between 1.0 and 2.5. To ensure that the map has generated at least two continents and some islands or smaller continents (without looking at the map in worldbuilder), ensure that the number of starting areas is >= 3 but preferably >=4.

v1.096:
	- Inquisitions now works without needing the Revolutions mod both for the player and the AI. Behaviour is similar except that a partial implementation of OrionVeteran's Inquisitions has been implemented to replace revolution effects. The gold surplus accrues +100 gold for each non-state religious building destroyed in an Inquisition as per OrionVeteran, but there is also an associated risk of the oppressed citizenry becoming unhappy if the inquisition fails. Feel free to suggest any simple balance adjustments that could be made to Inquisitions, as the non-revolutions variant has not undergone much game balance testing except for OrionVeteran's Inquisitions concept having now stabilised.
	
v1.092:
	- Airbomb plot bug fix in DCM, where without DCM airbombing enabled, aircraft could bomb plots with zero success. Thanks GatlingGun for reporting this bug and supplying save game.
	- Smeagolheart's Examine City at Conquest mod added
	- Updated to Advanced Combat Odds 1.0

v1.087: 
	- Update Perfectworld2_RevDCM for better corps balance and region variety

v1.084:
	- Updates to BarbarianCiv and Revolution
		Improved stack formation for counter and attack UNITAIs shortly after BarbCiv or rebel civ is formed
		Notification of settling barb civs updated so you will always be notified of nearby settling
		Modified number of attack units given to forming minor barbcivs, generally fewer units for early civs, unit number scales by city size, plus added a random multiplier so some get more units than others
		If a settling militaristic barbciv has a current unique unit, type of militaristic settling is now biased so they're more likely to get lots of those units
		Adjusted unit AI types given to rebel units so they will more readily form attack stacks
		Rebel units spawning alone and with little chance of capturing a city may now run pillage unit AI (which groups with ATTACK_CITY when available)
		Large groups of barbarian rebels will now attack cities more directly using ATTACK_CITY_LEMMING
	- Some AI updates

v1.081:
	- Introduction of a new diplomacy mode "Cease bothering us!" for single player mode only, and only issued from the human player to the AI:
		Player can ask the AI to cease contacting them. If the AI wants to contact the human player but has been told not to, the AI will behave as if the human player had rejected whatever request the AI was intending to make. This may either be no change in attitude, a change in attitude or perhaps an unexpected war declaration as well! All war declaration diplomacy contact and first contact meetings are not affected by this option.  The AI behaviour is 100% BTS behaviour equivalent and so there is no change in the AI behaviour. The change is actually only an interface enhancement useful to the human player when there are a lot of insignificant AI's about that the human player does not want to communicate with. The do not bother status is not saved in the game state, so reloading or restarting will reset all do not bother status to back to vanilla contact behaviour. This is good, because there is a high likelihood that players will forget that they have told the AI to shut up between gaming sessions, and thus are wide open to unexpected surprises in diplomacy.
	- Introduction of a modified perfect world map script customised for RevolutionDCM
	- Bug fix that allowed the DCM AI to interact with the Better BTS AI if DCM options turned off.
	- Autolog python error fixes
	- Inquisition bug fix in a small number of cases

v1.073:
	- Perfect World Map Script added (thanks Cephalo)

v1.072:
	- Incorporated Advanced Combat Odds 0.4 (thanks Phungus420 for helping making it enable/disable ready)
	- Small text changes to the RevDCM options screen

v1.071:
	- Updated to BUG 3.6
	- Following maps 50 civ aware:
		Team battleground, Ring world, Tectonics
	- Following maps not needing 50 civ aware:
		Maze, Lows Random, Planet Gen, SmartMap
	- "isColor" bug fixed
	- Various small fixes

v1.068:
	- Range bombard AI introduced. The AI now captures cities more readily and is pretty much at the strength of Better BTS. However the AI is not aware of range bombard exploits that a human will devise over time. Please keep range bombard scenarios coming, by posting saves to the RevDCM forum so that the range bombard AI can be continually improved.
	- Field bombard seperated from naval bombardment as two seperate options independent of each other. The naval AI is no different from DCM 1.7 and has not been updated or improved.

v1.065:
	- One hundred new hints relevant to RevolutionDCM and it's components added.
	- Various game changes to ranged bombard:
		Range bombard now only inflicts standard collateral damage. Thus it is useless against single units or large stacks. The head defender is not injured, and damage is capped as per standard collateral damage. This reduces the seige stack of doom tactic and replicates the BTS model of field conflict where ranged bombardment tends to hit the main throng of the opposing army, not the front line and not the seige at the rear.
		Range bombarding cities also has a slight chance of hitting defenders not just defenses.
		Seige can standard bombard cities as well as range bombard, for more rapid dropping of city defenses.
		Range bombarding from two tiles out, does not reduce city defenses as much as from one tile out.
		Range bombarding from inside cities is much less effective than from properly constructed field structures like forts.
		Range bombarding units from two tiles out has a bit more chance of missing altogether, than from one tile out.
		Range bombard strength now customisable in GlobalDefinesAlt.xml.

v1.058:
	- Bribe button updates the button's origin screen either the advisor or the city screen.
	- Revolution Watch Advisor shortcut CTRL-SHIFT-G and improvements to the legend overview.
	- Stack attack option seperated into two options. "Stack attack" and "attack support". This allows stack attack to be used to automate and animate stack attacks, without changing the underlying battle mechanics.
	- Attack support option introduced. The option is no different to the implementation in DCM which was formely incorporated in "stack attack". This option allows range bombard seige and or archer bombard to free bombard every time a unit in a stack attacks. Both sides of the battle, the attacker and defender, get this free bombard ability.
	- Various cleanup's to DCM text reporting.

v1.035:
	- The game info screen now graphs civ wide revolution stability. The information is is considered a "demographic" and thus will also display for opponent civs for which enough espionage points have been accrued.

v1.034:
	- RevDCM choose religion option implemented. With this option selected, the player chooses the religion on behalf of themselves and the AI when a religion can be founded. Works in tandem or independently of the limited religion option and/or the game default choose religion option.

v1.028:
	- Revolution Watch Advisor replaces the Revolution Watch screen. The advisor is customisable and identical to the domestic advisor in behaviour.
	- IDW no longer gives culture if fight involved animal, hidden nationality unit
	- IDW no longer takes away all of a player's culture in a plot (previously could lose ownership of plots around city generating no culture after losing one fight, even if city had no neighbors)
	- updated to Better BTS AI 0.60

v1.020:
	- Inquisitions mod incorporated into the mod core
		- Inquisitions and main inquisitions features can be turned on or off in the interface
	- Inquisitions AI further improved.
		- inquisition holy city consideration behaviour corrected
		- inquisition city choice behaviour improved to consider the inquisitors path to the city
		- inquisition city choice now considers revolution trend
	- Inquisitions respawn holy city code fixed and tested, becoming a game option in replace of holy city elimination.

v1.002:
	- Enables option control of the Super Spies mod so that it can be turned on or off. Note that the Super Spies AI and the Better BTS AI are not yet exactly the same.
	- Returns the commando promotion to spies for the Super Spies component. Without any default promotion for spies (before they are able to promote through experience), the chance of them being caught in enemy territory is too high. The "Logistics" promotion line becomes a little more attractive as well.
	- Fixes an issue in the BUG unit naming system regarding spies.

v1.001:
	- Adds a mod option control user interface. The interface is accessed via the CTRL-ALT-O BUG standard.

v1.000
	- updated to Better BTS AI 0.50

v0.97
	- updated to Better BTS 0.45
	- included autolog.ini to disable autologging bugs if desired. Otherwise delete and BUG will regenerate an a new ini.

v0.9611:
	- Fixed espionage culture bug in Better AI as per Dresdon report
	- Recompiled the DLL for 50 civs as requested on the RevolutionDCM forum
	- Included the latest makefile to compile this mod
	- Included Jdog5000 patch to fix barbarian civ
	- Included new more "BTS" looking spy promotion icons (thanks Lord Tirian)
	- You may want to keep the supplied autolog.ini because it reduces some very low urgency python errors for this bug component that do not impede game progress. The suggestion is to turn off python popups with HidePythonExceptions = 1 in CivilizationIV.ini. A good idea is to copy across your own BUG 3.5 ini files so that you do not have to reconfigure the interface by hand.

v0.961:
	- Updated to Revolution 1.65c
	- Updated to BUG 3.5
		- Autolog for tracking minor civs disabled due to incompatibility (assets/python/contrib/autologEventManager::onChangeWar(self, argsList)
	- Rolled back the exotic and foreign advisor screens to the pre-BUG versions because of incompatibility with minor civs. Compatibility will be restored at a later point.
	- A non fatal python bug exists in BUG 3.5. The error is possibly due to spawned rebel units but is also perhaps inherent to BUG3.5. Since the bug seems to have no known impact on game play, until the issue is resolved, turn on HidePythonExceptions = 1 in your civilisation ini file.
	  Dump of the bug:
		Traceback (most recent call last):
		File "CvEventInterface", line 30, in onEvent
		File "BugEventManager", line 260, in handleEvent
		File "BugEventManager", line 273, in _handleDefaultEvent
		File "UnitNameEventManager", line 250, in onUnitBuilt
		File "UnitNameEventManager", line 401, in getUnitNameConvFromIniFile
		File "BugCore", line 145, in get
		AttributeError: 'NoneType' object has no attribute 'isColor'
		ERR: Python function onEvent failed, module CvEventInterface
	- Core RevolutionDCM now contains Super Spies 1.3 by default and this mod is thus no longer an addon
	- Be sceptical about some of the DCM air bombing options such as active defense, air bombing, fighter engage. The better BTS AI is now a substantial upgrade on the aerial AI in many cases superceding DCM. Of course DCM options can still be turned off and on via XML.
	- The Super Spies AI is very similar to Better BTS AI except that counter espionage is more diligently executed in SS and AI city poisonings have been reduced somewhat. See the docs/superspies readme file for a bit more detail.
	- Extra espionage missions in Super Spies (assassinate and bribe) are turned off by default in order to preserve standard espionage. Again, see the docs/superspies readme for more information.

v0.96:
	- Updated to Revolution 1.64b
	- Updated to Better BTS AI 0.41B (includes Dresdon unofficial patch 0.21)
	- Updated to Super Spies 1.3 (see readme)
	- Included Ninja2's bug find regarding not being able to bombard from friendly territory (thanks!)

v0.951:
	- Updated to Dresdon's unofficial BTS Solver patch 0.19.1
	- Updated Revolution Inquisition to 1.04 and re-enabled it as an addon
	- Enabled BTS 3.17 Next War as an addon incorporating both Super Spies 1.23 and Revolution Inquisition 1.04
	- Note: Addons to RevolutionDCM are mutually exclusive. You cannot add to an addon.
	- Stealth destroyers cannot range bombard even though the text says they can. This is a DCM issue.

v0.95:
	- Updated to Better BTS AI 0.36
	- Updated user interface to BUG 3.0
	- BUG ini file has been renamed to RevolutionDCM
	- Minor Civ handling in the BUG civ scoreboard is not Revolution mod equivalent. Minor Civs
		do not display until they become full civs and are contactable. This behaviour
		might be addressed at a latter stage because it could be confusing to beginners.
	- Updated Super Spies to Better BTS AI 0.36 and BUG, but omitted the espionage AI changes 
		because this specialised area is covered by TSheep already.

v0.94:
	- Updated to DCM 1.7
	- Stack attack CTD's have been removed and attack abnormalities regarding seige are gone.
	  However the user still can loose control of the interface occasionally. This is not a 
	  major issue because if the user saves right after control is lost (via ctrl-s) and the
	  game reloaded, interface control is resumed perfectly.
	- DCM options that are off by default are still off because they *potentially* are unbalanced
	  with either default game play or with the Better-AI project. More testing is needed.
	- Super Spies addon updated to be compatible with this release.

v0.93: 
	- Updated to Revolution 1.63
	- Includes Super Spies 1.23 for BTS 3.17 as an addon (thanks jojoweb for debugging assistance)
v0.92:
	- Updated to Revolution 1.62

v0.91:
	- Merged Xenomorph's UDCM 1.61 (thanks) into Revolution 1.61
	- Stack attack not supported because it is bugged. Does not work with IDW either. 
          If stack attack is not fixed by Dale next release this will have to wait until 
          I can get a free debugger working for myself so that I can debug it.
	- Battle effect duration reduced to 1:1 ratio (halved)
	- The default enabled DCM options are considered by me to be "compatible" and "working".
	- The default enabled IDW options are considered by me to be "compatible" and "working".
	- All options are now configurable at will via \modname\assets\XML\GlobalDefinesAlt.xml
	- Double checked asset compatibility with BTS 3.17
	- Better Ship Scale BTS 1.0 merged (thanks to the good person who tweaked this)
	- Solver's BTS patch v019 merged
	- Better BTS AI 0.21 merged. This version is still a beta release. Refer to \doc\readme
	- Updated RevolutionDCM foreign advisor screens to highlight border and religious tension
	- Revolution Inquisition still in the process of being updated and improved to BTS 3.17
	- Super Supies addon still requires BTS 3.17 upgrading

Glider1