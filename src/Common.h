#pragma once

#include <vector>
#include <map>

#include <QSet>

class QOpenGLTexture;
class QOpenGLShaderProgram;

enum class InteractionMode
{
	Navigation,
	Selection,
	WindowLevel
};

inline QString interactionModeTypeName(const InteractionMode& interactionMode)
{
	switch (interactionMode)
	{
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
	Lasso
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

	default:
		break;
	}

	return "";
}