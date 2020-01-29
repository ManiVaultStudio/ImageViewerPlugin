R"(
#version 330

uniform sampler2D lineTexture;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(lineTexture, uv);
}
)"