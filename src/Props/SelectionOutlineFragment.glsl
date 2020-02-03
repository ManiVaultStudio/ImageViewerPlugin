R"(
#version 330

uniform sampler2D stippleTexture;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(stippleTexture, uv);
}
)"