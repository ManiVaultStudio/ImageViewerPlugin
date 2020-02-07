R"(
#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 100) out;

out vec4 g_color;

uniform mat4 screenToNormalizedScreenMatrix;		// Projection matrix

#define PI 3.1415926535897932384626433832795
#define TWO_PI 2.0f * 3.1415926535897932384626433832795

void main() {
	int noSegments = 8;
	float brushRadius = 0.02;

	vec4 center = gl_in[0].gl_Position;

	for (int s = 1; s < noSegments + 1; s++) {
		float thetaPrevious	= (TWO_PI * float(s - 1)) / float(noSegments);
		float thetaCurrent	= (TWO_PI * float(s)) / float(noSegments);
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