R"(
#version 330

uniform sampler2DArray channelTextures;		// Texture samplers (0: Channel 1, 1: Channel 2, 2: Channel 3, 3: Alpha)
uniform vec2 displayRanges[3];				// Display ranges for each channel
uniform float opacity;						// Layer opacity
in vec2 uv;									// Input texture coordinates
out vec4 fragmentColor;						// Output fragment

// Perform channel tone mapping
float toneMapChannel(float minPixelValue, float maxPixelValue, float pixelValue)
{
	float range		= maxPixelValue - minPixelValue;
	float fraction	= pixelValue - minPixelValue;

	return clamp(fraction / range, 0.0, 1.0);
}

void main(void)
{
	float intensity = texture(channelTextures, vec3(uv, 0)).r;

	for (int c = 0; c < 3; c++) {
		fragmentColor[c] = toneMapChannel(displayRanges[0].x, displayRanges[0].y, intensity);
	}

	fragmentColor.a = opacity;
}
)"