#pragma once

#include <QSet>
#include <QOpenGLTexture>

using TextureData		= std::vector<unsigned char>;
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