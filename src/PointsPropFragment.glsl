R"(
#version 330

uniform sampler2D colorMapTexture;		// Colormap texture sampler
uniform sampler2DArray channelTextures;	// Texture samplers (0: Channel 1, 1: Channel 2, 2: Channel 3, 3: Alpha)
uniform vec2 displayRanges[4];			// Display ranges for each channel
uniform int noChannels;					// Number of active channels
uniform bool invertAlpha;				// Invert the alpha channel
uniform float opacity;					// Render opacity

in vec2 uv;								// Input texture coordinates
out vec4 fragmentColor;					// Output fragment

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
			float channel = toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(channelTextures, vec3(uv, 0)).r);
			
			// Color mapping
			fragmentColor = texture(colorMapTexture, vec2(channel, 0));
			
			break;
		}

		case 2:
		{
			// Grab channel(s)
			float channel1 = toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(channelTextures, vec3(uv, 0)).r);
			float channel2 = toneMapChannel(displayRanges[1].x, displayRanges[1].y, texture(channelTextures, vec3(uv, 1)).r);
			
			// Color mapping
			fragmentColor = texture(colorMapTexture, vec2(channel1, channel2));
			
			break;
		}

		case 3:
		{
			// Grab channel(s)
			fragmentColor.r = toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(channelTextures, vec3(uv, 0)).r);
			fragmentColor.g = toneMapChannel(displayRanges[1].x, displayRanges[1].y, texture(channelTextures, vec3(uv, 1)).r);
			fragmentColor.b = toneMapChannel(displayRanges[2].x, displayRanges[2].y, texture(channelTextures, vec3(uv, 2)).r);
			
			break;
		}
	}
	
	// Grab alpha channel
	float alphaChannel = toneMapChannel(displayRanges[3].x, displayRanges[3].y, texture(channelTextures, vec3(uv, 3)).r);

	if (invertAlpha)
		alphaChannel = 1.0f - alphaChannel;

	fragmentColor.a = opacity;
}
)"