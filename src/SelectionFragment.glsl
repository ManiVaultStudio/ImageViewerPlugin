R"(
//#version 330

uniform sampler2D texture;
varying mediump vec4 texc;

void main(void)
{
	float value		= texture2D(texture, texc).r;

	gl_FragColor	=  value > 0 ? vec4(1, 0, 0, 1) : vec4(0);
}
)"