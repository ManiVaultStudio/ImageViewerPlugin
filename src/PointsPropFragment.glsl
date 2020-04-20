R"(
#version 330

#extension GL_ARB_gpu_shader_fp64 : enable

uniform sampler2D textures[4];		// Texture samplers (0: Color map, 1: Channel 1, 2: Channel 2, 3: Channel 3)
uniform vec2 displayRanges[3];		// Display ranges for each channel
uniform int noChannels;				// Number of active channels
uniform float opacity;				// Render opacity

in vec2 uv;							// Input texture coordinates
out vec4 fragmentColor;				// Output fragment

// Perform channel tone mapping
float toneMapChannel(float minPixelValue, float maxPixelValue, float pixelValue)
{
	float range		= maxPixelValue - minPixelValue;
	float fraction	= pixelValue - minPixelValue;

	return clamp(fraction / range, 0.0, 1.0);
}

void main(void)
{
	switch (noChannels) {
		case 1:
		{
			// Grab channel(s)
			float channel = toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(textures[1], uv).r);
			
			// Color mapping
			fragmentColor = texture(textures[0], vec2(channel, 0));
			
			break;
		}

		case 2:
		{
			// Grab channel(s)
			float channel1 = toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(textures[1], uv).r);
			float channel2 = toneMapChannel(displayRanges[1].x, displayRanges[1].y, texture(textures[2], uv).r);
			
			// Color mapping
			fragmentColor = texture(textures[0], vec2(channel1, channel2));
			
			break;
		}

		case 3:
		{
			// Grab channel(s)
			fragmentColor.r = toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(textures[1], uv).r);
			fragmentColor.g = toneMapChannel(displayRanges[1].x, displayRanges[1].y, texture(textures[2], uv).r);
			fragmentColor.b = toneMapChannel(displayRanges[2].x, displayRanges[2].y, texture(textures[3], uv).r);
			
			break;
		}
	}
	
	fragmentColor.a = opacity;
}
)"