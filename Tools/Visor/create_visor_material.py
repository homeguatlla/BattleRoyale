"""
Creates /Game/Core/UI/Visor/M_VisorWarp, the RetainerBox effect material used by the
visor HUD prototype (UVisorWarpWidget, technique 2).

Run it from the Unreal Editor: Tools > Execute Python Script... (or `py <path>` in the
Output Log console). Needs the "Python Editor Script Plugin", enabled by default in UE5.

Parameters exposed to the prototype:
  Texture     - the RetainerBox render target (set by code)
  Curvature   - barrel warp, how much the edges bend like a visor
  Scanlines   - intensity of the hologram scanlines
  Aberration  - chromatic aberration towards the edges
"""
import unreal

PACKAGE_PATH = "/Game/Core/UI/Visor"
ASSET_NAME = "M_VisorWarp"

HLSL = r"""
float2 c = UV - 0.5;
float r2 = dot(c, c);
float2 warped = 0.5 + c * (1.0 + Curvature * r2);
if (warped.x < 0.0 || warped.x > 1.0 || warped.y < 0.0 || warped.y > 1.0)
{
    return float4(0, 0, 0, 0);
}
float2 offset = c * Aberration;
float4 center = Texture2DSample(Tex, TexSampler, warped);
float red = Texture2DSample(Tex, TexSampler, warped + offset).r;
float blue = Texture2DSample(Tex, TexSampler, warped - offset).b;
float4 color = float4(red, center.g, blue, center.a);
float scan = 1.0 - Scanlines * (0.5 + 0.5 * sin(UV.y * 1400.0));
color.rgb *= scan;
return color;
"""

lib = unreal.MaterialEditingLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
full_path = f"{PACKAGE_PATH}/{ASSET_NAME}"

if unreal.EditorAssetLibrary.does_asset_exist(full_path):
    unreal.EditorAssetLibrary.delete_asset(full_path)

material = asset_tools.create_asset(ASSET_NAME, PACKAGE_PATH, unreal.Material, unreal.MaterialFactoryNew())
material.set_editor_property("material_domain", unreal.MaterialDomain.MD_UI)
material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)

texture = lib.create_material_expression(material, unreal.MaterialExpressionTextureObjectParameter, -800, 0)
texture.set_editor_property("parameter_name", "Texture")
texture.set_editor_property("texture", unreal.load_asset("/Engine/EngineResources/WhiteSquareTexture"))

uv = lib.create_material_expression(material, unreal.MaterialExpressionTextureCoordinate, -800, 200)


def scalar(name, value, y):
    node = lib.create_material_expression(material, unreal.MaterialExpressionScalarParameter, -800, y)
    node.set_editor_property("parameter_name", name)
    node.set_editor_property("default_value", value)
    return node


curvature = scalar("Curvature", 0.12, 320)
scanlines = scalar("Scanlines", 0.06, 440)
aberration = scalar("Aberration", 0.0015, 560)

custom = lib.create_material_expression(material, unreal.MaterialExpressionCustom, -400, 0)
custom.set_editor_property("code", HLSL)
custom.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT4)
custom.set_editor_property("description", "VisorWarp")
inputs = []
for input_name in ["Tex", "UV", "Curvature", "Scanlines", "Aberration"]:
    custom_input = unreal.CustomInput()
    custom_input.set_editor_property("input_name", input_name)
    inputs.append(custom_input)
custom.set_editor_property("inputs", inputs)

lib.connect_material_expressions(texture, "", custom, "Tex")
lib.connect_material_expressions(uv, "", custom, "UV")
lib.connect_material_expressions(curvature, "", custom, "Curvature")
lib.connect_material_expressions(scanlines, "", custom, "Scanlines")
lib.connect_material_expressions(aberration, "", custom, "Aberration")

rgb = lib.create_material_expression(material, unreal.MaterialExpressionComponentMask, -150, -60)
rgb.set_editor_property("r", True)
rgb.set_editor_property("g", True)
rgb.set_editor_property("b", True)
rgb.set_editor_property("a", False)
alpha = lib.create_material_expression(material, unreal.MaterialExpressionComponentMask, -150, 80)
alpha.set_editor_property("r", False)
alpha.set_editor_property("g", False)
alpha.set_editor_property("b", False)
alpha.set_editor_property("a", True)
lib.connect_material_expressions(custom, "", rgb, "")
lib.connect_material_expressions(custom, "", alpha, "")

lib.connect_material_property(rgb, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
lib.connect_material_property(alpha, "", unreal.MaterialProperty.MP_OPACITY)

lib.recompile_material(material)
unreal.EditorAssetLibrary.save_asset(full_path)
unreal.log(f"Created {full_path}")
