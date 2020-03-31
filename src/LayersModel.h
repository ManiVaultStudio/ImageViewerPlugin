#pragma once

#include "TreeItem.h"

#include <QAbstractListModel>
#include <QAbstractItemView>

class ImageViewerPlugin;
class Dataset;
class GroupLayer;

class LayersModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	LayersModel(QObject *parent = nullptr);

	~LayersModel();

	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool setData(const QModelIndex &index, const QVariant &value,
		int role = Qt::EditRole) override;
	bool setHeaderData(int section, Qt::Orientation orientation,
		const QVariant &value, int role = Qt::EditRole) override;

	bool insertColumns(int position, int columns,
		const QModelIndex &parent = QModelIndex()) override;
	bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;

	bool insertLayer(int row, Layer* layer, const QModelIndex& parent = QModelIndex());

	bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

	bool moveRow(const QModelIndex& sourceParent, const int& sourceRow, const QModelIndex& targetParent, int targetRow);
	
	Qt::DropActions supportedDropActions() const
	{
		return Qt::MoveAction;
	}

public: // MIME

	QStringList mimeTypes() const;

	QMimeData* mimeData(const QModelIndexList& indexes) const;

	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

private:
	void setupModelData(const QStringList &lines, Layer *parent);
	Layer *getItem(const QModelIndex &index) const;

	Layer *rootItem;
};