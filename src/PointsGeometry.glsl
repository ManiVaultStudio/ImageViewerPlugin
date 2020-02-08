R"(
#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 200) out;

//in pointData vertexOut[1];		// Input points for geometry shader

// Vertex shader output
in PointData {
	vec4 position;
	float radius;
	vec4 color;
} vertexOut[1];

// out pointData frag;			// Output points for fragment shader

uniform mat4 screenToNormalizedScreenMatrix;	// Screen coordinates to normalized screen coordinates matrix

// Constants
#define PI			3.14159265
#define HALF_PI		1.57079
#define TWO_PI		6.283185
#define DEG_TO_RAD	0.01745329
#define RAD_TO_DEG	57.2957786

void main() {
	int noSegments = 32;

	vec4 position	= vertexOut[0].position;
	float radius	= vertexOut[0].radius;
	float radDelta = TWO_PI / float(noSegments);
	
	// Generate vertices
	for (int s = 1; s < noSegments + 1; s++) {
		float thetaPrevious	= (s - 1) * radDelta;
		float thetaCurrent	= s * radDelta;

		vec4 pVertexA		= vec4(radius * cos(thetaPrevious), radius * sin(thetaPrevious), 0.0, 0.0);
		vec4 pVertexB		= vec4(radius * cos(thetaCurrent), radius * sin(thetaCurrent), 0.0, 0.0);

		gl_Position = screenToNormalizedScreenMatrix * position;
		EmitVertex();

		gl_Position = screenToNormalizedScreenMatrix * (position + pVertexA);
		EmitVertex();

		gl_Position = screenToNormalizedScreenMatrix * (position + pVertexB);
		EmitVertex();
	}
	
	EndPrimitive();
}
)"