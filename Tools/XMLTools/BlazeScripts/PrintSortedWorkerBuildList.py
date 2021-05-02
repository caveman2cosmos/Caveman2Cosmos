import lxml.etree as ET
from Common import load_tree

path_xml_folder =  '../../../Assets/XML'

techInfoPath  = "/Technologies/CIV4TechInfos.xml"
buildInfoPath = "/Units/CIV4BuildInfos.xml"
impInfoPath   = "/Terrain/CIV4ImprovementInfos.xml"
# routeInfoPath = "/Misc/CIV4RouteInfos.xml"

era_dict = {
    'C2C_ERA_PREHISTORIC'   : 1,
    'C2C_ERA_ANCIENT'       : 2,
    'C2C_ERA_CLASSICAL'     : 3,
    'C2C_ERA_MEDIEVAL'      : 4,
    'C2C_ERA_RENAISSANCE'   : 5,
    'C2C_ERA_INDUSTRIAL'    : 6,
    'C2C_ERA_ATOMIC'        : 7,
    'C2C_ERA_INFORMATION'   : 8,
    'C2C_ERA_NANOTECH'      : 9,
    'C2C_ERA_TRANSHUMAN'    : 10,
    'C2C_ERA_GALACTIC'      : 11,
    'C2C_ERA_COSMIC'        : 12,
    'C2C_ERA_TRANSCENDENT'  : 13,
    'C2C_ERA_FUTURE'        : 14}

min_era = "C2C_ERA_PREHISTORIC"
max_era = "C2C_ERA_FUTURE"


######## USE THIS TO CONTROL WHAT IS PRINTED ######

# Either provide an XML filepath (None to not use):
# unitInfoPath = "/Units/U_Workers_CIV4UnitInfos.xml"
unitInfoPath = None

# Or fill out the following, to be used if None entered above
# 0 is naval only, 1 is land only, 2 is both
landType = 2
# The tech that the unit is unlocked from; builds that obsolete before this ERA won't be shown (None or first era for no prereq tech)
# worker_unlocked_tech = "TECH_SCREW_PROPELLER"
worker_unlocked_tech = None
# The tech that the units can be upgraded at; builds that unlock at ERAS higher than this won't be shown (None or C2C_ERA_FUTURE for no obsolete)
# worker_obsolete_tech = "worker_obsolete_tech"
worker_obsolete_tech = None

#####################################################

# helper functions to slightly reduce retyping
def find_text(element, schema, child_element):
    tag = element.find(f"{schema}{child_element}")
    if tag is not None:
        text = element.find(f"{schema}{child_element}").text
        return text
    return None



# Brute scripting, ugh

# tech_era_dict = {TECH_ABC: C2C_ERA_XYZ, TECH_DEF: C2C_ERA...}
tech_era_dict = {}
_, root, schema = load_tree(path_xml_folder+techInfoPath)
for tech_info in root[0].findall(f"{schema}TechInfo"):
    tech = find_text(tech_info, schema, 'Type')
    era  = find_text(tech_info, schema, 'Era')
    tech_era_dict[tech] = era


# store bool for water improvement or not; too lazy to do Tunnel, manually check for sea routes
impWaterDict = {}
_, root, schema = load_tree(path_xml_folder+impInfoPath)
for buildInfo in root[0].findall(f"{schema}ImprovementInfo"):
    impWaterDef = find_text(buildInfo, schema, 'bWaterImprovement')
    if impWaterDef is not None:
        impWaterDict[find_text(buildInfo, schema, 'Type')] = int(impWaterDef)
    else:
        impWaterDict[find_text(buildInfo, schema, 'Type')] = 0

# grab a list of [tech required, tech obsoleted, name of build action, bool if naval or not]
builds = []
_, root, schema = load_tree(path_xml_folder+buildInfoPath)
for buildInfo in root[0].findall(f"{schema}BuildInfo"):
    buildName = find_text(buildInfo, schema, 'Type')
    preqTech  = find_text(buildInfo, schema, 'PrereqTech')
    postTech  = find_text(buildInfo, schema, 'ObsoleteTech')
    impName   = find_text(buildInfo, schema, 'ImprovementType')

    builds.append([preqTech, postTech, buildName, impWaterDict.get(impName, 0)])


# sort in order; tech required together, then obsolete, then alphabetical buildname
builds.sort(key=lambda tagName: (era_dict[tech_era_dict.get(tagName[0], max_era)],
    era_dict[tech_era_dict.get(tagName[1], max_era)],
    tagName[2]))


# Oof. Formatting. Care with spaces vs tabs! Make sure is copied right...
def wrapEntry(entry, isComment=False):
    if not isComment:
        print(f"\t\t<Build>\n\t\t    <BuildType>{entry}</BuildType>\n\t\t    <bBuild>1</bBuild>\n\t\t</Build>")
    else:
        print(f"\t\t<!-- {entry} -->")

print("BE SURE TO DOUBLE CHECK FOR '_KILL' OR OTHER ODD BUILD ACTIONS; DO A QUICK ONCE-OVER!\n")


unit_list = []
if unitInfoPath is not None:
    _, root, schema = load_tree(path_xml_folder+unitInfoPath)
    for unitInfo in root[0].findall(f"{schema}UnitInfo"):
        unit_type = find_text(unitInfo, schema, 'Type')
        domain = find_text(unitInfo, schema, 'Domain')
        if domain == "DOMAIN_LAND":
            landType = 1
        elif domain == "DOMAIN_SEA":
            landType = 0
        worker_unlocked_tech = find_text(unitInfo, schema, 'PrereqTech')
        worker_obsolete_tech = find_text(unitInfo, schema, 'ForceObsoleteTech')
        unit_list.append([unit_type, landType, worker_unlocked_tech, worker_obsolete_tech])
else:
    unit_list.append(['CUSTOM', landType, worker_unlocked_tech, worker_obsolete_tech])


for unit in unit_list:
    print(f"\n\t    {unit[0]}\n\t    <Builds>")

    curr_era_prereq = 0
    curr_era_post   = 0

    for tag in builds:
        if tag[3] != unit[1]:
            # Only allow builds that unlock in eras before worker can upgrade and builds that obsolete in eras in or after worker is unlocked
            if era_dict[tech_era_dict.get(unit[3], max_era)] >= era_dict[tech_era_dict.get(tag[0], min_era)] and\
                era_dict[tech_era_dict.get(unit[2], min_era)] <= era_dict[tech_era_dict.get(tag[1], max_era)]:

                if era_dict[tech_era_dict.get(tag[0], max_era)] != curr_era_prereq:
                    wrapEntry(f"BUILDS REQUIRING: {tech_era_dict.get(tag[0], max_era)}", True)
                    curr_era_prereq = era_dict[tech_era_dict.get(tag[0], max_era)]
                    curr_era_post = 0

                if era_dict[tech_era_dict.get(tag[1], max_era)] != curr_era_post:
                    wrapEntry(f"Obsolete in: {tech_era_dict.get(tag[1], max_era)}", True)
                    curr_era_post   = era_dict[tech_era_dict.get(tag[1], max_era)]

                wrapEntry(tag[2])

    print("\t    </Builds>")