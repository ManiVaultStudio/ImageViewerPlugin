R"(
#version 330

#extension GL_ARB_gpu_shader_fp64 : enable

uniform sampler2D colorMap;
uniform sampler2D channels[3];
uniform int noChannels;
uniform vec2 channelRange[3];
uniform float opacity;

in vec2 uv;
out vec4 fragmentColor;

float toneMapChannel(double minPixelValue, double maxPixelValue, double pixelValue)
{
	double range	= maxPixelValue - minPixelValue;
	double fraction	= (pixelValue * 65535.0) - minPixelValue;

	return float(clamp(fraction / range, 0.0, 1.0));
}

void main(void)
{
	float channel1 = toneMapChannel(channelRange[0].x, channelRange[0].y, texture(channels[0], uv).r);

	fragmentColor = texture(colorMap, vec2(channel1, 0));
	fragmentColor.a = opacity;
}
)"