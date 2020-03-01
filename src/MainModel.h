#pragma once

#include <QAbstractListModel>
#include <QSize>
#include <QImage>

class ImageViewerPlugin;
class DatasetsModel;
class LayersModel;

class QItemSelectionModel;

/**
 * Datasets class
 * @author Thomas Kroes
 */
class MainModel : public QAbstractListModel
{
public: // Columns

	/** TODO */
	enum class Columns : int {
		CurrentDatasetID
	};

public:

	/** TODO */
	struct Dataset
	{
		/** TODO */
		struct Layer
		{
			QString					_name;					/** TODO */
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
		};

		QString				_name;				/** TODO */
		int					_type;				/** TODO */
		std::uint32_t		_noImages;			/** TODO */
		QSize				_size;				/** TODO */
		std::uint32_t		_noPoints;			/** TODO */
		std::uint32_t		_noDimensions;		/** TODO */
		std::uint32_t		_currentImage;		/** TODO */
		std::uint32_t		_currentDimension;	/** TODO */
		QStringList			_imageNames;		/** TODO */
		QStringList			_dimensionNames;	/** TODO */
		bool				_averageImages;		/** TODO */
		QStringList			_imageFilePaths;	/** TODO */
		QList<Layer>		_layers;			/** TODO */
	};

public: // Construction/destruction

	/** Constructor */
	MainModel(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~MainModel();

public: // Inherited members

/** TODO */
	int rowCount(const QModelIndex& parent) const { return 1; };

	/** TODO */
	int columnCount(const QModelIndex& parent) const { return 1; };

	/** TODO */
	QVariant data(const QModelIndex& index, int role) const;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex &index) const;

	/** TODO */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

public: //

	/** TODO */
	void addDataset(const Dataset& dataset);

	/** TODO */
	QList<Dataset>* datasets() { return &_datasets; };

	/** TODO */
	DatasetsModel* datasetsModel() { return _datasetsModel; }

	/** TODO */
	LayersModel* layersModel() { return _layersModel; }

	/** TODO */
	QItemSelectionModel* selectionModel() { return _selectionModel; }

private:
	QList<Dataset>			_datasets;				/** TODO */
	std::uint32_t			_currentDatasetID;		/** TODO */
	DatasetsModel*			_datasetsModel;			/** TODO */
	LayersModel*			_layersModel;			/** TODO */
	QItemSelectionModel*	_selectionModel;		/** TODO */
};

using Datasets = QList<MainModel::Dataset>;
using Layers = QList<MainModel::Dataset::Layer>;