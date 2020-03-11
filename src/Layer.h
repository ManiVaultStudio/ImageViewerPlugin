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
		Cluster			/** TODO */
	};

	static QString typeName(const Type& type) {
		switch (type)
		{
			case Layer::Type::Image:
				return "Image";

			case Layer::Type::Selection:
				return "Selection";

			case Layer::Type::Cluster:
				return "Cluster";

			default:
				break;
		}

		return "";
	}

	/** TODO */
	enum Flags : int {
		Enabled		= 1ul << 0,		/** TODO */
		Frozen		= 1ul << 1,		/** TODO */
		Removable	= 1ul << 2,		/** TODO */
		Mask		= 1ul << 3,		/** TODO */
		Renamable	= 1ul << 4		/** TODO */
	};

	/** TODO */
	Layer(QObject* parent);

	/** TODO */
	Layer(QObject* parent, const QString& id, const QString& name, const Type& type, const std::uint32_t& flags, const std::uint32_t& order, const float& opacity = 1.0f, const float& window = 1.0f, const float& level = 0.5f);

public: // Getters/setters

	/** TODO */
	QVariant id(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setId(const QString& id);

	/** TODO */
	QVariant name(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setName(const QString& name);

	/** TODO */
	QVariant type(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setType(const Type& type);

	/** TODO */
	QVariant flag(const std::uint32_t& flag, const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setFlag(const std::uint32_t& flag, const bool& enabled = true);

	/** TODO */
	QVariant order(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setOrder(const std::uint32_t& order);

	/** TODO */
	QVariant opacity(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setOpacity(const float& opacity);

	/** TODO */
	const LayerImage& image() const { return _image; }

	/** TODO */
	LayerImage& image() { return _image; }

	/** TODO */
	QColor color(const int& role = Qt::DisplayRole) const { return _color; }

	/** TODO */
	void setColor(const QColor& color) { _color = color; }

	/** TODO */
	std::uint32_t& order(const int& role = Qt::DisplayRole) { return _order; }

private:
	QString			_id;			/** TODO */
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