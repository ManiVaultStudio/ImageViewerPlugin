#pragma once

#include <vector>
#include <map>

#include <QSet>

class QOpenGLTexture;
class QOpenGLShaderProgram;

/**
 * Interaction mode
 * Defines interaction modes that are possible in the image viewer
 */
enum class InteractionMode
{
	None,			/** No interaction takes place */
	Navigation,		/** The image view position and zoom are manipulated */
	Selection,		/** Image pixels are selected */
	WindowLevel		/** The image display window and/or level are manipulated */
};

/**
 * Returns the string-based name of the interaction mode
 * @param interactionMode Interaction mode
 */
inline QString interactionModeTypeName(const InteractionMode& interactionMode)
{
	switch (interactionMode)
	{
		case InteractionMode::None:
			return "None";

		case InteractionMode::Navigation:
			return "Navigation";

		case InteractionMode::Selection:
			return "Selection";

		case InteractionMode::WindowLevel:
			return "WindowLevel";

		default:
			break;
	}

	return "";
}

/**
 * Selection type
 * Defines selection types for selection of image pixels
 */
enum class SelectionType
{
	None,			/** Not set */
	Rectangle,		/** A rectangle is drawn to select pixels */
	Brush,			/** A brush is used the paint selection */
	Lasso,			/** A lasso tool is used to enclose pixels */
	Polygon			/** A polygon is drawn to select pixels */
};

/**
 * Returns the string-based name of the selection type
 * @param selectionType Selection type
 */
inline QString selectionTypeName(const SelectionType& selectionType)
{
	switch (selectionType)
	{
	case SelectionType::Rectangle:
		return "Rectangle";

	case SelectionType::Brush:
		return "Brush";

	case SelectionType::Lasso:
		return "Lasso";

	case SelectionType::Polygon:
		return "Polygon";

	default:
		break;
	}

	return "";
}