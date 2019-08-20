Female Missionaries ModComp

This modcomp adds a female version for each of the Missionaries as well as for the CEOs. These new units don't replace the original units. Instead there is a certain chance that each time a Missionary or CEO is build, this unit is female. The chance for this is 15 % without the emanzipation civic and 50 % with that civic. Because the Female Missionaries and CEOs have the same attributes as the original male ones this modcomp doesn't alter the gameplay in any way, it's just pure eye candy. In order to keep the civilopedia clean, the new units won't appear in it. However, you can see that this modcomp works by checking the original Missionaries. If there is a new entry 'Replaced by ...' the modcomp works.

Now for the more technical details:

This modcomp is mostly modular. However, there are some files that can't be made modular. These files are three python files (CvEventManager, CvPediaUnit, CvPediaMain) and two xml files (Audio3DScripts and AudioDefines). The changes in the python files can be found by searching for 'saibotlieh', the changes in the xml files can be found near the buttom of the files (lines 22861-23005 and lines 10979-11013). Further there is a sound folder with custom sounds that must be placed direct into the base mod Assets folder.
All files in the zip-file are already put into the necessary folder, unzipping it directly in the Mods folder of BtS should result into a working mod.
The python functions that changes the Missionaries and CEOs to female units is written in a way that all units with the DefaultUnitAI attribute 'UNITAI_MISSIONARY' that have similiar named female unit counterparts are targets. For example, the existence of the Unit_Female_Christian_Missionary makes the Unit_Christian_Missionary a target. So, if it is desired to add more female Missionaries and CEOs to this modcomp (for custom Missionaries for example) this is possible by just naming the new female units accordingly and making sure that their DefaultUnitAI attribute is 'UNITAI_MISSIONARY'. For example, if a Female Shinto Missionary should be added to this modcomp, that unit should be named Unit_Female_Shinto_Missionary (if the original unit is named Unit_Shinto_Missionary that is).

If you are using my Native Amazon, my Female Great People or my Female Modern Soldiers modcomp with this modcomp:

- The python files in the Screens folder don't need to be changed.
- Most of the sound files are the same, they can be overriden if asked for.

Credits/Thanks

- The_J for his 'Invisible dummy techs' mod that I've modified to fit to my needs.
- God-Emperor for the idea to use the Civilopedia entry for turning the units invisible.
- BioWare, the audio files are from their game Dragon Age Origins.
- Ploeperpengels 'female sound animations' that where my starting point for my animation sounds.
- All the users that gave their feedback to my unit creations.
- All the writers of the tutorial section and helpful people in the forums. Without those, I'd never have learnt how to mod in the first place.

Known bugs/errors/potential bugs

- For now not all Missionaries have a female counterpart. Missing so far are the ones for the Buddhism, the Hinduism, the Islam and the Judaism religion.


saibotlieh