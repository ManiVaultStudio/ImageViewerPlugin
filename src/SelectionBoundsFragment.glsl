R"(
#version 330

uniform vec4 color;

out vec4 fragmentColor;

void main(void)
{
	fragmentColor = color;
}
)"