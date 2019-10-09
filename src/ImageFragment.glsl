R"(
#version 330

uniform sampler2D imageTexture;
uniform float minPixelValue;
uniform float maxPixelValue;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	float value		= texture(imageTexture, uv).r * 65535.0;
	float fraction	= value - minPixelValue;
	float range		= maxPixelValue - minPixelValue;
	float clamped	= clamp(fraction / range, 0.0, 1.0);
	fragmentColor	= vec4(1);//vec4(clamped, clamped, clamped, 1.0);
}
)"