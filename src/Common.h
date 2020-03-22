#pragma once

#include <vector>
#include <map>

#include <QSet>
#include <QSharedPointer>

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

/**
 * Actor events
 * Defines actor event types
 */
enum class ActorEvent
{
	None			= 1ul << 0,													/** Don't capture mouse events */
	MousePress		= 1ul << 1,													/** Capture mouse press events */
	MouseRelease	= 1ul << 2,													/** Capture mouse release events */
	MouseMove		= 1ul << 3,													/** Capture mouse move events */
	MouseWheel		= 1ul << 4,													/** Capture mouse wheel events */
	MouseAll		= MousePress | MouseRelease | MouseMove | MouseWheel,		/** Capture all mouse events */

	KeyPress		= 1ul << 5,													/** Capture key press events */
	KeyRelease		= 1ul << 6,													/** Capture key release events */
};

/**
 * Returns the string-based name of the actor event
 * @param actorEvent Actor event
 */
inline QString actorEventName(const ActorEvent& actorEvent)
{
	switch (actorEvent)
	{
		case ActorEvent::None:
			return "None";

		case ActorEvent::MousePress:
			return "MousePress";

		case ActorEvent::MouseRelease:
			return "MouseRelease";

		case ActorEvent::MouseMove:
			return "MouseMove";

		case ActorEvent::MouseWheel:
			return "MouseWheel";

		case ActorEvent::MouseAll:
			return "MouseAll";

		default:
			break;
	}

	return "";
}

enum Roles {
	FontIconText = Qt::UserRole
};

/** TODO */
enum class LayerType {
	Points,			/** TODO */
	Images,			/** TODO */
	Clusters,		/** TODO */
	Selection		/** TODO */
};

/** TODO */
static QString layerTypeName(const LayerType& type) {
	switch (type)
	{
		case LayerType::Images:
			return "Image";

		case LayerType::Selection:
			return "Selection";

		case LayerType::Clusters:
			return "Clusters";

		case LayerType::Points:
			return "Points";

		default:
			break;
	}

	return "";
}

/** TODO */
enum LayerColumn {
	Enabled,
	Type,
	Locked,
	ID,						// Name for internal use
	Name,					// Name in the user interface
	Dataset,				// Name in the user interface
	Flags,
	Frozen,
	Removable,
	Mask,
	Renamable,
	Order,
	Opacity,
	WindowNormalized,
	LevelNormalized,
	ColorMap,
	Image,
	ImageRange,
	DisplayRange,

	// Selection
	// Points

	// Images
	NoImages,
	Width,
	Height,
	Size,
	NoPoints,
	NoDimensions,
	ImageNames,
	FilteredImageNames,
	ImageIDs,
	ImageFilePaths,
	CurrentImageId,
	CurrentImageName,
	CurrentImageFilePath,
	Average,
	Selection,

	// Clusters
	ClustersStart,
	ClustersEnd,

	// General settings column range
	GeneralStart = Enabled,
	GeneralEnd = DisplayRange,

	// Selection layer settings column range
	SelectionStart,
	SelectionEnd,

	// Points layer settings column range
	PointsStart,
	PointsEnd,

	// Images layer settings column range
	ImagesStart,
	ImagesEnd,

	End
};

static QString layerColumnName(const LayerColumn& column) {
	switch (column) {
		case LayerColumn::Enabled:
		case LayerColumn::Type:
		case LayerColumn::Locked:
			return "";

		case LayerColumn::ID:
			return "ID";

		case LayerColumn::Name:
			return "Name";

		case LayerColumn::Dataset:
			return "Dataset";

		case LayerColumn::Flags:
			return "Flags";

		case LayerColumn::Frozen:
			return "Frozen";

		case LayerColumn::Removable:
			return "Removable";

		case LayerColumn::Mask:
			return "Mask";

		case LayerColumn::Renamable:
			return "Renamable";

		case LayerColumn::Order:
			return "Order";

		case LayerColumn::Opacity:
			return "Opacity";

		case LayerColumn::WindowNormalized:
			return "Window";

		case LayerColumn::LevelNormalized:
			return "Level";

		case LayerColumn::ColorMap:
			return "Color";

		case LayerColumn::Image:
			return "Image";

		case LayerColumn::ImageRange:
			return "Image range";

		case LayerColumn::DisplayRange:
			return "Display range";

		case LayerColumn::NoImages:
			return "No. images";

		case LayerColumn::Width:
			return "Width";

		case LayerColumn::Height:
			return "Height";

		case LayerColumn::Size:
			return "Size";

		case LayerColumn::NoPoints:
			return "NoPoints";

		case LayerColumn::NoDimensions:
			return "NoDimensions";

		case LayerColumn::ImageNames:
			return "ImageNames";

		case LayerColumn::FilteredImageNames:
			return "FilteredImageNames";

		case LayerColumn::ImageIDs:
			return "ImageIDs";

		case LayerColumn::ImageFilePaths:
			return "ImageFilePaths";

		case LayerColumn::CurrentImageId:
			return "CurrentImageId";

		case LayerColumn::CurrentImageName:
			return "CurrentImageName";

		case LayerColumn::CurrentImageFilePath:
			return "CurrentImageFilePath";

		case LayerColumn::Average:
			return "Average";

		case LayerColumn::Selection:
			return "Selection";

		default:
			return QString();
	}

	return QString();
}

/** TODO */
enum class LayerFlag {
	Enabled		= 0x01,		/** TODO */
	Frozen		= 0x02,		/** TODO */
	Removable	= 0x04,		/** TODO */
	Mask		= 0x08,		/** TODO */
	Renamable	= 0x20,		/** TODO */
	Renderable	= 0x40		/** TODO */
};