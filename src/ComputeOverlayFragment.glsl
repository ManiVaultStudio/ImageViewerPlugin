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
uniform vec2 previousPosition;
uniform vec2 currentPosition;

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
			vec2 uvWorld			= uv * imageSize;
			bool inBrush			= length(uvWorld - currentBrushCenter) < brushRadius;
			bool prevInBrush		= texture(pixelSelectionTexture, vec2(uv.x, 1.f - uv.y)).r > 0;
			
			if (currentBrushCenter != previousBrushCenter) {
				vec2 a		= currentBrushCenter - previousBrushCenter;
				vec2 b		= uvWorld - previousBrushCenter;
				float dotAB = dot(b, normalize(a));
				

				if (dotAB > 0 && dotAB < length(a)) {
					vec2 c = previousBrushCenter + dotAB * normalize(a);
					if (length(uvWorld - c) < brushRadius)
						inBrush = true;
				}
			}

			//float distanceToLine	= 	;			
			fragmentColor			= (inBrush || prevInBrush) ? vec4(1) : vec4(vec3(0), 1);
			break;
		}
	}
}
)"