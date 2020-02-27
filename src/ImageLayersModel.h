#pragma once

#include "Common.h"

#include "ImageData/Images.h"

#include <QAbstractTableModel>
#include <QItemSelectionModel>

class ImageViewerPlugin;

class ImageLayersModel : public QAbstractTableModel
{
public:
	/** TODO */
	enum class Columns : int {
		Name,
		Order,
		Opacity,
		Window,
		Level
	};

	/** TODO */
	struct ImageLayer
	{
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

public: // Construction/destruction

	/** Constructor */
	ImageLayersModel();

	/** Destructor */
	~ImageLayersModel();

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
	QList<ImageLayer> imageLayers();

private:
	QList<ImageLayer>	_imageLayers;			/** TODO */
	QString				_currentDatasetName;	/** TODO */
};