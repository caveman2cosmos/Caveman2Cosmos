Enhanced Tech Conquest v0.6
By: Grave, Jeckel

Patch Compatibility: Beyond the Sword v3.02 and v3.13
MP Compatible: ?

Description:

The Enhanced Tech Conquest Mod is designed to give a tech boost from conquering 
enemy cities. When an enemy city is conquered, a list of techs the enemy has 
that your empire doesn't is created. From that list a random tech is selected.


Installation Instructions:

1) Unzip this into the "warlords_install_folder\Mods\" folder. 
2) Open the CivilizationIV.ini configuration file
3) Change the Mod line to read: Mod = Mods\Enhanced Tech Conquest
4) Load the game. 
5) Then play as normal. 

-----Game Play-----

- Captured cities always give a minimum of 25% towards the cost of a tech plus 
  a random amount

- The random amount is from 0 to <city size * 5>%. For example a city of size 4 
  would give between 25% and 45% of the tech cost. This assumes the default
  configuration values have not been changed
  
- Cities greater than size 15 increase the chance of getting a full tech. For
  example a city of size 20 would give between 25% and 125% of the tech cost.
  This assumes the default configuration value has not been changed.

- Several configurable options allow tuning of how technology will get 
  transfered from the conquered city to the conquering civilization. 


-----Notes to Modmakers-----

In the Python files I have added # < Enhanced Tech Conquest Start > and 
# < Enhanced Tech Conquest End  > in all of the places that I have made changes 
to the original files. 


-----Version Information-----

-----v0.6------

- Certified to work with Beyond the Sword v3.13!

- Updated all code to be compatible with Beyond the Sword v3.02


-----v0.5w------

- Updated all code to be compatible with Warlords v2.0.0.0

- Fixed several areas of the code that were not working correctly leading to
  technology being given from one civ to another civ with the mod is configured
  to prevent this.


-----v0.4.x------

- Integrated Dr. Elmer Jiggle's event handler and INI parser code

- Added the configurable option allowing players to specify if technology 
  should be handed completely over from the conquered city to their new owners 
  or not.

- Added the configurable option allowing players to specify the number of or 
  part of technologies conquered cities will hand over to their new owners.

- Added the configurable option allowing players to specify the amount of 
  technologies conquered cities will hand over to their new owners should not 
  be random.

- Added the configurable option allowing players to specify if the conquering 
  civilization can receive technology without the appropriate prerequisites or 
  ignore their civilization technology restrictions.

- Added the configurable option allowing players to specify if full technology 
  transfer should be allowed. By setting the value to true this will force 
  players to spend at least one turn researching pillaged technology.

- Added the configurable option allowing players to specify the base technology 
  transfer percentage amount.

- Added the configurable option allowing players to specify the percent amount 
  per city population that will be used to transfer technology to the new owners 
  of the conquered city.

- Added the popup informing players that they did not learn anything from a
  conquered city if there was nothing to learn. Requested by woodelf.

- Changed removed the popups from the mod and replaced them with messages 
  instead. The popups are displayed for all human players even if they are not
  the currently active players. Requested by Shqype, reported by TheLopez.


-----v0.3------

- base tech cost wasn't reflecting modifications from game speed/map size/difficulty level

- altered percentages to give higher tech points for smaller cities

- fixed a bug preventing the popup from displaying

- only techs for which the player has the prerequisites are included in tech list


-----v0.2------

- changed from a flat percentage system to one based on the city size

  
-----v0.1------

- first release
  

----- Todo -----

- Add a configurable option allowing players to choose which technology to
  receive from a conquered city.

-----===Credits & Thanks===-----

- Jeckel
	Without his help, the BtS version wouldn't be possible!

- TheLopez 
	For the Warlords version this is based off of

- Bhruic
	For his original Tech Conquest code
	
- Dr Elmer Jiggle 
	For providing the INI file parser code allowing for players to 
	customize this mod without having to touch the python code!!!