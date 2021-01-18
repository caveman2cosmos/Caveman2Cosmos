import csv
import lxml.etree as ET


path_xml_folder =  '../../../Assets/XML'


path_improvements = "/Terrain/CIV4ImprovementInfos.xml"

#####################################################

# Given path to xml file, return (root, schema) of that file
def load_tree(path_to_xml):
    tree = ET.parse(path_to_xml)
    root = tree.getroot()
    tag = root.tag
    schema = tag[:tag.find('}')+1] #probably some .attrib way to get it but this works too
    return tree, root, schema

# helper functions to slightly reduce retyping
def find_text(element, schema, child_element, return_if_missing = None):
    tag = element.find(f"{schema}{child_element}")
    if tag != None:
        text = element.find(f"{schema}{child_element}").text
        return text
    else:
        return return_if_missing


"""
psuedocode:
b_dict = {bonus: [iDiscoverRand1, iDiscoverRand2, ...]} where position of iDiscoverRand values correlates to list of improvements
for improvement
    if has bonus structs entry
        for bonus struct
            add improvement to array

for k, v in dict
    print k fancy
    for v2 in dict
        print v2 fancy
"""

# It would be much simpler to pre-alloc an overlarge array to fill then later prune or even dynamically resize one, but owell
imp_list = []
b_dict = {}
_, root, schema = load_tree(path_xml_folder+path_improvements)
# Loop thru improvements from ImprovementInfos
for imp_info in root[0].findall(f"{schema}ImprovementInfo"):
    imp = find_text(imp_info, schema, 'Type')[12:]
    b_structs = imp_info.find(f"{schema}BonusTypeStructs")
    # If improvement has bonus structs, then collect relevant info
    if b_structs != None:
        if imp not in imp_list:
            imp_list.append(imp)

        for b_struct in b_structs.findall(f"{schema}BonusTypeStruct"):
            b_name = find_text(b_struct, schema, 'BonusType')[6:]
            b_trad = find_text(b_struct, schema, 'bBonusTrade', 0)
            b_disc = find_text(b_struct, schema, 'iDiscoverRand', 0)
            b_depl = find_text(b_struct, schema, 'iDepletionRand', 0)
            
            if b_dict.get(b_name) == None:
                b_dict[b_name] = [0]*len(imp_list)
            else:
                if len(b_dict[b_name]) < len(imp_list):
                    b_dict[b_name] += [0]*(len(imp_list) - len(b_dict[b_name]))
            
            b_dict[b_name][imp_list.index(imp)] = (int(b_trad), int(b_disc), int(b_depl))

# print(['Bonuses']+imp_list)
# exit()

with open ('Reports/BonusBreakdown.csv', 'w', newline='') as csvf:
    csvw = csv.writer(csvf)
    csvw.writerow(['Bonuses'] + imp_list)
    for b_name, chances in b_dict.items():
        # extending 0s
        if len(chances) < len(imp_list):
            chances += [0]*(len(imp_list) - len(chances))

        csvw.writerow([b_name]+chances)