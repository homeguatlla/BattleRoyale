"""
Imports the visor HUD fonts (Tools/Visor/Fonts, SIL Open Font License) as Font Face assets in
/Game/Fonts/Visor. The HUD builds its composite fonts from these faces at runtime
(see VisorHealthWidget.cpp), so no Font asset has to be authored by hand.

Run it from the Unreal Editor: Tools > Execute Python Script...
"""
import os
import unreal

DESTINATION = "/Game/Fonts/Visor"
FONTS = [
    "ChakraPetch-Bold.ttf",
    "ChakraPetch-SemiBold.ttf",
    "BarlowSemiCondensed-SemiBold.ttf",
    "BarlowSemiCondensed-Medium.ttf",
]

source_dir = os.path.join(unreal.Paths.project_dir(), "Tools", "Visor", "Fonts")
tasks = []
for font in FONTS:
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", os.path.abspath(os.path.join(source_dir, font)))
    task.set_editor_property("destination_path", DESTINATION)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    tasks.append(task)

unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)
for task in tasks:
    for path in task.get_editor_property("imported_object_paths"):
        unreal.log(f"Imported {path}")
