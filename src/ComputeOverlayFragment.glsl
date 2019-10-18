R"(
#version 330

#extension GL_ARB_separate_shader_objects : enable

in vec2 uv;

uniform sampler2D pixelSelectionTexture;

//layout(location = 0) in vec4 vertex;
//layout(location = 0) in vec4 pixelSelectionIn;
//layout(location = 1) out vec4 pixelSelectionOut;

out vec4 fragmentColor;

uniform vec2 brushCenter;
uniform float brushRadius;

void main(void)
{
	float distance = length(uv - brushCenter);
	fragmentColor = (distance < 0.1f || texture(pixelSelectionTexture, vec2(uv.x, 1-uv.y)).r > 0) ? vec4(vec3(1, 0, 0), 1) : vec4(vec3(0, 1, 0), 1);
}
)"