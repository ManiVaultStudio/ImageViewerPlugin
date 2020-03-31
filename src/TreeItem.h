#pragma once

#include "ImageRange.h"

#include <QColor>
#include <QObject>
#include <QImage>
#include <QModelIndex>

class Dataset;

class Layer
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

		Settings,

		Start = Enabled,
		End = DisplayRange
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
		Selection,		/** TODO */
		Group			/** TODO */
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

public:
	explicit Layer(const QVector<QVariant> &data, Layer *parent = nullptr);
	~Layer();

	Layer *child(int number);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	bool insertChildren(int position, int count, int columns);
	bool insertColumns(int position, int columns);
	Layer *parent();
	bool removeChildren(int position, int count);
	bool removeColumns(int position, int columns);
	int childNumber() const;
	bool setData(int column, const QVariant &value);

private:
	QVector<Layer*> childItems;
	QVector<QVariant> itemData;
	Layer *parentItem;

	QVector<Layer*>		_children;				/** TODO */
	Layer*				_parent;				/** TODO */
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