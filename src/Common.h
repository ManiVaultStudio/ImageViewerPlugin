#pragma once

#include <vector>
#include <map>

#include <QSet>

class QOpenGLTexture;
class QOpenGLShaderProgram;

enum class InteractionMode
{
	None,
	Navigation,
	Selection,
	WindowLevel
};

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

enum class SelectionType
{
	Rectangle,
	Brush,
	Lasso,
	Polygon
};

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