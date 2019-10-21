R"(
#version 330

uniform sampler2D pixelSelectionTexture;
uniform int selectionType;
uniform vec4 selectionColor;
uniform vec2 brushCenter;
uniform float brushRadius;
uniform vec2 rectangleTopLeft;
uniform vec2 rectangleBottomRight;

in vec2 uv;
out vec4 fragmentColor;

void main(void)
{
	switch (selectionType) {
		case 0:
		{
			bool inRectangle	= uv.x >= rectangleTopLeft.x && uv.x < rectangleBottomRight.x && uv.y >= rectangleTopLeft.y && uv.y < rectangleBottomRight.y;
			fragmentColor		= inRectangle ? vec4(1) : vec4(vec3(0), 1);
			break;
		}

		case 1:
		{
			bool inBrush		= length(uv - brushCenter) < brushRadius;
			bool prevInBrush	= texture(pixelSelectionTexture, vec2(uv.x, 1-uv.y)).r > 0;
			fragmentColor		= (inBrush || prevInBrush) ? vec4(1) : vec4(vec3(0), 1);
			break;
		}
	}
}
)"