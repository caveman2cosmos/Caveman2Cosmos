import os
import glob
import sys
import lxml.etree as ET


path_assets_folder =  '../../Assets/'


techInfoPath  = "XML/Technologies/CIV4TechInfos.xml"
buildInfoPath = "XML/Units/CIV4BuildInfos.xml"

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

# Given path to xml file, return (root, schema) of that file
def load_tree(path_to_xml):
    tree = ET.parse(path_to_xml)
    root = tree.getroot()
    tag = root.tag
    schema = tag[:tag.find('}')+1] #probably some .attrib way to get it but this works too
    return tree, root, schema

# helper functions to slightly reduce retyping
def find_text(element, schema, child_element):
    text = element.find(f"{schema}{child_element}").text
    return text

# Oof. Formatting.
def wrapEntry(entry, isComment=False):
    if not isComment:
        print(f"                <Build>\n                    <BuildType>{entry}</BuildType>\n                    <bBuild>1</bBuild>\n                </Build>")
    else:
        print(f"                <!-- {entry} -->")



# Brute scripting, ugh.
tech_era_dict = {}
_, root, schema = load_tree(path_assets_folder+techInfoPath)
for tech_info in root[0].findall(f"{schema}TechInfo"):
    tech = find_text(tech_info, schema, 'Type')
    era  = find_text(tech_info, schema, 'Era')
    tech_era_dict[tech] = era

build_era_dict = {}
_, root, schema = load_tree(path_assets_folder+buildInfoPath)
for tech_info in root[0].findall(f"{schema}BuildInfo"):
    build   = find_text(tech_info, schema, 'Type')
    techreq = find_text(tech_info, schema, 'PrereqTech')

    # Could improve to sort by obsolete tech but mreh
    try:
        techObs = find_text(tech_info, schema, 'ObsoleteTech')
    except Exception as e:
        techObs = "C2C_ERA_FUTURE"
    

    if techreq != "NONE":
        era = tech_era_dict[techreq]
    else:
        era = "NONE"

    if build_era_dict.get(era):
        build_era_dict[era].append(build)
    else:
        build_era_dict[era] = [build]


builds = []
for entry in build_era_dict:
    builds.append(entry)
builds.sort(key=lambda x: era_dict[x])

for entry in builds:
    wrapEntry(entry, True)
    build_era_dict[entry].sort()
    for build in build_era_dict[entry]:
        wrapEntry(build)

"""
Output for v41, 12/2020, after some manual sorting:

                <!-- C2C_ERA_PREHISTORIC -->
                <Build>
                    <BuildType>BUILD_ABATIS_THORN_HEDGE_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_BURN_VEGETATION</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_DESERT_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FORTIFIED_CAVE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FORTIFIED_CAVE_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FRUIT_GATHERING_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_GRAPE_GATHERING_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_JUNGLE_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_MUSHROOM_GATHERER_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_NOMADIC_HERD_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PLANT_GATHERING_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SALT_GATHERER_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SCAVENGING_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SEASONAL_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SEED_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_STONE_TOOLS_WORKSHOP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TAR_GATHERER_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TRAIL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WETLANDS_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WOODEN_PALISADE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WOODEN_PALISADE_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WOODEN_WATCHTOWER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WOODEN_WATCHTOWER_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WOOD_GATHERER_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <!-- SPECIAL -->
                <Build>
                    <BuildType>BUILD_MOAI_STATUES</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_ANCIENT -->
                <Build>
                    <BuildType>BUILD_COTTAGE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_DESERT_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_EARLY_MOUNTAIN_MINE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_EARLY_QUARRY</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FARM</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FORTIFIED_CAVE_WITH_CACHE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FRUIT_PICKING_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_GEOGLYPH</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_GRAPE_PICKING_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_HUNTING_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_HUNTING_PRESERVE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_JUNGLE_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_LUMBERJACK</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_MINE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_MUSHROOM_GATHERER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_ORCHARD</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PASTURE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PATH</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PLANT_PICKING_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_QUARRY</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_REMOVE_VEGETATION</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SALT_GATHERER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SEASONAL_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SEED_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SILK_FARM</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_STONE_TOOLS_WORKSHOP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_STONE_WATCHTOWER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TAR_GATHERER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_THATCH_CUTTER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WETLANDS_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WINERY</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_CLASSICAL -->
                <Build>
                    <BuildType>BUILD_DRAIN_SWAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FORT</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_HAMLET</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_LUMBERMILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PAVED_ROAD</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PEAT_CUTTER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PLANTATION</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WATERMILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WORKSHOP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <!-- SPECIAL: DOES NOT OBSOLETE -->
                <Build>
                    <BuildType>BUILD_ROAD</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_MEDIEVAL -->
                <Build>
                    <BuildType>BUILD_MOUNTAIN_MINE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_VILLAGE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WINDMILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_YOUNG_FOREST</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_RENAISSANCE -->
                <Build>
                    <BuildType>BUILD_ARCHEOLOGICAL_SITE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FOSSIL_DIG_SITE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_REMOVE_ROCK_FORMATION</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SHAFT_MINE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TOWN</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_INDUSTRIAL -->
                <Build>
                    <BuildType>BUILD_COMMAND_BUNKER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FACTORY</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FACTORY_COLD_CLIMATE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FOREST_PRESERVE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_HYDROCARBON_WELL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_HYDRO_DAM</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_RADIO_TOWER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_RAILROAD</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_STEEL_WATCHTOWER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SUBURBS</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_ATOMIC -->
                <Build>
                    <BuildType>BUILD_ELECTRIC_RAILROAD</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_GEOTHERMAL_FACTORY</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_GROUNDWATER_WELL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_GROW_FOREST</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_HIGHWAY</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_MANUFACTURING_COMPLEX</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_MODERN_MINE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_MODERN_WINDMILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_NATURE_PRESERVE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PLANT_BAMBOO</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PLANT_JUNGLE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_PLANT_SAVANNA</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_RADAR_TOWER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SCRUB_FALLOUT</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SOLAR_PANELS</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TREEFARM</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WILDLIFE_SANCTUARY</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_INFORMATION -->
                <Build>
                    <BuildType>BUILD_CLEAR_FALLOUT_FAST</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_COMMAND_CENTER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_MAGLEV</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_RECLAIM_LAND</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SANITIZE</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_NANOTECH -->
                <Build>
                    <BuildType>BUILD_HYBRID_FOREST</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_VACTRAIN</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_VERTICAL_FARM</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_TRANSHUMAN -->
                <Build>
                    <BuildType>BUILD_CORE_MINE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_GRAVITY_TRAIN</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_LISTENING_NODE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_DESERT</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_DUNES</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_GRASS</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_LUSH</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_MUDDY</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_PERMAFROST</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_PLAINS</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_ROCKY</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_SCRUB</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_TAIGA</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TERRAFORM_TUNDRA</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_GALACTIC -->
                <Build>
                    <BuildType>BUILD_JUMPLANE</BuildType>
                    <bBuild>1</bBuild>
                </Build>

NAVAL::

                <!-- C2C_ERA_PREHISTORIC -->
                <Build>
                    <BuildType>BUILD_FISHING_BOATS</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_FISHING_CAMP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_HARVEST_KELP_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WHALING_BOATS</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_ANCIENT -->
                <Build>
                    <BuildType>BUILD_BEACON_CORAL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_BEACON_REEF</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_HARVEST_KELP</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_CLASSICAL -->

                <!-- C2C_ERA_MEDIEVAL -->
                <Build>
                    <BuildType>BUILD_FISHING_CAMP</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_LIGHTHOUSE_CORAL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_LIGHTHOUSE_REEF</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_WHALING_SHIPS</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_RENAISSANCE -->

                <!-- C2C_ERA_INDUSTRIAL -->
                <Build>
                    <BuildType>BUILD_BREAK_ICE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_BREAK_ICE_KILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TUNNEL</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                
                <!-- C2C_ERA_ATOMIC -->
                <Build>
                    <BuildType>BUILD_MARINE_PRESERVE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_OFFSHORE_PLATFORM</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SEA_MILL</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_TIDAL_HARNESS</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_INFORMATION -->
                <Build>
                    <BuildType>BUILD_SANITIZE_WATER</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_SUBMERGED_TOWN</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_NANOTECH -->
                <Build>
                    <BuildType>BUILD_AQUATIC_ANTI_AIR_DEFENSE</BuildType>
                    <bBuild>1</bBuild>
                </Build>
                <Build>
                    <BuildType>BUILD_EXTRACTION_FACILITY</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_TRANSHUMAN -->
                <Build>
                    <BuildType>BUILD_AQUATIC_FORCEFIELD</BuildType>
                    <bBuild>1</bBuild>
                </Build>

                <!-- C2C_ERA_GALACTIC -->

"""