#version 330

uniform usampler2DArray channelTextures;    // Integer texture samplers (0: Selection)
uniform vec2 textureSize;                   // Size of the textures in pixels
uniform vec4 overlayColor;                  // Selection overlay color
uniform float opacity;                      // Render opacity of the layer
uniform vec2 topLeft;                       // Selection boundaries top-left
uniform vec2 bottomRight;                   // Selection boundaries bottom-right
uniform bool showRegion;                    // Show selection region on/off
in vec2 uv;                                 // Input texture coordinates
out vec4 fragmentColor;                     // Output fragment

layout(pixel_center_integer) in vec4 gl_FragCoord;

void main(void)
{
    vec2 offset = vec2(1.0f) / (2.0f * textureSize);

    // Compute texel UV position in absolute texture coordinates
    vec2 texelUv = (uv - vec2(0.00001f)) * textureSize;

    texelUv.x = floor(texelUv.x);
    texelUv.y = floor(texelUv.y);

    // Within selection boundaries
    if (texelUv.x >= topLeft.x && texelUv.x <= bottomRight.x && texelUv.y >= topLeft.y && texelUv.y <= bottomRight.y) {

        // Determine whether the pixel is selected
        bool selected = texelFetch(channelTextures, ivec3(texelUv, 0), 0).r > 0u ? true : false;

        if (texture(channelTextures, vec3(uv, 0)).r > 0u)
            fragmentColor= vec4(overlayColor.rgb, opacity);
        else
            fragmentColor = vec4(overlayColor.rgb, showRegion ? 0.75f * opacity : 0.0f);
    } else {
        fragmentColor = vec4(0);
    }
}
