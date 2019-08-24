[COLOR="Green"][B]DynamicCivNames Description[/B][/COLOR]
Version 0.51
[COLOR="Green"]Enabled[/COLOR] by default

[B]The goal:[/B]

This component renames civs based on their civics choices and other status factors.  A civs names will change many times over the course of a game.  While it has no actual effect on gameplay, it adds dynamic variety to the game.

[B]How it works:[/B]

When a civ is created or changes their civics, they are potentially assigned a new name based on many factors.  Civs spawned by either Revolution or BarbarianCiv will get temporary names indicating their status, like Mongolian Horde or Russian Rebels, until they're more firmly established.  When a civ switches to Hereditary Rule their name will change to something like Kingdom of America, while under Representation or Universal Sufferage they would be the American Republic.  Civs with only one city in the later stages of the game may have the city name included in their title, but their civ type will always be clarified as in the French Kingom of Paris.  Vassals will also get different names.  There are many different potential names, and even variations for civs with the same status to provide some flavor.

All of the civics determinations are made using the added Revolution flags in the civics xml file to support variations on the standard civics systems.  Having a civic with the isCommunism bool set will cause Communist style names.  In addition, some civs have special names like show up in certain circumstances, like the German Reich and Arabian Caliphate.  These determinations are made by matching text in the civs adjective (ie German or Arabian).

This component has several features designed to support names of civs created by BarbarianCiv and Revolution, but would provide plenty of fun flavor name changes as a standalone mod or addition for other mods.  It requires changes to the DLL to allow name changes to stick, but all name changes are generated in Python.

Feedback and suggestions for how to improve the names and add more variety would be great.