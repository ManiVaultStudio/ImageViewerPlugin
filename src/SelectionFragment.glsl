R"(
#version 330

uniform sampler2D selectionTexture;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	float value = float(texture(selectionTexture, uv).r);
	fragmentColor = value > 0 ? vec4(1, 0, 0, 1) : vec4(0);
}
)"