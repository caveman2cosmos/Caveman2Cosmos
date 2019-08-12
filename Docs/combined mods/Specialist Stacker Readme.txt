Specialist Stacker Mod v0.8.1w
By: TheLopez

Patch Compatibility: Warlords v2.0.8.0
MP Compatible: ?


Description:

This mod "stacks" the specialized citizens, super specialized citizens and angry 
citizens instead of displaying them side-by-side. It also makes use of the space 
to the left of the specialized citizens by allowing them to span into two columns 
instead of just one. This will allow for more than the vanilla six specialized 
citizens to exist in the game.


Installation Instructions:

1) Unzip this into the "warlords_install_folder\Mods\" folder. 
2) Open the CivilizationIV.ini configuration file
3) Change the Mod line to read: Mod = Mods\Specialist Stacker
4) Load the game. 
5) Then play as normal. 


-----Notes to Modmakers-----

I have modified the CvMainInterface.py and Dr Elmer Jiggle's CvConfigParser.py 
file. I have added #< STACKER START > and #< STACKER END > in all of the places 
that I have made changes to the files. 

As of version v0.6.1 the Specialist Stacker configuration options have been moved
to the "Specialist Stacker Config.ini" file. There are 11 variables that can be 
used to configure the stacker settings and are well documented in the INI file. 
Please note that you no longer need to copy the "Specialist Stacker Config.ini"
file to the "civ4_install_folder\Mods\" directory. Just make sure that you put
a copy of the "Specialist Stacker Config.ini" in your directory and you update
the modName variable in the CvModName.py file with your mod's name.

I highly recommend that you use Dr Elmer Jiggle's INI parser if you want to 
allow players to configure your mod through INI files.

If you use this mod, all I ask is that you give me credit.


-----Version Information-----

-----v0.8.xw------

- Updated to be compatible with Warlords v2.0.8.0 patch.

- Readded in the missing functionality that displays the total additions from 
  each type of specialist and the total from all of the specialists together. 
  Reported by Zuul.

-----v0.7w------

- Updated all code to be compatible with Warlords v2.0.0.0


-----v0.7------

- Updated all code to be compatible with the v1.61 patch!


-----v0.6.2------

- Updated the mod to use the latest version Dr Elmer Jiggle's INI parser code.


-----v0.6.1------

- Updated Dr Elmer Jiggle's to look in the "civ4_install_folder\Mods\<ActiveModName>" 
  directory. The <ActiveModName> is retrieved from the modName variable in the 
  CvModName.py file. With these changes the mod component configuration INI files 
  will no longer need to live in the "civ4_install_folder\Mods\" directory. 

-----v0.6------

- Integrated Dr Elmer Jiggle's INI parser code and created the INI file 
  containing all of the possible values that can be used to configure the
  features in the mod

- Updated Dr Elmer Jiggle's to look in the "civ4_install_folder\Mods\"
  and "<userDir>\Mods\" directories. Also added and tested a fix provided 
  Dr Elmer Jiggle fixing the default INI values bug.


-----v0.5------

- Added the coded needed to make angry citizens stack including a variable 
  allowing to turn the angry citizens stacking feature on and off. It is 
  turned off by default.

- Added a variable to allow specifing distance between the angry citizens 
  stacked specialists 

- Added a variable to specify if all angry citizens specialists should be 
  displayed or not.

- Added a variable to specify if angry citizens should be dynamically spaced 
  or not


-----v0.4------

- Added a "Specialists" label and background above the specialists

- Added the coded needed to make super specialists stack including a variable
  allowing to turn the super specialists stacking feature on and off. It is 
  turned off by default.

- Added a variable to allow specifing distance between the super stacked 
  specialists 

- Added a variable to specify if all super specialists should be displayed or 
  not.

- Added a variable to specify if specialists should be dynamically spaced or
  not


-----v0.3------

- Added code to make specialized citizen span two columns instead of one

- Added a variable allowing to turn on and off the highlighting of forced
  specialized citizens

- Added a variable to allow specifing distance between the stacked specialists 

-----v0.2------

- Updated code to make specialists stack from right to left

- Started cleaning up code to allow reuse in other mods.


-----v0.1------

- Setup Specialist Stacker infrastructure

- Isolated code responsible for displaying and updating specialists


-----To Do-----

- Modularize the code even more to allow specification of columns of
  specialized citizens


-----===Credits & Thanks===-----

- Exavier 
	Composite Mod - readme.txt format


- Snaitf
	For starting the changes needed to make this mod component happen.

- Chalid
	For providing the suggestion that super specialists should stack
	like the regular specialists

- Dr Elmer Jiggle 
	For providing the INI file parser code allowing for players to 
	customize this mod without having to touch the python code!!!