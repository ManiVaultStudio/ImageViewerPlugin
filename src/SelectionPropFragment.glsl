R"(
#version 330

uniform sampler2D imageTexture;
uniform float opacity;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	if (texture(imageTexture, uv).r > 0)
		fragmentColor= vec4(vec3(0, 1, 0), opacity);
}
)"