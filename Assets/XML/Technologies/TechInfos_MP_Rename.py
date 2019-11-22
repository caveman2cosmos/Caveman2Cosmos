######################
# IMPORTANT NOTE:
# To run this, since idk how xml schemas interact with xml.etree lol, you
# need to TEMPORARILY # remove the xlmns bit from the CIV4TechInfos.xml
# Remember to replace when done!
#######################

import lxml.etree as ET

path_tech_infos = 'CIV4TechInfos.xml'

b_tech_infos_reformatted = 0

tree_tech_infos = ET.parse(path_tech_infos)
root_tech_infos = tree_tech_infos.getroot()[0]

for tech_info in root_tech_infos.iter('TechInfo'):
    tech_mp_sound = tech_info.find('SoundMP').text

    # mp quote needs replacing if it isn't already generic and not already _MP_
    if ((tech_mp_sound != 'AS2D_TECH_GENERIC') and ('_MP_' not in tech_mp_sound)):
        print(f"Switching {tech_mp_sound} to AS2D_TECH_GENERIC")
        tech_info.find('SoundMP').text = 'AS2D_TECH_GENERIC'
        b_tech_infos_reformatted = 1

if b_tech_infos_reformatted:
    tree_tech_infos.write(path_tech_infos)
    print('''
        CIV4TechInfos.xml reformatted.
        Remember to replace the xmlns when done!
        Additionally, the reformatting occasionally drops the first line of:
        <?xml version="1.0" encoding="utf-8"?>
        so be sure to include that if it's missing!
        ''')
else:
    print('No MP techs discovered with duplicate SP counterparts')