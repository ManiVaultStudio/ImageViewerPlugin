#pragma once

#include "TreeItem.h"

#include <QAbstractListModel>
#include <QAbstractItemView>

class ImageViewerPlugin;
class Dataset;
class Layer;
class GroupLayer;

class LayersModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	/** TODO */
	LayersModel(QObject *parent = nullptr);

	/** TODO */
	~LayersModel();

public: // Data access

	/** TODO */
	QVariant data(const QModelIndex &index, int role) const override;
	
	/** TODO */
	QVariant data(const int& row, const int& column, const int& role) const;

	/** TODO */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	/** TODO */
	bool setData(const int& row, const int& column, const QVariant& value, int role = Qt::EditRole);

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	/** TODO */
	Qt::ItemFlags flags(const int& row, const int& column) const;

	/** TODO */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	/** TODO */
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	
	/** TODO */
	QModelIndex parent(const QModelIndex &index) const override;

	/** TODO */
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	/** TODO */
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	/** TODO */
	bool insertLayer(int row, Layer* layer, const QModelIndex& parent = QModelIndex());

	/** TODO */
	bool removeLayer(const QModelIndex& index);

	/** TODO */
	bool mayMoveLayer(const QModelIndex& index, const int& delta) const;

	/** TODO */
	bool moveLayer(const QModelIndex& sourceParent, const int& sourceRow, const QModelIndex& targetParent, int targetRow);
	
	/** TODO */
	Qt::DropActions supportedDropActions() const
	{
		return Qt::MoveAction | Qt::CopyAction;
	}

public: // Miscellaneous

	/** TODO */
	QItemSelectionModel& selectionModel() { return _selectionModel; }

public: // MIME drag and drop

	/** TODO */
	QStringList mimeTypes() const;

	/** TODO */
	QMimeData* mimeData(const QModelIndexList& indexes) const;

	/** TODO */
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

private:
	Layer* getLayer(const QModelIndex& index) const;

private:
	QItemSelectionModel		_selectionModel;	/** TODO */
	GroupLayer*				_root;				/** TODO */
};