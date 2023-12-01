#version 330

uniform usampler2DArray textures;   // Textures (0: Selection channel, 1: Mask channel)
uniform vec4 overlayColor;          // Selection overlay color
uniform float opacity;              // Render opacity of the layer
uniform bool showRegion;            // Show selection region on/off
in vec2 uv;                         // Input texture coordinates
out vec4 fragmentColor;             // Output fragment

layout(pixel_center_integer) in vec4 gl_FragCoord;

void main(void)
{
    if (texture(textures, vec3(uv, 0)).r > 0u && texture(textures, vec3(uv, 1)).r > 0u)
        fragmentColor = vec4(overlayColor.rgb, opacity);
    else
        fragmentColor = vec4(overlayColor.rgb, 0.f);
}
