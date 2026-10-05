"""
Creates /Game/Core/UI/Visor/M_VisorCylinder, the RetainerBox effect material for the visor HUD
(UVisorPrototype technique 3).

The HUD is drawn by Slate in 2D (exact colours, native resolution, after tonemapping) and this
material re-projects it as if it were printed on a cylinder in front of the camera, which is
what the 3D WidgetComponent technique looks like, plus a soft glow.

For every screen pixel it casts the camera ray, intersects it with the cylinder (camera at the
origin looking +X, cylinder axis vertical, concave side towards the camera, centre of the widget
at distance 1) and samples the flat widget at the arc/height coordinates of the hit point.

Parameters set from code: TanHalfFov, Aspect, ArcAngle (radians), GlowStrength, GlowRadius (px).
Run it from the Unreal Editor: Tools > Execute Python Script...
"""
import unreal

PACKAGE_PATH = "/Game/Core/UI/Visor"
ASSET_NAME = "M_VisorCylinder"

HLSL = r"""
// Screen pixel -> camera ray
float dy = (UV.x * 2.0 - 1.0) * TanHalfFov;
float dz = (1.0 - UV.y * 2.0) * TanHalfFov / Aspect;

// Widget printed on a cylinder: arc length L covers the screen width at distance 1
float L = 2.0 * TanHalfFov;
float2 widgetUV = UV;
if (ArcAngle > 0.001)
{
    float R = L / ArcAngle;
    float cx = 1.0 - R;
    float a = 1.0 + dy * dy;
    float disc = cx * cx - a * (cx * cx - R * R);
    if (disc < 0.0)
    {
        return float4(0, 0, 0, 0);
    }
    float t = (cx + sqrt(disc)) / a;
    float phi = atan2(t * dy, t - cx);
    widgetUV.x = 0.5 + (R * phi) / L;
    widgetUV.y = 0.5 - (t * dz) / (L / Aspect);
}
if (widgetUV.x < 0.0 || widgetUV.x > 1.0 || widgetUV.y < 0.0 || widgetUV.y > 1.0)
{
    return float4(0, 0, 0, 0);
}

// Light 2x2 box filter over the pixel footprint (the warp minifies a little near the edges)
float2 du = ddx(widgetUV);
float2 dv = ddy(widgetUV);
float4 color = 0;
color += Texture2DSampleLevel(Tex, TexSampler, widgetUV + (du + dv) * 0.25, 0);
color += Texture2DSampleLevel(Tex, TexSampler, widgetUV + (du - dv) * 0.25, 0);
color += Texture2DSampleLevel(Tex, TexSampler, widgetUV + (-du + dv) * 0.25, 0);
color += Texture2DSampleLevel(Tex, TexSampler, widgetUV + (-du - dv) * 0.25, 0);
color *= 0.25;

// Glow: blurred copy of the HUD added under it
float2 pixel = float2(length(du), length(dv));
float4 glow = 0;
const int TAPS = 12;
for (int i = 0; i < TAPS; i++)
{
    float angle = 6.2831853 * i / TAPS;
    float2 dir = float2(cos(angle), sin(angle));
    glow += Texture2DSampleLevel(Tex, TexSampler, widgetUV + dir * pixel * GlowRadius, 0);
    glow += Texture2DSampleLevel(Tex, TexSampler, widgetUV + dir * pixel * GlowRadius * 0.5, 0);
}
glow /= (TAPS * 2.0);
float glowAlpha = glow.a * GlowStrength;
float3 glowColor = glow.a > 0.001 ? glow.rgb / glow.a : float3(0, 0, 0);

float outAlpha = color.a + glowAlpha * (1.0 - color.a);
float3 outColor = outAlpha > 0.001 ? (color.rgb * color.a + glowColor * glowAlpha * (1.0 - color.a)) / outAlpha : float3(0, 0, 0);
return float4(outColor, saturate(outAlpha));
"""

lib = unreal.MaterialEditingLibrary
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
full_path = f"{PACKAGE_PATH}/{ASSET_NAME}"

if unreal.EditorAssetLibrary.does_asset_exist(full_path):
    material = unreal.load_asset(full_path)
    lib.delete_all_material_expressions(material)
else:
    material = asset_tools.create_asset(ASSET_NAME, PACKAGE_PATH, unreal.Material, unreal.MaterialFactoryNew())
material.set_editor_property("material_domain", unreal.MaterialDomain.MD_UI)
material.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)

texture = lib.create_material_expression(material, unreal.MaterialExpressionTextureObjectParameter, -900, 0)
texture.set_editor_property("parameter_name", "Texture")
texture.set_editor_property("texture", unreal.load_asset("/Engine/EngineResources/WhiteSquareTexture"))

uv = lib.create_material_expression(material, unreal.MaterialExpressionTextureCoordinate, -900, 200)

custom = lib.create_material_expression(material, unreal.MaterialExpressionCustom, -400, 0)
custom.set_editor_property("code", HLSL)
custom.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT4)
custom.set_editor_property("description", "VisorCylinder")

scalars = [("TanHalfFov", 1.0), ("Aspect", 1.7777), ("ArcAngle", 0.785), ("GlowStrength", 0.9), ("GlowRadius", 10.0)]
input_names = ["Tex", "UV"] + [name for name, _ in scalars]
inputs = []
for input_name in input_names:
    custom_input = unreal.CustomInput()
    custom_input.set_editor_property("input_name", input_name)
    inputs.append(custom_input)
custom.set_editor_property("inputs", inputs)

lib.connect_material_expressions(texture, "", custom, "Tex")
lib.connect_material_expressions(uv, "", custom, "UV")
for index, (name, value) in enumerate(scalars):
    node = lib.create_material_expression(material, unreal.MaterialExpressionScalarParameter, -900, 320 + index * 110)
    node.set_editor_property("parameter_name", name)
    node.set_editor_property("default_value", value)
    lib.connect_material_expressions(node, "", custom, name)

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
