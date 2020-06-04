#pragma once

#include "Common.h"

#include <QImage>

/**
 * Color map class
 *
 * Primarily a container for color map related parameters (e.g. name, resource path)
 * This model can contain 1D and 2D color maps (perhaps in the future also 3D)
 *
 * @author Thomas Kroes
 */
class ColorMap {
public: // Enumerations

	/**
	* Color map type
	* Defines the color map types
	*/
	enum class Type {
		ZeroDimensional,	/** Zero-dimensional color map (solid color) */
		OneDimensional,		/** One-dimensional color map */
		TwoDimensional		/** Two-dimensional color map */
	};

public:
	/** (Default) constructor */
	ColorMap(const QString& name = "", const QString& resourcePath = "", const Type& type = Type::OneDimensional, const QImage& image = QImage());

	/** Returns the color map name */
	[[nodiscard]] QString name() const;

	/** Returns the resource path */
	[[nodiscard]] QString resourcePath() const;

	/** Returns the color map type */
	[[nodiscard]] Type type() const;

	/** Returns the color map image */
	[[nodiscard]] QImage image() const;

	/** Returns the number of dimensions */
	[[nodiscard]] int noDimensions() const;

private:
	QString		_name;				/** Name in the user interface */
	QString		_resourcePath;		/** Resource path of the color image */
	Type		_type;				/** Color map type */
	QImage		_image;				/** Color map image */
};