import lxml.etree as ET

path_xml_folder =  '../../../Assets/XML'

########################################

# target_filepath = "/Units/CIV4UnitInfos.xml"
target_filepath = "/Units/U_Land_CIV4UnitInfos.xml"
main_tag = "UnitInfo"

# target_tag = "Domain"
# target_tag_value = "DOMAIN_AIR"
target_tag = "SubCombatType"
target_tag_value = "UNITCOMBAT_CIVILIAN"

# string_disallow = None
string_disallow = "<iCombat>"

########################################

opening_tag = f"<{main_tag}>"
closing_tag = f"</{main_tag}>"
matching_line = f"<{target_tag}>{target_tag_value}</{target_tag}>"

with open(path_xml_folder + target_filepath, 'r') as src_file:
    matching_line_list = []
    is_match = False
    is_disallowed = False

    # read thru target file, note codeblocks around tag you want
    for line_count, line in enumerate(src_file):

        if opening_tag in line:
            block_start = line_count
        if matching_line in line:
            is_match = True
        if string_disallow in line:
            is_disallowed = True
        if closing_tag in line:
            block_end = line_count
            if is_match and is_disallowed is False:
                matching_line_list.append((block_start, block_end))
            is_match = False
            is_disallowed = False

    if len(matching_line_list) == 0:
        print(f'No matches for {matching_line} found!')
        exit()
    else:
        print(f"{len(matching_line_list)} matches found for {matching_line}.")

    src_file.seek(0)

    # write to file the codeblocks you found (could be done without double looping by storing stuff the first time 'round but eh whatever)
    block_list = matching_line_list
    in_block = False
    block = matching_line_list.pop(0)
    file_dst = f"{target_tag}_{target_tag_value}.xml"
    with open(file_dst, 'w') as dst_file:
        for line_count, line in enumerate(src_file):
            if line_count == block[0]:
                in_block = True

            if in_block:
                dst_file.write(line)

            if line_count == block[1]:
                in_block = False
                if len(matching_line_list) == 0:
                    print(f"Saved matching entries to {file_dst}")
                    break
                block = matching_line_list.pop(0)

    choice = input(f'Remove entries from original {path_xml_folder+target_filepath} file Y/N?\n')
    if choice.upper() == 'Y':
        print('lol sorry not implemented yet!')



    else:
        print('Exiting')