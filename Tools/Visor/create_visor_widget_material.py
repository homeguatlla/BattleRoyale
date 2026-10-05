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
    material = unreal.load_asset(full_path)
    lib.delete_all_material_expressions(material)
else:
    material = asset_tools.create_asset(ASSET_NAME, PACKAGE_PATH, unreal.Material, unreal.MaterialFactoryNew())
material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
material.set_editor_property("disable_depth_test", True)
material.set_editor_property("translucency_pass", unreal.MaterialTranslucencyPass.MTP_AFTER_MOTION_BLUR)

# Being drawn after TSR the visor gets no anti-aliasing, and the widget render target has no
# mips, so thin slanted lines alias. The custom node box-filters the render target over the
# screen pixel footprint (4x4 taps, using the UV derivatives), i.e. hand-made supersampling AA.
HLSL = r"""
float2 dx = ddx(UV);
float2 dy = ddy(UV);
float4 sum = 0;
for (int i = 0; i < 4; i++)
{
    for (int j = 0; j < 4; j++)
    {
        float2 offset = dx * ((i + 0.5) / 4.0 - 0.5) + dy * ((j + 0.5) / 4.0 - 0.5);
        sum += Texture2DSampleLevel(Tex, TexSampler, UV + offset, 0);
    }
}
float4 color = sum / 16.0;
color.rgb *= Brightness;
return color;
"""

# WidgetComponent feeds its render target through the "SlateUI" texture parameter.
slate_ui = lib.create_material_expression(material, unreal.MaterialExpressionTextureObjectParameter, -800, 0)
slate_ui.set_editor_property("parameter_name", "SlateUI")
slate_ui.set_editor_property("texture", unreal.load_asset("/Engine/EngineResources/WhiteSquareTexture"))

uv = lib.create_material_expression(material, unreal.MaterialExpressionTextureCoordinate, -800, 200)

brightness = lib.create_material_expression(material, unreal.MaterialExpressionScalarParameter, -800, 320)
brightness.set_editor_property("parameter_name", "Brightness")
brightness.set_editor_property("default_value", 1.0)

custom = lib.create_material_expression(material, unreal.MaterialExpressionCustom, -400, 0)
custom.set_editor_property("code", HLSL)
custom.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT4)
custom.set_editor_property("description", "VisorSupersampleAA")
inputs = []
for input_name in ["Tex", "UV", "Brightness"]:
    custom_input = unreal.CustomInput()
    custom_input.set_editor_property("input_name", input_name)
    inputs.append(custom_input)
custom.set_editor_property("inputs", inputs)
lib.connect_material_expressions(slate_ui, "", custom, "Tex")
lib.connect_material_expressions(uv, "", custom, "UV")
lib.connect_material_expressions(brightness, "", custom, "Brightness")

rgb = lib.create_material_expression(material, unreal.MaterialExpressionComponentMask, -150, -60)
for channel, enabled in (("r", True), ("g", True), ("b", True), ("a", False)):
    rgb.set_editor_property(channel, enabled)
alpha = lib.create_material_expression(material, unreal.MaterialExpressionComponentMask, -150, 80)
for channel, enabled in (("r", False), ("g", False), ("b", False), ("a", True)):
    alpha.set_editor_property(channel, enabled)
lib.connect_material_expressions(custom, "", rgb, "")
lib.connect_material_expressions(custom, "", alpha, "")

lib.connect_material_property(rgb, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
lib.connect_material_property(alpha, "", unreal.MaterialProperty.MP_OPACITY)

lib.recompile_material(material)
unreal.EditorAssetLibrary.save_asset(full_path)
unreal.log(f"Created {full_path}")
