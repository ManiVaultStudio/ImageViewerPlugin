#pragma once

#include "TreeItem.h"
#include "ImageRange.h"

#include <QColor>
#include <QObject>
#include <QImage>
#include <QModelIndex>

class Dataset;

/** TODO */
class Layer : public TreeItem
{
public:

	/** TODO */
	enum class Column {
		Name,
		Enabled,
		Type,
		ID,
		Dataset,
		Opacity,
		WindowNormalized,
		LevelNormalized,
		ColorMap,
		Image,
		ImageRange,
		DisplayRange,
		Flags,

		Start = Enabled,
		End = Flags
	};

	static QString columnName(const Column& column) {
		switch (column) {
			case Column::Name:
				return "Name";

			case Column::Enabled:
				return "";

			case Column::Type:
				return "";

			case Column::ID:
				return "ID";

			case Column::Dataset:
				return "Dataset";

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

			case Column::Flags:
				return "Flags";

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

	/** Constructor */
	Layer(Dataset* dataset, const Type& type, const QString& id, const QString& name, const int& flags);

	/** Destructor */
	virtual ~Layer();

public: // MVC

	/** TODO */
	virtual int noColumns() const;

	/** TODO */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/** TODO */
	virtual QVariant data(const QModelIndex& index, const int& role) const;

	/** TODO */
	virtual void setData(const QModelIndex& index, const QVariant& value, const int& role);

	/** TODO */
	bool isBaseLayerIndex(const QModelIndex& index) const;

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
	QVariant flag(const Layer::Flag& flag, const int& role) const;

	/** TODO */
	void setFlag(const Layer::Flag& flag, const bool& enabled = true);

	/** TODO */
	QVariant flags(const int& role) const;

	/** TODO */
	void setFlags(const int& flags);

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

signals:

	/** TODO */
	void dataChanged();

protected:
	Dataset*			_dataset;				/** TODO */
	QString				_id;					/** TODO */
	QString				_name;					/** TODO */
	Layer::Type			_type;					/** TODO */
	std::uint32_t		_flags;					/** TODO */
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