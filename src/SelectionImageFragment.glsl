R"(
#version 330

uniform sampler2D imageTexture;
uniform vec4 color;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(imageTexture, uv).r > 0 ? color : vec4(0);
}
)"