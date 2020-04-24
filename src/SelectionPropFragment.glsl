R"(
#version 330

uniform sampler2D imageTexture;			// Selection image texture
uniform sampler2D colorMapTexture;		// Colormap texture sampler
uniform float opacity;					// Render opacity of selected data points
in vec2 uv;								// Input texture coordinates
out vec4 fragmentColor;					// Output fragment

void main(void)
{
	if (texture(imageTexture, uv).r > 0) {
		//vec4 color = texture(colorMapTexture, vec2(0, 0));
		vec3 color = vec3(0, 1, 0);
		fragmentColor= vec4(color, opacity);
	}
}
)"