#pragma once

#include "Layer.h"

#include <QAbstractListModel>
#include <QItemSelectionModel>

class ImageViewerPlugin;

/** TODO */
class LayersModel : public QAbstractListModel
{
public: // Columns
	
	/** TODO */
	enum Columns : int {
		Enabled,
		Type,
		Locked,
		ID,						// Name for internal use
		Name,					// Name in the user interface
		Dataset,				// Name of the dataset
		Flags,
		Frozen,
		Removable,
		Mask,
		Renamable,
		Order,
		Opacity,
		WindowNormalized,
		LevelNormalized,
		ColorMap,				//
		Image,
		ImageRange,
		DisplayRange
	};

public: // Construction/destruction

	/** Constructor */
	LayersModel(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~LayersModel();

	/** TODO */
	QItemSelectionModel& selectionModel() { return _selectionModel; }

public: // Inherited members

	/** TODO */
	int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/** TODO */
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

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
	bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild);

public: // Overloaded data access
	
	/** TODO */
	QVariant data(const int& row, const int& column, int role) const;

	/** TODO */
	void setData(const int& row, const int& column, const QVariant& value);

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
	bool doesLayerExist(const QString& id);

	/** TODO */
	void addLayer(const Layer& layer);

	/** TODO */
	void renameDefaultLayers(const QString& name);

	/** TODO */
	void setDefaultColorImage(const QImage& image);

	/** TODO */
	void setDefaultSelectionImage(const QImage& image);

private:
	ImageViewerPlugin*		_imageViewerPlugin;		/** TODO */
	Layers					_layers;				/** TODO */
	QItemSelectionModel		_selectionModel;		/** TODO */

	friend class MainModel;
};