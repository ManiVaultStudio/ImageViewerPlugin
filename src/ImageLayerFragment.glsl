R"(
#version 330

#extension GL_ARB_gpu_shader_fp64 : enable

uniform sampler2D imageTexture;
uniform int type;
uniform float minPixelValue;
uniform float maxPixelValue;
uniform float opacity;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = texture(imageTexture, uv);

	switch (type) {
		case 0:
		{
			double range = maxPixelValue - minPixelValue;

			for (int c = 0; c < 3; c++)
			{
				double fraction		= (double(fragmentColor[c]) * 65535.0) - double(minPixelValue);
				fragmentColor[c]	= float(clamp(fraction / range, 0.0, 1.0));
			}

			fragmentColor.a = opacity;
			break;
		}
		
		case 1:
		{
			fragmentColor.a = fragmentColor[0] > 0 ? opacity : 0;
			fragmentColor.r = 1.0f;
			fragmentColor.g = 0.0f;
			fragmentColor.b = 0.0f;
			break;
		}

		case 2:
		{
			fragmentColor.a = opacity;
			break;
		}

		default:
			break;
	}
}
)"