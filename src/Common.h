#pragma once

#include <vector>
#include <map>
#include <type_traits>

#include <QSet>
#include <QVector>
#include <QSharedPointer>

class QOpenGLTexture;
class QOpenGLShaderProgram;

/**
 * Interaction mode
 * Defines interaction modes that are possible in the image viewer
 */
enum class InteractionMode
{
	None,				/** No interaction takes place */
	Navigation,			/** The image view position and zoom are manipulated */
	LayerEditing		/** Layer editing interaction */
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

		case InteractionMode::LayerEditing:
			return "Layer editing";

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
	None = -1,		/** Not set */
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

template <typename E>
constexpr auto ult(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

using Indices = QVector<std::uint32_t>;

Q_DECLARE_METATYPE(Indices);

/** Turns a lengthy list of strings into an abbreviated string */
inline QString abbreviatedStringList(const QStringList& stringList)
{
	const auto noStrings = stringList.size();

	if (noStrings == 1)
		return QString("%1").arg(stringList.first());

	if (noStrings == 2)
		return QString("[%1, %2]").arg(stringList.first(), stringList.last());

	if (noStrings > 2)
		return QString("[%1, ..., %2]").arg(stringList.first(), stringList.last());

	return QString("[]");
}

/**
 * Color space
 * Defines color spaces
 */
enum class ColorSpace
{
	RGB,	/** Red Green Blue */
	HSL,	/** Hue Saturation Lightness */
	LAB		/**  */
};

/**
 * Returns the string-based name of the color space
 * @param colorSpace Color space
 */
inline QString colorSpaceName(const ColorSpace& colorSpace)
{
	switch (colorSpace)
	{
		case ColorSpace::RGB:
			return "RGB";

		case ColorSpace::HSL:
			return "HSL";

		case ColorSpace::LAB:
			return "LAB";

		default:
			break;
	}

	return "";
}