#pragma once

#include "Common.h"

#include <QColor>
#include <QObject>
#include <QImage>

class Dataset;

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

Q_DECLARE_METATYPE(Range);

/** TODO */
class Layer : public QObject
{

public:

	/** TODO */
	enum class Column {
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

		Count
	};

	static QString columnName(const Column& column) {
		switch (column) {
			case Column::Enabled:
			case Column::Type:
			case Column::Locked:
				return "";

			case Column::ID:
				return "ID";

			case Column::Name:
				return "Name";

			case Column::Dataset:
				return "Dataset";

			case Column::Flags:
				return "Flags";

			case Column::Frozen:
				return "Frozen";

			case Column::Removable:
				return "Removable";

			case Column::Mask:
				return "Mask";

			case Column::Renamable:
				return "Renamable";

			case Column::Order:
				return "Order";

			case Column::Opacity:
				return "Opacity";

			case Column::WindowNormalized:
				return "Window";

			case Column::LevelNormalized:
				return "Level";

			case Column::ColorMap:
				return "Color";

			case Column::Image:
				return "Image";

			case Column::ImageRange:
				return "Image range";

			case Column::DisplayRange:
				return "Display range";

			default:
				return QString();
		}

		return QString();
	}

	/** TODO */
	enum class Type {
		Points,			/** TODO */
		Images,			/** TODO */
		Clusters,		/** TODO */
		Selection		/** TODO */
	};

	/** TODO */
	static QString typeName(const Type& type) {
		switch (type)
		{
			case Type::Images:
				return "Image";

			case Type::Selection:
				return "Selection";

			case Type::Clusters:
				return "Clusters";

			case Type::Points:
				return "Points";

			default:
				break;
		}

		return "";
	}

	/** TODO */
	enum class Flag {
		Enabled		= 0x01,		/** TODO */
		Frozen		= 0x02,		/** TODO */
		Removable	= 0x04,		/** TODO */
		Mask		= 0x08,		/** TODO */
		Renamable	= 0x20,		/** TODO */
		Renderable	= 0x40		/** TODO */
	};

	/** TODO */
	Layer(Dataset* dataset, const Type& type, const QString& id, const QString& name, const std::uint32_t& flags);

public: // MVC

	/** TODO */
	static int columnCount();

	/** TODO */
	static QVariant headerData(int section, Qt::Orientation orientation, int role);

	/** TODO */
	virtual Qt::ItemFlags itemFlags(const int& column) const;

	/** TODO */
	virtual QVariant data(const int& column, int role) const;

	/** TODO */
	virtual void setData(const int& column, const QVariant& value, const int& role);

public: // Getters/setters

	/** TODO */
	QVariant id(const int& role) const;

	/** TODO */
	void setId(const QString& id);

	/** TODO */
	QVariant name(const int& role) const;

	/** TODO */
	void setName(const QString& name);

	/** TODO */
	QVariant dataset(const int& role) const;

	/** TODO */
	QVariant type(const int& role) const;

	/** TODO */
	void setType(const Layer::Type& type);

	/** TODO */
	QVariant flags(const int& role) const;

	/** TODO */
	QVariant flag(const Layer::Flag& flag, const int& role) const;

	/** TODO */
	void setFlag(const Layer::Flag& flag, const bool& enabled = true);

	/** TODO */
	void setFlags(const std::uint32_t& flags);

	/** TODO */
	QVariant order(const int& role) const;

	/** TODO */
	void setOrder(const std::uint32_t& order);

	/** TODO */
	QVariant opacity(const int& role) const;

	/** TODO */
	void setOpacity(const float& opacity);

	/** TODO */
	QVariant colorMap(const int& role) const;

	/** TODO */
	void setColorMap(const QImage& colorMap);

public: // Image functions

	/** TODO */
	QVariant image(const int& role) const;

	/** TODO */
	void setImage(const QImage& image);

	/** TODO */
	QVariant imageRange(const int& role) const;

	/** TODO */
	QVariant displayRange(const int& role) const;

	/** TODO */
	QVariant windowNormalized(const int& role) const;

	/** TODO */
	void setWindowNormalized(const float& windowNormalized);

	/** TODO */
	QVariant levelNormalized(const int& role) const;

	/** TODO */
	void setLevelNormalized(const float& levelNormalized);

	/** TODO */
	QVariant window(const int& role) const;

	/** TODO */
	void setWindow(const float& window);

	/** TODO */
	QVariant level(const int& role) const;

	/** TODO */
	void setLevel(const float& level);

protected:

	/** TODO */
	void computeImageRange();

	/** TODO */
	void computeDisplayRange();

protected:
	Dataset*			_dataset;				/** TODO */
	QString				_id;					/** TODO */
	QString				_name;					/** TODO */
	Layer::Type			_type;					/** TODO */
	std::uint32_t		_flags;					/** TODO */
	std::uint32_t		_order;					/** TODO */
	float				_opacity;				/** TODO */
	QImage				_colorMap;				/** TODO */
	QImage				_image;					/** TODO */
	Range				_imageRange;			/** TODO */
	Range				_displayRange;			/** TODO */
	float				_windowNormalized;		/** TODO */
	float				_levelNormalized;		/** TODO */
	float				_window;				/** TODO */
	float				_level;					/** TODO */
};

using Layers = QList<Layer*>;