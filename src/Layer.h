#pragma once

#include "LayerImage.h"

/** TODO */
class Layer : public QObject
{
	Q_OBJECT

public:

	/** TODO */
	enum Type : int {
		Image,			/** TODO */
		Selection,		/** TODO */
		Metadata		/** TODO */
	};

	/** TODO */
	enum Flags : int {
		Enabled		= 1ul << 0,		/** TODO */
		Fixed		= 1ul << 1,		/** TODO */
		Removable	= 1ul << 2		/** TODO */
	};

	/** TODO */
	Layer(QObject* parent);

	/** TODO */
	Layer(QObject* parent,const QString& name, const Type& type, const std::uint32_t& flags, const std::uint32_t& order, const float& opacity = 1.0f, const float& window = 1.0f, const float& level = 0.5f);

public: // Getters/setters

	/** TODO */
	QString name() const { return _name; }

	/** TODO */
	void setName(const QString& name) { _name = name; }

	/** TODO */
	Type type() const { return _type; }

	/** TODO */
	void setType(const Type& type) { _type = type; }

	/** TODO */
	bool isFlagSet(const std::uint32_t& flag) const;

	/** TODO */
	void setFlag(const std::uint32_t& flag, const bool& enabled = true);

	/** TODO */
	std::uint32_t order() const { return _order; }

	/** TODO */
	std::uint32_t& order() { return _order; }

	/** TODO */
	void setOrder(const std::uint32_t& order) { _order = order; }

	/** TODO */
	float opacity() const { return _opacity; }

	/** TODO */
	void setOpacity(const float& opacity) { _opacity = opacity; }

	/** TODO */
	const LayerImage& image() const { return _image; }

	/** TODO */
	LayerImage& image() { return _image; }

	/** TODO */
	QColor color() const { return _color; }

	/** TODO */
	void setColor(const QColor& color) { _color = color; }

private:
	QString			_name;			/** TODO */
	Type			_type;			/** TODO */
	std::uint32_t	_flags;			/** TODO */
	std::uint32_t	_order;			/** TODO */
	float			_opacity;		/** TODO */
	LayerImage		_image;			/** TODO */
	QColor			_color;			/** TODO */

	friend class ImageDataset;
};

using Layers = QList<Layer*>;