R"(
#version 330

layout(location = 0) in vec3	position;	// Position vertex attribute
layout(location = 1) in float	radius;		// Radius vertex attribute
layout(location = 2) in vec4	color;		// Color vertex attribute

uniform bool screenCoordinates;				// If true, the supplied position and radius are in screen coordinates, else in world coordinates
uniform mat4 modelViewProjectionMatrix;		// Model-view-projection matrix

// Vertex shader output
out PointData {
	vec4 position;
	float radius;
	vec4 color;
} vertexOut;

void main(void)
{
	if (screenCoordinates) {
		vertexOut.position = vec4(position, 1.0);
	} else {
		vertexOut.position = modelViewProjectionMatrix * vec4(position, 1.0);
	}

	vertexOut.radius	= radius;
	vertexOut.color		= color;
}
)"