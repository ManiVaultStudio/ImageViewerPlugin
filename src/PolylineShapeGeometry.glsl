R"(
#version 330 core
layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 4) out;

out vec4 g_color;

uniform mat4	screenSpaceToClipSpace;		// Projection matrix
uniform float	lineWidth;					// Line width

vec3 halfAngle(vec3 vA, vec3 vB) 
{ 
    return normalize((vA + vB) * 0.5);
}

vec3 halfAngle(vec3 pA, vec3 pB, vec3 pC) 
{ 
    return halfAngle(normalize(pA - pB), normalize(pC - pB));
}

void main() {
	// Viewport pop
    vec3 pPrevious	= gl_in[0].gl_Position.xyz;
    vec3 pStart		= gl_in[1].gl_Position.xyz;
    vec3 pEnd		= gl_in[2].gl_Position.xyz;
    vec3 pNext		= gl_in[3].gl_Position.xyz;

	vec3 vBiStart	= halfAngle(pPrevious, pStart, pEnd);
	vec3 vBiEnd		= halfAngle(pStart, pEnd, pNext);

	bool colStart	= length(pStart - pPrevious) < 0.0001;
	bool colEnd		= length(pEnd - pNext) < 0.0001;

	vec3 lhs		= cross(normalize(pEnd - pStart), vec3(0.0, 0.0, -1.0));
	vec3 startLhs	= vBiStart * sign(dot(vBiStart, lhs));
	vec3 endLhs		= vBiEnd * sign(dot(vBiEnd, lhs));

	if(colStart)
		startLhs = lhs;
	if(colEnd)
		endLhs = lhs;

	float startInvScale = dot(startLhs, lhs);
	float endInvScale = dot(endLhs, lhs);

	float halfLineWidth = 0.5 * lineWidth;

	startLhs	*= halfLineWidth;
	endLhs		*= halfLineWidth;

	vec3 vOffsetStart	= startLhs / startInvScale;
	vec3 vOffsetEnd		= endLhs / endInvScale;

	gl_Position		= screenSpaceToClipSpace * vec4(pStart + vOffsetStart, 1.0);
	g_color			= vec4(1);
	EmitVertex();
	
	gl_Position		= screenSpaceToClipSpace * vec4(pStart - vOffsetStart, 1.0);
	g_color			= vec4(1);
	EmitVertex();
	
	gl_Position		= screenSpaceToClipSpace * vec4(pEnd + vOffsetEnd, 1.0);
	g_color			= vec4(1);
	EmitVertex();

	gl_Position		= screenSpaceToClipSpace * vec4(pEnd - vOffsetEnd, 1.0);
	g_color			= vec4(1);
	EmitVertex();

    EndPrimitive();
}
)"