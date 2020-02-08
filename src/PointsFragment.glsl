R"(
#version 330

in vec4 geometryColor;		// Output from geometry shader
out vec4 fragmentColor;		// Output fragment color

void main(void)
{
	fragmentColor = geometryColor;
}
)"