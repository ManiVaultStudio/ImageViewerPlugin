R"(
#version 330

uniform sampler2D selectionTexture;
uniform vec4 color;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(selectionTexture, uv).r > 0 ? color : vec4(0);
}
)"