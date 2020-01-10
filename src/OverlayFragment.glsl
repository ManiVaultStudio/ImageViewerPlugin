R"(
#version 330

uniform sampler2D overlayTexture;
uniform vec4 color;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	fragmentColor = vec4(1.f, 1.f, 1.f, 0.5f);//texture(overlayTexture, uv).r > 0 ? color : vec4(0);
}
)"