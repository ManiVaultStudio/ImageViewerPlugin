#pragma once

#include <QImage>

/** TODO */
class Layer
{
public:

	/** TODO */
	enum class Type {
		Image,			/** TODO */
		Selection,		/** TODO */
		Metadata		/** TODO */
	};

	/** TODO */
	Layer();

	/** TODO */
	Layer(const QString& name, const Type& type, const bool& enabled, const std::uint32_t& order, const float& opacity = 1.0f);

public:
	QString					_name;					/** TODO */
	Type					_type;					/** TODO */
	bool					_enabled;				/** TODO */
	std::uint32_t			_order;					/** TODO */
	float					_opacity;				/** TODO */
	QImage					_image;					/** TODO */
	QPair<float, float>		_imageRange;			/** TODO */
	QPair<float, float>		_displayRange;			/** TODO */
	float					_windowNormalized;		/** TODO */
	float					_levelNormalized;		/** TODO */
	float					_window;				/** TODO */
	float					_level;					/** TODO */

	friend class ImageDataset;
};

using Layers = QList<Layer>;