R"(
#version 330

#extension GL_ARB_gpu_shader_fp64 : enable

uniform sampler2D imageTexture;
uniform float minPixelValue;
uniform float maxPixelValue;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(imageTexture, uv);

	double range = maxPixelValue - minPixelValue;

	for (int c = 0; c < 3; c++)
	{
		double fraction		= (double(fragmentColor[c]) * 65535.0) - double(minPixelValue);
		fragmentColor[c]	= float(clamp(fraction / range, 0.0, 1.0));
	}

	fragmentColor.a = 1.0f;
}
)"