import lxml.etree as ET

path_xml_folder =  '../../../Assets/XML'

########################################

target_filepath = "/Units/CIV4UnitInfos.xml"
main_tag = "UnitInfo"

target_tag = "Domain"
target_tag_value = "DOMAIN_AIR"

########################################

opening_tag = f"<{main_tag}>"
closing_tag = f"</{main_tag}>"
matching_line = f"<{target_tag}>{target_tag_value}</{target_tag}>"

with open(path_xml_folder + target_filepath, 'r') as src_file:
    matching_line_list = []
    is_match = False

    # read thru target file, note codeblocks around tag you want
    for line_count, line in enumerate(src_file):

        if opening_tag in line:
            block_start = line_count
        elif matching_line in line:
            is_match = True
        elif closing_tag in line:
            block_end = line_count
            if is_match:
                matching_line_list.append((block_start, block_end))
            is_match = False

    src_file.seek(0)

    # write to file the codeblocks you found (could be done without double looping by storing stuff the first time 'round but eh whatever)
    in_block = False
    block = matching_line_list.pop(0)
    with open(f"{target_tag}_{target_tag_value}.xml", 'w') as dst_file:
        for line_count, line in enumerate(src_file):
            if line_count == block[0]:
                in_block = True

            if in_block:
                dst_file.write(line)

            if line_count == block[1]:
                in_block = False
                if len(matching_line_list) == 0:
                    print("Done!")
                    break
                block = matching_line_list.pop(0)