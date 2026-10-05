"""
Creates /Game/Core/UI/Visor/M_VisorWidget, the material used by the visor WidgetComponent
(UVisorPrototype technique 1).

The default widget material is drawn like any other translucent object, so temporal
anti-aliasing (TSR/TAA) jitters it every frame and small text shimmers. This material:
  - is drawn in the "After Motion Blur" translucency pass, after TSR/TAA and motion blur,
    so the HUD stays sharp and stable
  - disables the depth test, so the visor never gets hidden by walls close to the camera

Run it from the Unreal Editor: Tools > Execute Python Script...
"""
import unreal

PACKAGE_PATH = "/Game/Core/UI/Visor"
ASSET_NAME = "M_VisorWidget"

lib = unreal.MaterialEditingLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
full_path = f"{PACKAGE_PATH}/{ASSET_NAME}"

if unreal.EditorAssetLibrary.does_asset_exist(full_path):
    unreal.EditorAssetLibrary.delete_asset(full_path)

material = asset_tools.create_asset(ASSET_NAME, PACKAGE_PATH, unreal.Material, unreal.MaterialFactoryNew())
material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
material.set_editor_property("disable_depth_test", True)
material.set_editor_property("translucency_pass", unreal.MaterialTranslucencyPass.MTP_AFTER_MOTION_BLUR)

# WidgetComponent feeds its render target through the "SlateUI" texture parameter.
slate_ui = lib.create_material_expression(material, unreal.MaterialExpressionTextureSampleParameter2D, -600, 0)
slate_ui.set_editor_property("parameter_name", "SlateUI")
slate_ui.set_editor_property("texture", unreal.load_asset("/Engine/EngineResources/WhiteSquareTexture"))

brightness = lib.create_material_expression(material, unreal.MaterialExpressionScalarParameter, -600, 260)
brightness.set_editor_property("parameter_name", "Brightness")
brightness.set_editor_property("default_value", 1.0)

multiply = lib.create_material_expression(material, unreal.MaterialExpressionMultiply, -250, 0)
lib.connect_material_expressions(slate_ui, "RGB", multiply, "A")
lib.connect_material_expressions(brightness, "", multiply, "B")

lib.connect_material_property(multiply, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
lib.connect_material_property(slate_ui, "A", unreal.MaterialProperty.MP_OPACITY)

lib.recompile_material(material)
unreal.EditorAssetLibrary.save_asset(full_path)
unreal.log(f"Created {full_path}")
