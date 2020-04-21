R"(
#version 330

uniform sampler2D colorMapTexture;		// Colormap texture sampler
uniform sampler2DArray channelTextures;	// Texture samplers (0: Channel 1, 1: Channel 2, 2: Channel 3, 3: Alpha)
uniform vec2 displayRanges[3];			// Display ranges for each channel
uniform int noChannels;					// Number of active channels
uniform bool solidColor;				// Solid color (zero dimensional color map)
uniform int colorSpace;					// Color space (0: RGB, 1: HSL)
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

// Floating point modulo
float fmodf(float x, float y)
{
return x - y * floor(x / y);
}

// Convert from HSL to RGB color space 
vec3 hslToRgb(float hue, float saturation, float lightness)
{
    vec3 rgb1, rgbResult;

    float chroma = ( 1.0 - (float) abs( 2.0 * lightness - 1.0 ) ) * saturation;
    float h1 = hue / 60.0;
	float x = chroma * ( 1.0 - (float) abs( fmodf(h1, 2.0) - 1.0 ));

    if ( ( 0 <= h1 ) && ( h1 < 1 ) ) {

        rgb1.r = chroma;
        rgb1.g = x;
        rgb1.b = 0.0;

    } else if ( ( 1 <= h1 ) && ( h1 < 2 ) ) {

        rgb1.r = x;
        rgb1.g = chroma;
        rgb1.b = 0.0;

    } else if ( ( 2 <= h1 ) && ( h1 < 3 ) ) {

        rgb1.r = 0.0;
        rgb1.g = chroma;
        rgb1.b = x;

    } else if ( ( 3 <= h1 ) && ( h1 < 4 ) ) {

        rgb1.r = 0.0;
        rgb1.g = x;
        rgb1.b = chroma;

    } else if ( ( 4 <= h1 ) && ( h1 < 5 ) ) {

        rgb1.r = x;
        rgb1.g = 0.0;
        rgb1.b = chroma;

    } else if ( ( 5 <= h1 ) && ( h1 < 6 ) ) {

        rgb1.r = chroma;
        rgb1.g = 0;
        rgb1.b = x;

    } else {

        rgb1.r = 0.0;
        rgb1.g = 0.0;
        rgb1.b = 0.0;

    }

    float m = lightness - 0.5 * chroma;

    rgbResult.r = rgb1.r + m;
    rgbResult.g = rgb1.g + m;
    rgbResult.b = rgb1.b + m;

    return rgbResult;
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
			// Channels before color space conversion
			vec3 channels;

			// Grab channel(s)
			channels.r = toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(channelTextures, vec3(uv, 0)).r);
			channels.g = toneMapChannel(displayRanges[1].x, displayRanges[1].y, texture(channelTextures, vec3(uv, 1)).r);
			channels.b = toneMapChannel(displayRanges[2].x, displayRanges[2].y, texture(channelTextures, vec3(uv, 2)).r);

			switch (colorSpace) {
				case 0:
				{
					fragmentColor.rgb = channels;
					break;
				}

				case 1:
				{
					fragmentColor.rgb = hslToRgb(360.0f * channels.r, channels.g, channels.b);
					break;
				}

				default:
					break;
			}

			break;
		}
	}
	
	fragmentColor.a = opacity;
	
	if (solidColor)
		fragmentColor.a *= toneMapChannel(displayRanges[0].x, displayRanges[0].y, texture(channelTextures, vec3(uv, 0)).r);
}
)"