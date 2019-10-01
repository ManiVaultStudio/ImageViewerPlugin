#pragma once

#include <QSet>
#include <QOpenGLTexture>

using TextureData		= std::vector<std::uint16_t>;
using TextureDataMap	= std::map<QString, TextureData>;
using Index				= unsigned int;
using Indices			= std::vector<Index>;
using TextureMap		= std::map<QString, QOpenGLTexture>;
using PixelCoordinate	= std::pair<std::int32_t, std::int32_t>;
using PixelCoordinates	= std::vector<PixelCoordinate>;

enum class ImageCollectionType
{
	Undefined,
	Sequence,
	Stack,
	MultiPartSequence
};

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
	Freehand
};

inline QString selectionTypeTypeName(const SelectionType& selectionType)
{
	switch (selectionType)
	{
	case SelectionType::Rectangle:
		return "Rectangle";

	case SelectionType::Brush:
		return "Brush";

	case SelectionType::Freehand:
		return "Freehand";

	default:
		break;
	}

	return "";
}

enum class SelectionModifier
{
	Replace,
	Add,
	Remove
};

inline QString selectionModifierName(const SelectionModifier& selectionModifier)
{
	switch (selectionModifier)
	{
	case SelectionModifier::Replace:
		return "Replace";

	case SelectionModifier::Add:
		return "Add";

	case SelectionModifier::Remove:
		return "Remove";

	default:
		break;
	}

	return "";
}