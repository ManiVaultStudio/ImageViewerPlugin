#pragma once

#include "Common.h"

#include "ImageData/Images.h"

#include <QAbstractTableModel>
#include <QItemSelectionModel>

class ImageViewerPlugin;

/**
 * Datasets class
 * @author Thomas Kroes
 */
class ImageDatasetsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/** TODO */
	enum class Columns : int {
		Name,
		Type,
		NoImages,
		Size,
		NoPoints,
		NoDimensions,
		ImageID,
		DimensionID,
		ImageNames,
		DimensionNames,
		AverageImages
	};

	/** TODO */
	struct ImageDataset
	{
		QString				_name;				/** TODO */
		QString				_type;				/** TODO */
		std::uint32_t		_noImages;			/** TODO */
		QSize				_size;				/** TODO */
		std::uint32_t		_noPoints;			/** TODO */
		std::uint32_t		_noDimensions;		/** TODO */
		std::uint32_t		_imageID;			/** TODO */
		std::uint32_t		_dimensionID;		/** TODO */
		QStringList			_imageNames;		/** TODO */
		QStringList			_dimensionNames;	/** TODO */
		bool				_averageImages;		/** TODO */
	};

public:

	/** Constructor */
	ImageDatasetsModel(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~ImageDatasetsModel();

	/** TODO */
	void add(const ImageDataset& imageDataset);

	/** TODO */
	//QItemSelectionModel& selectionModel() { return _selectionModel; };

public: // Inherited members

	/** TODO */
	int rowCount(const QModelIndex& parent) const;

	/** TODO */
	int columnCount(const QModelIndex& parent) const;

	/** TODO */
	QVariant data(const QModelIndex& index, int role) const;

	/** TODO */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex &index) const;

	/** TODO */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole);

	/** TODO */
	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex());

	/** TODO */
	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex());

	/** TODO */
	QList<ImageDataset> imageDatasets();

/*
signals:
	void namesChanged(const QStringList& names);
	void currentDatasetNameChanged(const QString& previousDatasetName, const QString& currentDatasetName);
	void currentDatasetChanged(ImageDataset* previousImageDataset, ImageDataset* currentImageDataset);
*/

private:
	QItemSelectionModel		_selectionModel;		/** TODO */
	QList<ImageDataset>		_imageDatasets;			/** TODO */
	QString					_currentDatasetName;	/** TODO */
};