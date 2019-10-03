R"(
uniform sampler2D texture;
varying mediump vec4 texc;
uniform float minPixelValue;
uniform float maxPixelValue;

void main(void)
{
	float value		= texture2D(texture, texc).r * 65535.0;
	float fraction	= value - minPixelValue;
	float range		= maxPixelValue - minPixelValue;
	float clamped	= clamp(fraction / range, 0.0, 1.0);
	gl_FragColor	= vec4(clamped, clamped, clamped, 1.0);

	//float value = texture2D(texture, texc.st).r;
    //gl_FragColor = vec4(vec3(value), 1);
}
)"