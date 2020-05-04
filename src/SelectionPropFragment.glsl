R"(
#version 330

uniform sampler2D imageTexture;			// Selection image texture
uniform vec4 overlayColor;				// Selection overlay color
uniform float opacity;					// Render opacity of selected data points
in vec2 uv;								// Input texture coordinates
out vec4 fragmentColor;					// Output fragment

void main(void)
{
	if (texture(imageTexture, uv).r > 0) {
		fragmentColor= vec4(overlayColor.rgb, opacity);
	}
}
)"