# Script to rename sound files according to the era the tech is in and to the name we actually
# see when ingame. FUTURE_TECH -> 14_simulation_awareness, etc

# NOTE: THIS ASSUMES THAT VANILLA .MP3 FILES ARE THOSE FOLLOWING THE "TECH_TECHNAME" FORMAT
# It won't catch techs that are pointed to a nonexistent mp3 file following the "Tech_*" format

import os
import glob
import sys
import lxml.etree as ET
from Common import load_tree

# RUN SCRIPT WITH "-rename" to automatically rename entries in the defines, sounds folder
actually_rename = bool(str(sys.argv[-1]) == '-rename')

path_assets_folder =  '../../../Assets/'
path_mp3_files  = '../../../Assets/Sounds/Tech/'

# Substrings from foldernames to not search through. Mormon is really messy, screws up script.
ignore_list = ['(unloaded)', 'Mormon']

# Prefixes to order techquotes for much easier finding, given possible name changes.
# Alternatively script can be easily modified to put each era audio files into their own folder
# or really whatever other organization scheme someone wants
era_dict = {
    'PREHISTORIC'   : '01',
    'ANCIENT'       : '02',
    'CLASSICAL'     : '03',
    'MEDIEVAL'      : '04',
    'RENAISSANCE'   : '05',
    'INDUSTRIAL'    : '06',
    'ATOMIC'        : '07',
    'INFORMATION'   : '08',
    'NANOTECH'      : '09',
    'TRANSHUMAN'    : '10',
    'GALACTIC'      : '11',
    'COSMIC'        : '12',
    'TRANSCENDENT'  : '13',
    'FUTURE'        : '14'}


# desired tech_dict structure:
# 'TECH_NAME' : [ *LIST* (singleplayer) , *LIST* (multiplayer) , 'ERA' , ['TXT_KEY_TECH_NAME','Tech Name'] ]
# *LIST* = ['AS2D_TECH_...' , 'SND_TECH_...' , 'Sounds/Tech/...']
tech_dict = {}

# helper functions to slightly reduce retyping
def find_text(element, schema, child_element):
    text = element.find(f"{schema}{child_element}").text
    return text

# To find relevant xml files in modules, elsewhere
def search_for_xml_ending(ending):
    term = f"{path_assets_folder}**/*{ending}"
    print(f"Recursive globbing for {term} please hold...")
    globreturn = glob.glob(term, recursive=True)
    print(f"Found {len(globreturn)} results")
    # Ignore stuff matching ignore_list elements
    glob_filtered = []
    for i in globreturn:
        ignoring = False
        for j in ignore_list:
            if i.find(j) != -1:
                ignoring = True
                print(f"IGNORING: {i}")
                break
        if not ignoring:
            glob_filtered.append(i)
    print(f"{len(glob_filtered)} good results")
    return(glob_filtered)

# checks tech_dict to see if structure matches after each import;
# might be some element missing. Assumes TXT_KEY description isn't missing.
def breakcheck(dict_depth, dict_width=4):
    err = False
    for k, v in tech_dict.items():
        if len(v) != dict_width:
            print(f"ERROR: Missing entry from: {v}")
            err = True
        else:
            if len(v[0]) != dict_depth:
                print("ERROR: Malformatted or missing:")
                print(f"{k}, {v}")
                err = True
            if len(v[1]) != dict_depth:
                print("ERROR: Malformatted or missing:")
                print(f"{k}, {v}")
                err = True
        if err:
            print(f"These should have depth {dict_depth} in items 0, 1, of {dict_width} items.")
            # Stopping here because continuing makes things messy and I don't want to have to deal w/ more error cases
            sys.exit()

# debug func
def show_dict(tech_entry=''):
    for k, v in tech_dict.items():
        if tech_entry == '' or tech_entry == k:
            print('##########')
            print(f"Tech: {k}")
            if type(v) == list:
                for i in v:
                    print(i)
            else:
                print(v)

# Looks for techs that start with tech_ or TECH_, in mp3 name or 'filename'
def check_vanilla(text):
    if len(text)>=17:
        if text[:17].upper() == 'SOUNDS/TECH/TECH_':
            return True
    elif len(text)>=5:
        if text[:5].upper() == 'TECH_':
            return True
    return False

def mp3_missing_query(mp3_filepath, source, miss_list):
    # is vanilla tech? Assuming used vanilla techs are formatted in this manner, see note
    if check_vanilla(mp3_filepath):
        return
    # Check if file exists
    if not os.path.exists(f"{path_assets_folder}{mp3_filepath}.mp3"):
        print(f"Missing mp3 file from {source}: {mp3_filepath}")
        miss_list.append(mp3_filepath)
        return

need_changing = False
# Sorted/human readable tech name from era, english tech name
def rename_file(filename, era, techname, element, schema, child_element):
    techname_replaced = techname.replace(' ', '_')
    era_number = era_dict[era]
    target_mp3_name = f"{era_number}_{techname_replaced}"
    target_filename = 'Sounds/Tech/' + target_mp3_name
    if filename == target_filename:
        return
    if not check_vanilla(filename):
        # rename file in xml
        print(f"{filename} should be: {target_filename}:")
        full_mp3_filename = f"{path_assets_folder}{filename}.mp3"
        need_changing = True
        if actually_rename:
            print(f"Finding: {schema}{child_element} and making: {target_filename}")
            element.find(f"{schema}{child_element}").text = target_filename
            # rename file in Sounds folder
            if os.path.exists(full_mp3_filename):
                print(f"Renaming: {full_mp3_filename} to {path_mp3_files}{target_mp3_name}.mp3")
                os.rename(full_mp3_filename, f"{path_mp3_files}{target_mp3_name}.mp3")
            else:
                print(f"Cannot find {full_mp3_filename} to rename.")
        else:
            print(f"{filename} should be: {target_filename}:")
            print(f"Will rename: {schema}{child_element}")
            print(f"Will rename: {full_mp3_filename} (if exists) to {path_mp3_files}{target_mp3_name}.mp3")

##############

# loop through techs in *CIV4TechInfos.xml
paths = search_for_xml_ending('CIV4TechInfos.xml')
for path in paths:
    _, root, schema = load_tree(path)
    for tech_info in root[0].findall(f"{schema}TechInfo"):
        tech = find_text(tech_info, schema, 'Type')
        if tech == 'TECH_DUMMY' or tech == '':
            print(f"Tech skipping: {tech}")
        elif tech_dict.get(tech) is not None:
            print(f"DUPLICATE ENTRY: {tech}, {path}")
        else:
            audio2D_single = find_text(tech_info, schema, 'Sound')
            audio2D_multip = find_text(tech_info, schema, 'SoundMP')
            era = find_text(tech_info, schema, 'Era')[8:]
            txt_key = find_text(tech_info, schema, 'Description')
            tech_dict[tech] = [[audio2D_single] , [audio2D_multip] , era , [txt_key]]

breakcheck(1)
print('no missing script links or data')


# Acquire english textname
paths = search_for_xml_ending('CIV4GameText.xml')
for path in paths:
    _, root, schema = load_tree(path)
    for tech_text in root.findall(f"{schema}TEXT"):
        tech_tag = find_text(tech_text, schema, 'Tag')
        for k, v in tech_dict.items():
            if v[3][0] == tech_tag:
                tech_name = find_text(tech_text, schema, 'English')
                v[3].append(tech_name)

for k, v in tech_dict.items():
    if len(v[3]) != 2:
        # These are probably religious techs in their module
        print(f"Posible error: Missing tag for {v[3]}")


# While looping thu scripts, look for tech scripts that aren't referenced in CIV4TechInfos.
unlinked_scripts = {}

# generate accurate link for CIV4TechInfos -> AudioDefines rather than assuming by name
paths = search_for_xml_ending('Audio2DScripts.xml')
for path in paths:
    _, root, schema = load_tree(path)
    for script_2D_sound in root.findall(f"{schema}Script2DSound"):
        script_ID = find_text(script_2D_sound, schema, 'ScriptID')
        if script_ID[:10] == 'AS2D_TECH_':
            matched = False
            sound_ID = find_text(script_2D_sound, schema, 'SoundID')
            for _, v in tech_dict.items():
                if script_ID == v[0][0]:
                    v[0].append(sound_ID)
                    matched = True
                if script_ID == v[1][0]:
                    v[1].append(sound_ID)
                    matched = True
            if not matched:
                unlinked_scripts[script_ID] = [sound_ID]

# # A number of these are from modules or unused quotes from vanilla, but not all.
if len(unlinked_scripts) == 0:
    print('No unlinked scripts!')
else:
    print('Some of these may be vanilla:')
    for i in unlinked_scripts:
        print(f"Possible unlinked script: {i}")

# breakcheck if something should be fixed.
breakcheck(2)
print('2D to script seems valid')


# While looping thu defines, look for tech sound files that aren't referenced in CIV4TechInfos.
unlinked_defines = {}

# find and add filenames from AudioDefines
paths = search_for_xml_ending('AudioDefines.xml')
for path in paths:
    _, root, schema = load_tree(path)
    for sound_data in root[0].findall(f"{schema}SoundData"):
        sound_ID = find_text(sound_data, schema, 'SoundID')
        matched = False
        if sound_ID[:9] == 'SND_TECH_':
            mp3_filepath = find_text(sound_data, schema, 'Filename')
            for _, v in tech_dict.items():
                if v[0][1] == sound_ID:
                    v[0].append(mp3_filepath)
                    matched = True
                if v[1][1] == sound_ID:
                    v[1].append(mp3_filepath)
                    matched = True
            # if matching to civ4techinfos fails, check against floating scripts from before
            if not matched:
                matched_floating = False
                for _, v2 in unlinked_scripts.items():
                    if sound_ID == v2[0]:
                        v2.append(mp3_filepath)
                        matched_floating = True
                if not matched_floating:
                    unlinked_defines[sound_ID] = [mp3_filepath]
if len(unlinked_defines) == 0:
    print('No unlinked defines!')
else:
    for i in unlinked_defines:
        print(f"Probable unlinked defines: {i}")

breakcheck(3)
print('script to mp3 seems valid, no one-to-many relations')


missing_mp3s = []
# Now check the actual .mp3 files, to see if any are missing
for k, v in tech_dict.items():
    for i in range(2):
        mp3_filepath = v[i][2]
        mp3_missing_query(mp3_filepath, 'MAIN TECHS', missing_mp3s)

# Check unlinked scripts that have a define:
for k, v in unlinked_scripts.items():
    if len(v) > 1:
        mp3_filepath = v[1]
        mp3_missing_query(mp3_filepath, 'unlinked script-defines', v)

# Check unlinked defines:
for k, v in unlinked_defines.items():
    mp3_filepath = v[0]
    mp3_missing_query(mp3_filepath, 'unlinked defines', v)


# Searching for unlinked mp3 files:
# ..... todo. Technically can be done by temporarily running the script with different rename_file func.

# Now loop thru AudioDefines again, this time changing things with certainty of no missing links
for path in paths:
    tree, root, schema = load_tree(path)
    for sound_data in root[0].findall(f"{schema}SoundData"):
        sound_ID = find_text(sound_data, schema, 'SoundID')
        if sound_ID[:9] == 'SND_TECH_':
            for _, v in tech_dict.items():
                # check both singleplayer and multiplayer versions, can differ.
                if v[0][1] == sound_ID:
                    rename_file(v[0][2], v[2], v[3][1], sound_data, schema, 'Filename')
                if v[1][1] == sound_ID:
                    rename_file(v[1][2], v[2], v[3][1], sound_data, schema, 'Filename')
    tree.write(path)


if not actually_rename and need_changing:
    print('!!!!!!!!!----------!!!!!!!!!!')
    print("To do these changes, run again with -rename ")
elif not need_changing:
    print('Nothing to rename!')
else:
    print('Done organizing names!')