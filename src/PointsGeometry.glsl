R"(
#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 128) out;

// Vertex shader output
in PointData {
	vec4 position;
	float radius;
	vec4 color;
} vertexOut[1];

// Geometry shader output
out vec4 geometryColor;

uniform int		noSegments;					// No segments
uniform mat4	screenToNormalizedScreen;	// Screen coordinates to normalized screen coordinates matrix

// Constants
#define PI			3.14159265
#define HALF_PI		1.57079
#define TWO_PI		6.283185
#define DEG_TO_RAD	0.01745329
#define RAD_TO_DEG	57.2957786

void main() {
	vec4 center		= vertexOut[0].position;
	float radius	= vertexOut[0].radius;
	vec4 color		= vertexOut[0].color;

	// Add center vertex
	gl_Position		= screenToNormalizedScreen * center;
	geometryColor	= color;
	EmitVertex();

	// Compute angle increment
	float thetaDelta = TWO_PI / float(noSegments);
	
	// Generate vertices
	for (int s = 0; s < noSegments + 1; s++) {

		// Add center vertex
		gl_Position		= screenToNormalizedScreen * center;
		geometryColor	= color;
		EmitVertex();

		// Compute current vertex angle
		float theta = s * thetaDelta;

		// Compute outer vertex position
		vec4 position = vec4(radius * cos(theta), radius * sin(theta), 0.0, 0.0);

		// Add outer vertex
		gl_Position		= screenToNormalizedScreen * (center + position);
		geometryColor	= color;
		EmitVertex();
	}

	EndPrimitive();
}
)"