R"(
#version 330

uniform sampler2D overlayTexture;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(overlayTexture, uv).r > 0 ? vec4(vec3(1, 0, 0), 0.4) : vec4(0);
}
)"