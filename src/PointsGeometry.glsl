R"(
#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 200) out;

out vec4 g_color;

uniform mat4 screenToNormalizedScreenMatrix;		// Projection matrix

#define PI 3.14159265
#define HALF_PI 1.57079
#define TWO_PI 6.283185
#define DEG_TO_RAD 0.01745329
#define RAD_TO_DEG 57.2957786

void main() {
	int noSegments = 32;
	float brushRadius = 3.0;

	vec4 center = gl_in[0].gl_Position;
	float radDelta = TWO_PI / float(noSegments);

	for (int s = 1; s < noSegments + 1; s++) {
		float thetaPrevious	= (s - 1) * radDelta;
		float thetaCurrent	= s * radDelta;
		vec4 pVertexA		= vec4(brushRadius * cos(thetaPrevious), brushRadius * sin(thetaPrevious), 0.0, 0.0);
		vec4 pVertexB		= vec4(brushRadius * cos(thetaCurrent), brushRadius * sin(thetaCurrent), 0.0, 0.0);

		gl_Position = screenToNormalizedScreenMatrix * center;
		EmitVertex();

		gl_Position = screenToNormalizedScreenMatrix * (center + pVertexA);
		EmitVertex();

		gl_Position = screenToNormalizedScreenMatrix * (center + pVertexB);
		EmitVertex();
	}
	
	EndPrimitive();
}
)"