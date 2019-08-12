Female Missionaries ModComp

This modcomp adds a female version for each of the Missionaries as well as for the CEOs. These new units don't replace the original units. Instead there is a certain chance that each time a Missionary or CEO is build, this unit is female. The chance for this is 15 % without the emanzipation civic and 50 % with that civic. Because the Female Missionaries and CEOs have the same attributes as the original male ones this modcomp doesn't alter the gameplay in any way, it's just pure eye candy. In order to keep the civilopedia clean, the new units won't appear in it. However, you can see that this modcomp works by checking the original Missionaries. If there is a new entry 'Replaced by ...' the modcomp works.

Now for the more technical details by Dancing Hoskuld:

I have modified this making it BUG/WoC compatible and thus making the python modular.  To make my life easier I have removed the bit that makes the units not appear in the Civlopedia.  This is because I would need to keep three versions of the python code - one for BUG/WoC, one for Rise of Mankind and one for the main mod to Rise of Mankind A New Dawn.

Installation instructions:

1. Copy the files from here into your mod.
2. In the Assets\Config folder edit init.xml and add the following line just before the last line, the one containing "</bug>".

	<load mod="Female Missionaries"/>

3. Run the woc_installer.jar found in the Assets\Modules folder.  This will install the sounds.

