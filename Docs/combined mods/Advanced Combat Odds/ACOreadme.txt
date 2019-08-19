-------------------------
Advanced Combat Odds v1.0 readme
-------------------------
Written by PieceOfMind

For more information:
http://forums.civfanatics.com/showthread.php?p=7768948#post7768948


Installation instructions:
--------------------------
1. You must have at least Beyond the Sword version 3.17 before installing.
2. To install as a mod, simply extract the zip to your mods folder (\My Documents\My Games\Beyond the Sword\MODS\)and a mod called "Advanced Combat Odds v1_0" will be created, which can be loaded from the game.
3. To install it over the original DLL (not recommended if you play multiplayer), then first BACKUP the original dll file, which can be found at \Firaxis Games\Sid Meier's Civilization 4\Beyond the Sword\Assets\CvGameCoreDLL.dll, and then paste the new DLL in its place. If you installed it correctly it should be obvious when you start up a game and try out the mouseover odds.  If you use this method, you should also place the contents of the "Advanced Combat Odds v0_4" folder inside your CustomAssets folder, as some XML changes have been made.

4. To change the detail level, you must change the value of COMBAT_ODDS_DETAIL in the XML file GlobalDefines.xml included.
0 = Low
1 = Medium
2 = High
3 = Everything




Documentation for Advanced Combat Odds v0.5
-------------------------------------------

Version History
---------------
v0.1

v0.2
-Fixed a bug causing Defender Unharmed odds to be too high if defender was 1 hit from being killed at start of battle.
-Added expected hitpoints conditional on certain unit outcomes, now placed beside the specific outcomes.
-Removed universal expected hitpoints, as they were less useful with the above additions
-Removed combatLimit display for attacker because this is now obvious from the expected defender hitpoints beside the Withdraw odds.

v0.3
-Added options for Low,Medium,High details. Also included Everything level.
-Added a few new statistics
-Added CombatRatio
-Added detailed HP outcomes
-Added more detail to XP shown for Everything detail level
-Added range CombatRatio detail to Everything detail level
-Moved Defender Unharmed odds to only High detail
-Re-added universal expected hitpoints but only for High and Everything detail settings

v0.4
-Green/red bars for detailed HP outcomes
-Fixed bug when withdraw odds were greater than 100% (eg. attack sub with +60% more withdraw chance). Units can in fact have exactly 100% withdraw odds
-Added visual bar at top of display, representing the 3 main battle outcomes (or 2 if no retreat odds involved)
-Fixed bug where the bottom Attacker HP value was not including the odds of retreat.
-Re-added defender/attacker unharmed odds to medium detail.
-Coloured part of the bottom HP bar for attacker yellow (high detail and above only) to reflect the proportion of the odds that are due to reatreat chances.

v0.5
-1 decimal place only for Low setting
-Merged HP bars when they were individually less than some cutoff - currently 0.5%.  Note the attacker min HP bar was left alone in case retreat odds came into it.
-Added XP modifiers (eg. Great General promotion - Leadership)
-Fixed XP range calculations so they will work with mods
-Added detail about free wins vs barbs - now shows correct odds for such battles unlike old calculator which lied to the user. (note this only works if defender is barb at the moment)
-Added correct XP taking barb XP caps into account, including animals.  e.g. if on 8XP and killing the barb would normally result in large xp, the shown amount will be only 2.
-Fixed negative XP bug in barb normalization code (thanks to phungus420) (it was only present in earlier builds of this version)
-Prevented the unit unharmed HP bars from being combined with others.  eg. if you use a warrior to attack an infantry it will still show you the bar with the probability of your warrior keeping 100HP or whatever other hitpoints it starts with.


v0.6
-Added display for barb free wins remaining.  You get free wins vs. barbs on Prince difficulty and lower.

v1.0 Released



To Do (later versions)
+Flanking damage
+GG point modifiers?  eg. great wall and IMP leaders
+look at how incorrect modifiers are showing, when using switcharoo (not enabled in v1.0)


Using Advanced Combat Odds:

Right from the outset, make sure you understand that all odds in the Advanced Combat Odds display are rounded to 1 or 2 decimal places. Displayed odds of 100.00%, for example, could represent 99.995% or greater meaning the odds are not certain. Similarly, 0.00% does not mean zero odds - just very small odds.

There are now four settings for the level of detail you wish to have displayed using this mod: Low, Medium, High and Everything! Medium is the default setting and recommended for most users.  For the three settings Low, Medium and High, pressing and holding the SHIFT key brings up the Everything! detail - handy if you only need all the detail very occasionally.

Advanced Combat Odds aims to be more accurate, relevant and informative than the original odds calculator. See the included text file for a list of features available at each detail setting.




What do all the values mean?
---------------------------

Survival Odds:
These are simple - the odds of both the attacker (green) and defender (red) surviving.  Sometimes using units like catapults means the defender will have 100% odds; This is because the attacker cannot kill the defender and must withdraw before dealing a killing blow.

Victory:
The odds of the attacker killing the defender.  Also presented on this line is the number of XP the attacker would earn if he wins.  At Medium detail and higher, there is an additional value on this line.  This value is the average hitpoints of the attacker if he kills the defender.

Withdraw:
These are the odds of a forced withdrawal from combat.  Units like catapults and cannons, which have a combat limit, cannot kill a defending unit.  Instead they can only damage the defender down to a certain level (eg. 25HP for catapults, 20HP for cannons, 15HP for artillery).  The XP a unit earns for withdrawal is 1XP.  At the end of this line, the average values of both attacker and defender HP are presented for the withdrawal battle outcome (for Medium detail and higher).  Note average defender HP on this line corresponds to the damage limit of the unit you are using to attack.

Retreat:
If you are using units that have inherent retreat odds or that have earned promotions that give them retreat chances, then this line tells you the probability of your unit retreating.  Units will only retreat if they are one hit away from being killed, and only when they are the attacker.  The HP value presented on this line (Medium detail and higher) is the number of hitpoints the attacker would have if he were to retreat from the battle injured.

Defeat:
The odds of the attacker being killed by the defender.  The XP the defender will earn is displayed, along with the average number of hitpoints the defender will have if he survives combat (Medium detail and higher).

Detailed Attacker HP Outcomes:
At the High and Everything, these values will be displayed directly under the battle outcomes representing the attacker surviving.  For each number of hitpoints the attacker could possibly have, beside it you will find the probability of the unit exiting battle with that many hitpoints.  There is also a coloured bar beside each outcome, to help visually identify the scope of the values.  Note if there are retreat odds involved, the lowest HP value for the attacker will include the probability of retreat in its odds.  Note the sum of all the HP outcomes should be the same as the odds of the attacker surviving.

Detailed Defender HP Outcomes:
Much the same as Attacker HP Outcomes, except for the defender.  Note the sum of all these values should be the odds of the defender surviving.

Attacker/Defender damager per hit, number of hits required, odds of hits and CombatRatio:
A lot of data is displayed on this line.  The first two values are the amount of HP damage each of the attacker and defender (respectively) will cause per hit.  Then after that, the number of hits required by each combatant are displayed.  In green, is the number of hits required by the attacker to kill the defender or to force the attacker's withdrawal due to a combat limit.  In red is the number of hits required for the defender to make to either kill the attacker or to make him retreat.  Next, the odds displayed in green are the odds of the attacker for making a hit in each combat round.  The defender's odds are just 100% minus this number.  Finally on this line is the important value called the CombatRatio or simply R.  This value is equal to the attacker's modified combat strength divided by the defender's modified combat strength.  This number is important for many reasons - one being that it determines how many XP the attacker and defender can earn from combat.  The value R takes both injured units and all combat related modifiers into account (e.g. promotions and defense bonuses).

Average HP: (High detail an above)
This line simply gives the average hitpoints for each combatant after battle.  Unlike the averages presented earlier, these averages include the possibility of the unit dying, so they better reflect the overall outcome of battle.

Unharmed:
These odds are very simple.  They are the odds of the attacker/defender exiting combat without taking a single hit i.e. unharmed.  Units that are already injured are counted as winning unharmed if they don't take damage during the battle.

Unrouneded XP:
These values are shown at Everything detail only.  These values are rounded down to the nearest whole number, and then forced to be within 1 and 10, to give the actual XP that can be earned by each combatant for killing their opponent.

XP range:
This line is only shown at Everything detail, and it crams a lot of information on one line.  In the middle, it presents the CombatRatio i.e. the R value, and the XP it corresponds to.  On either side of it, the CombatRatios that would lead to different number of XP earned are shown.  If you increase the value of R (for example, by taking a Combat promotion) then tipping it over the next value will cause your XP earned to drop by 1 point.