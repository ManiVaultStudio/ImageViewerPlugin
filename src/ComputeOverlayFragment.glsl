R"(
#version 330

uniform sampler2D pixelSelectionTexture;
uniform int selectionType;
uniform vec2 previousBrushCenter;
uniform vec2 currentBrushCenter;
uniform float brushRadius;
uniform vec2 rectangleTopLeft;
uniform vec2 rectangleBottomRight;
uniform vec2 imageSize;

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
			vec2 P				= uv * imageSize;
			bool inBrush		= length(P - currentBrushCenter) < brushRadius;
			bool prevInBrush	= texture(pixelSelectionTexture, vec2(uv.x, 1.f - uv.y)).r > 0;
			
			if (currentBrushCenter != previousBrushCenter) {
				vec2 A		= currentBrushCenter - previousBrushCenter;
				vec2 AN		= normalize(A);
				vec2 B		= P - previousBrushCenter;
				float dotBA	= dot(B, normalize(A));

				if (dotBA > 0 && dotBA < length(A)) {
					vec2 C = previousBrushCenter + dotBA * AN;
					
					if (length(P - C) < brushRadius)
						inBrush = true;
				}
			}

			fragmentColor = (inBrush || prevInBrush) ? vec4(1) : vec4(vec3(0), 1);
			break;
		}
	}
}
)"