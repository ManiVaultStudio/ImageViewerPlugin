R"(
#version 330

#extension GL_ARB_gpu_shader_fp64 : enable

uniform sampler2D textures[4];		// Texture samplers (0: Color map, 1: Channel 1, 2: Channel 2, 3: Channel 3)
uniform vec2 displayRanges[3];		// Display ranges for each channel
uniform int noChannels;				// Number of active channels
uniform float opacity;				// Render opacity

in vec2 uv;
out vec4 fragmentColor;

float toneMapChannel(float minPixelValue, float maxPixelValue, float pixelValue)
{
	float range		= maxPixelValue - minPixelValue;
	float fraction	= pixelValue - minPixelValue;

	return clamp(fraction / range, 0.0, 1.0);
}

void main(void)
{
	float channel1 = toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(textures[1], uv).r);

	fragmentColor = texture(textures[0], vec2(channel1, 0));
	//fragmentColor.r = channel1;
	fragmentColor.a = opacity;
}
)"