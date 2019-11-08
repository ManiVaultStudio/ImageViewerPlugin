R"(
#version 330

uniform sampler2D imageTexture;
uniform float minPixelValue;
uniform float maxPixelValue;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(imageTexture, uv);

	float range	= maxPixelValue - minPixelValue;

	for (int c = 0; c < 3; c++)
	{
		float fraction		= (fragmentColor[c] * 65535.0f) - minPixelValue;
		fragmentColor[c]	= clamp(fraction / range, 0.0, 1.0);
	}
}
)"