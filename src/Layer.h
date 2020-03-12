#pragma once

#include "Common.h"

#include <QColor>
#include <QObject>
#include <QImage>

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
	class Range
	{
	public:

		/** TODO */
		Range(const float& min = 0.0f, const float& max = 0.0f) :
			_min(std::min(min, max)),
			_max(std::max(min, max))
		{
		}

		/** TODO */
		float min() const { return _min; }

		/** TODO */
		void setMin(const float& min) { _min = std::min(min, _max); }

		/** TODO */
		float max() const { return _max; }

		/** TODO */
		void setMax(const float& max) { _max = std::max(_min, max); }

		/** TODO */
		void include(const float& value) {
			_min = std::min(_min, value);
			_max = std::max(_max, value);
		}

		/** TODO */
		void setFullRange() {
			_max = std::numeric_limits<float>::min();
			_min = std::numeric_limits<float>::max();
		}

		/** TODO */
		float length() const { return _max - _min; }

	private:
		float	_min;	/** TODO */
		float	_max;	/** TODO */
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
	QVariant color(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setColor(const QColor& color);

	/** TODO */
	std::uint32_t& order(const int& role = Qt::DisplayRole);

public: // Image functions

	/** TODO */
	QVariant image(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setImage(const QImage& image);

	/** TODO */
	QVariant imageRange(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant displayRange(const int& role = Qt::DisplayRole) const;

	/** TODO */
	QVariant windowNormalized(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setWindowNormalized(const float& windowNormalized);

	/** TODO */
	QVariant levelNormalized(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setLevelNormalized(const float& levelNormalized);

	/** TODO */
	QVariant window(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setWindow(const float& window);

	/** TODO */
	QVariant level(const int& role = Qt::DisplayRole) const;

	/** TODO */
	void setLevel(const float& level);

protected:

	/** TODO */
	void computeImageRange();

	/** TODO */
	void computeDisplayRange();

private:
	QString			_id;					/** TODO */
	QString			_name;					/** TODO */
	Type			_type;					/** TODO */
	std::uint32_t	_flags;					/** TODO */
	std::uint32_t	_order;					/** TODO */
	float			_opacity;				/** TODO */
	QColor			_color;					/** TODO */
	QImage			_image;					/** TODO */
	Range			_imageRange;			/** TODO */
	Range			_displayRange;			/** TODO */
	float			_windowNormalized;		/** TODO */
	float			_levelNormalized;		/** TODO */
	float			_window;				/** TODO */
	float			_level;					/** TODO */

	friend class ImageDataset;
};

Q_DECLARE_METATYPE(Layer::Range);

using Layers = QList<Layer*>;