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