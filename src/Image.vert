R"(
#version 330

in vec4 vertex;

void main( void )
{
 gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
)" 