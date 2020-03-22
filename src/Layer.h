#pragma once

#include "GeneralSettings.h"
#include "PointsSettings.h"
#include "ImagesSettings.h"
#include "ClustersSettings.h"

#include <QColor>
#include <QObject>
#include <QImage>

class Dataset;

/** TODO */
class Layer : public QObject
{
public:

	/** TODO */
	Layer(Dataset* dataset, const QString& id, const QString& name, const LayerType& type, const std::uint32_t& flags);

public: // MVC

	/** TODO */
	static int columnCount();

	/** TODO */
	static QVariant headerData(int section, Qt::Orientation orientation, int role);

	/** TODO */
	virtual Qt::ItemFlags itemFlags(const LayerColumn& column) const;

	/** TODO */
	virtual QVariant data(const LayerColumn& column, int role) const;

	/** TODO */
	virtual void setData(const LayerColumn& column, const QVariant& value, const int& role);

	GeneralSettings& general() { return _general; }
	PointsSettings& points() { return _points; }
	ImagesSettings& images() { return _images; }
	ClustersSettings& clusters() { return _clusters; }

protected:
	GeneralSettings			_general;		/** TODO */
	PointsSettings			_points;		/** TODO */
	ImagesSettings			_images;		/** TODO */
	ClustersSettings		_clusters;		/** TODO */
	
	friend class ImagesDataset;
};

using Layers = QList<Layer*>;

static int layerColumnId(const LayerColumn& column) { return static_cast<int>(column); };