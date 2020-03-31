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
	bool removeColumns(int position, int columns,
		const QModelIndex &parent = QModelIndex()) override;
	bool insertRows(int position, int rows,
		const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int position, int rows,
		const QModelIndex &parent = QModelIndex()) override;

	Qt::DropActions supportedDropActions() const
	{
		return Qt::MoveAction;
	}

	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {

		if (!canDropMimeData(data, action, row, column, parent))
			return false;

		if (action == Qt::IgnoreAction)
			return true;

		int beginRow;

		if (row != -1)
			beginRow = row;
		else if (parent.isValid())
			beginRow = parent.row();
		else
			beginRow = rowCount(QModelIndex());

		insertRow(beginRow, parent);
		setData(parent.child(beginRow, 0), "asdsad");

		return true;

	}

private:
	void setupModelData(const QStringList &lines, TreeItem *parent);
	TreeItem *getItem(const QModelIndex &index) const;

	TreeItem *rootItem;
};