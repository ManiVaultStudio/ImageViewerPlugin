R"(
#version 330
in vec2 v_texCoords;

out vec4 o_fragColor;

uniform sampler2D image;

void main()
{
	o_fragColor = texture(image, v_texCoords);
}

/*
#version 330

layout(location = 0, index = 0) out vec4 fragColor;

void main( void )
{
 fragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}


#version 430 core

uniform sampler2D image;
uniform float minPixelValue;
uniform float maxPixelValue;

in vec2 fragCoord;
out vec4 fragmentColor;

void main() {
	float value = texture(image, fragCoord).r * 65535.0;
	float fraction = value - minPixelValue;
	float range = maxPixelValue - minPixelValue;
	float clamped = clamp(fraction / range, 0.0, 1.0);
	fragmentColor = vec4(clamped, clamped, clamped, 1.0);
};
*/

)"