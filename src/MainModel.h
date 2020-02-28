#pragma once

#include "DatasetsModel.h"
#include "LayersModel.h"

#include <QSize>
#include <QImage>

class ImageViewerPlugin;

/**
 * Datasets class
 * @author Thomas Kroes
 */
class MainModel : public QObject
{
public:

	/** TODO */
	struct ImageDataset
	{
		/** TODO */
		struct ImageLayer
		{
			/** TODO */
			enum class Columns : int {
				Name,
				Order,
				Opacity,
				Window,
				Level
			};

			QString					_name;					/** TODO */
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
		ImageLayer			_layers;			/** TODO */
	};

public:

	/** Constructor */
	MainModel(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~MainModel();

	/** TODO */
	void add(const ImageDataset& imageDataset);

	/** TODO */
	QList<ImageDataset> imageDatasets() { return _imageDatasets; };

private:
	QList<ImageDataset>		_imageDatasets;			/** TODO */
	QString					_currentDatasetName;	/** TODO */
	DatasetsModel			_datasetsModel;			/** TODO */
	LayersModel				_layersModel;			/** TODO */
};