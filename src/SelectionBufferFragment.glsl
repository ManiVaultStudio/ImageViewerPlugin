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
uniform vec2 points[500];
uniform int noPoints;

in vec2 uv;
out vec4 fragmentColor;

// The routines for checking if a point is within a polygon are described here: https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/

// Given three colinear points p, q, r, the function checks if point q lies on line segment 'pr'
bool onSegment(vec2 p, vec2 q, vec2 r) 
{ 
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y)) 
        return true;
	
    return false; 
}

// To find orientation of ordered triplet (p, q, r), returns 0 > p, q and r are colinear, 1 > Clockwise, 2 > Counterclockwise
int orientation(vec2 p, vec2 q, vec2 r) 
{ 
    float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y); 
  
    if (val == 0) {

		// Colinear
		return 0;
	}
	
	// Clock or counterclock wise 
    return (val > 0) ? 1 : 2;
} 

// The function that returns true if line segment 'p1q1' and 'p2q2' intersect
bool doIntersect(vec2 p1, vec2 q1, vec2 p2, vec2 q2) 
{ 
    // Find the four orientations needed for general and special cases 
    int o1 = orientation(p1, q1, p2); 
    int o2 = orientation(p1, q1, q2); 
    int o3 = orientation(p2, q2, p1); 
    int o4 = orientation(p2, q2, q1); 
  
    // General case 
    if (o1 != o2 && o3 != o4) 
        return true; 
  
    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && onSegment(p1, p2, q1))
		return true; 
  
    // p1, q1 and p2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && onSegment(p1, q2, q1))
		return true;
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && onSegment(p2, p1, q2))
		return true;
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && onSegment(p2, q1, q2))
		return true;
  
	// Doesn't fall in any of the above cases 
    return false;
} 

// Returns true if the point p lies inside the polygon[] with n vertices 
bool isInside(vec2 p) 
{ 
    // There must be at least 3 vertices in points
    if (noPoints < 3)
		return false;
  
    // Create a point for line segment from p to infinite 
    vec2 extreme = vec2(10000.0f, p.y); 
  
    // Count intersections of the above line with sides of polygon 
    int count = 0, i = 0;
	
    do
    { 
        int next = (i + 1) % noPoints; 
  
        // Check if the line segment from 'p' to 'extreme' intersects with the line segment from 'points[i]' to 'points[next]' 
        if (doIntersect(points[i], points[next], p, extreme)) 
        { 
            // If the point 'p' is colinear with line segment 'i-next', then check if it lies on segment. If it lies, return true, otherwise false 
            if (orientation(points[i], p, points[next]) == 0) 
               return onSegment(points[i], p, points[next]); 
  
            count++; 
        } 
        i = next; 
    } while (i != 0); 
	
    // Return true if count is odd, false otherwise 
    return bool(count & 1);  // Same as (count % 2 == 1) 

	return true;
}

void main(void)
{
	vec2 P = uv * imageSize;

	switch (selectionType) {
		case 0:
		{
			bool inRectangle	= uv.x >= rectangleTopLeft.x && uv.x < rectangleBottomRight.x && uv.y >= rectangleTopLeft.y && uv.y < rectangleBottomRight.y;
			fragmentColor		= inRectangle ? vec4(1) : vec4(vec3(0), 1);
			break;
		}

		case 1:
		{
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

		case 2:
		case 3:
		{
			fragmentColor = isInside(P) ? vec4(1) : vec4(vec3(0), 1);
			break;
		}
	}
}
)"