R"(
#version 330

#extension GL_ARB_gpu_shader_fp64 : enable

uniform sampler2D imageTexture;
uniform float minPixelValue;
uniform float maxPixelValue;
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
	fragmentColor = texture(imageTexture, uv);

	for (int c = 0; c < 3; c++) {
		fragmentColor[c] = toneMapChannel(minPixelValue, maxPixelValue, fragmentColor[c]);
	}

	fragmentColor.a = opacity;
}
)"