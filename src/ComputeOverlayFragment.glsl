R"(
#version 330

in vec2 uv;
out vec4 fragmentColor;

uniform float brushRadius;
uniform float maxPixelValue;

void main(void)
{
	fragmentColor = uv.x > 0.5f ? vec4(vec3(1, 0, 0), 1) : vec4(vec3(0, 1, 0), 1);
}
)"