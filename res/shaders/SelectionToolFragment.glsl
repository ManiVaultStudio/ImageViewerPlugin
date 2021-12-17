#version 330

uniform sampler2D offScreenTexture;     // Selection image texture
uniform vec4 color;                     // Selection overlay color
uniform float opacity;                  // Render opacity

in vec2 uv;                             // Input texture coordinates
out vec4 fragmentColor;                 // Output fragment

void main(void)
{
//    if (texture(offScreenTexture, uv).r > 0)
//        fragmentColor= color;
//    else 
//        fragmentColor= vec4(0);
}
