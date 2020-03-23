#pragma once

#include <QColor>
#include <QObject>
#include <QImage>

class Dataset;
class GeneralSettings;
class PointsSettings;
class ImagesSettings;
class ClustersSettings;

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

		// Clusters
		ClustersStart,
		ClustersEnd,

		Start = GeneralStart,
		End = ClustersEnd
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

			case Column::NoImages:
				return "No. images";

			case Column::Width:
				return "Width";

			case Column::Height:
				return "Height";

			case Column::Size:
				return "Size";

			case Column::NoPoints:
				return "NoPoints";

			case Column::NoDimensions:
				return "NoDimensions";

			case Column::ImageNames:
				return "ImageNames";

			case Column::FilteredImageNames:
				return "FilteredImageNames";

			case Column::ImageIDs:
				return "ImageIDs";

			case Column::ImageFilePaths:
				return "ImageFilePaths";

			case Column::CurrentImageId:
				return "CurrentImageId";

			case Column::CurrentImageName:
				return "CurrentImageName";

			case Column::CurrentImageFilePath:
				return "CurrentImageFilePath";

			case Column::Average:
				return "Average";

			case Column::Selection:
				return "Selection";

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

	static int columnId(const Layer::Column& column) { return static_cast<int>(column); };

public:

	/** TODO */
	Layer(Dataset* dataset, const QString& id, const QString& name, const Type& type, const std::uint32_t& flags);

public: // MVC

	/** TODO */
	static int columnCount();

	/** TODO */
	static QVariant headerData(int section, Qt::Orientation orientation, int role);

	/** TODO */
	virtual Qt::ItemFlags itemFlags(const Column& column) const;

	/** TODO */
	virtual QVariant data(const Column& column, int role) const;

	/** TODO */
	virtual void setData(const Column& column, const QVariant& value, const int& role);

	GeneralSettings* general() { return _general; }
	PointsSettings* points() { return _points; }
	ImagesSettings* images() { return _images; }
	ClustersSettings* clusters() { return _clusters; }

protected:
	GeneralSettings*		_general;		/** TODO */
	PointsSettings*			_points;		/** TODO */
	ImagesSettings*			_images;		/** TODO */
	ClustersSettings*		_clusters;		/** TODO */
	
	friend class ImagesDataset;
};

using Layers = QList<Layer*>;