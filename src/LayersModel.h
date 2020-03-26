#pragma once

#include "LayerItem.h"

#include <QItemSelectionModel>
#include <QAbstractListModel>

class ImageViewerPlugin;

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
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	/** TODO */
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	/** TODO */
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

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

	/** TODO */
	bool removeRows(int position, int rows, const QModelIndex& index = QModelIndex()) override;

	/** TODO */
	bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override;

public: // Overloaded data access

/** TODO */
	QVariant data(const int& row, const int& column, int role, const QModelIndex& parent = QModelIndex()) const;

	/** TODO */
	void setData(const int& row, const int& column, const QVariant& value, const QModelIndex& parent = QModelIndex());

public: // TODO

	/** TODO */
	bool mayMoveUp(const int& row);

	/** TODO */
	bool mayMoveDown(const int& row);

	/** TODO */
	void moveUp(const int& row);

	/** TODO */
	void moveDown(const int& row);

	/** TODO */
	void sortOrder();

	/** TODO */
	void removeRows(const QModelIndexList& rows);

	/** TODO */
	void renameLayer(const QString& id, const QString& name);

	/** TODO */
	LayerItem* findLayerById(const QString& id);

	/** TODO */
	void addLayer(LayerItem* layer);

private:
	ImageViewerPlugin*		_imageViewerPlugin;		/** TODO */
	Layers					_layers;				/** TODO */
	QItemSelectionModel		_selectionModel;		/** TODO */

	friend class MainModel;
};