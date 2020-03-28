#pragma once

#include "Layer.h"

#include <QItemSelectionModel>
#include <QAbstractListModel>

class ImageViewerPlugin;
class Dataset;
class GroupLayer;

/** TODO */
class LayersModel : public QAbstractItemModel
{
public: // Construction/destruction

	/** Constructor */
	LayersModel(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~LayersModel();

	/** TODO */
	QItemSelectionModel* selectionModel() { return &_selectionModel; }
	
public: // Inherited members

	/** TODO */
	int rowCount(const QModelIndex& parentIndex = QModelIndex()) const override;

	/** TODO */
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	/** TODO */
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	/** TODO */
	QModelIndex parent(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, int role) const override;

	/** TODO */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	/** TODO */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::DisplayRole) override;

	/** TODO */
	bool insertRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;

public: // Reorganization

	/** TODO */
	int order(const QModelIndex& layerIndex) const;

	/** TODO */
	int noSiblings(const QModelIndex& layerIndex) const;

	/** TODO */
	bool mayMoveUp(const QModelIndex& layerIndex) const;

	/** TODO */
	bool mayMoveDown(const QModelIndex& layerIndex) const;

	/** TODO */
	void moveUp(const QModelIndex& layerIndex);

	/** TODO */
	void moveDown(const QModelIndex& layerIndex);

public: // TODO

	/** TODO */
	void renameLayer(const QString& id, const QString& name);

	/** TODO */
	Layer* findLayerById(const QString& id);

	/** TODO */
	void addLayer(Layer* layer, const QModelIndex& parentIndex = QModelIndex());

private:
	ImageViewerPlugin*		_imageViewerPlugin;		/** TODO */
	QItemSelectionModel		_selectionModel;		/** TODO */
	GroupLayer*				_rootItem;				/** TODO */

	friend class MainModel;
};