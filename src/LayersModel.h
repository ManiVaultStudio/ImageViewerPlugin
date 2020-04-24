#pragma once

#include "Common.h"

#include <QAbstractListModel>
#include <QAbstractItemView>

class ImageViewerPlugin;
class Dataset;
class LayerNode;
class RootLayer;

/**
 * Layers model class
 *
 * Provides a hierarchical layer model for image display
 *
 * @author Thomas Kroes
 */
class LayersModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	/**
	 * Constructor
	 * @param imageViewerPlugin Pointer to parent image viewer plugin
	 */
	LayersModel(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~LayersModel();

public: // Data access

	/**
	 * Returns the data for the given model index and data role
	 * @param index Model index
	 * @param role Data role
	 * @return Data in variant form
	 */
	QVariant data(const QModelIndex &index, int role) const override;
	
	/**
	 * Returns the data for the given model row, column and data role
	 * @param row Model row
	 * @param column Model column
	 * @param role Data role
	 * @return Data in variant form
	 */
	QVariant data(const int& row, const int& column, const int& role) const;

	/**
	 * Sets the data value for the given model index and data role
	 * @param index Model index
	 * @param value Data value in variant form
	 * @param role Data role
	 * @return Whether the data was properly set or not
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	/**
	 * Sets the data value for the given model row, column and data role
	 * @param row Model row
	 * @param column Model column
	 * @param value Data value in variant form
	 * @param role Data role
	 * @return Whether the data was properly set or not
	 */
	bool setData(const int& row, const int& column, const QVariant& value, int role = Qt::EditRole);

	/**
	 * Returns the item flags for the given model index
	 * @param index Model index
	 * @return Item flags for the index
	 */
	Qt::ItemFlags flags(const QModelIndex &index) const override;

	/**
	 * Returns the item flags for the given model row and column
	 * @param row Model row
	 * @param column Model column
	 * @return Item flags for the index
	 */
	Qt::ItemFlags flags(const int& row, const int& column) const;

	/**
	 * Returns the header data for the given section, orientation and data role
	 * @param section Model section
	 * @param orientation Orientation (e.g. horizontal or vertical)
	 * @param role Data role
	 * @return Header data in variant form
	 */
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	/**
	 * Returns the model index belonging to the given model row and column
	 * @param row Model row
	 * @param column Model column
	 * @param parent Parent model index
	 * @return Model index for the given model row and column
	 */
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	
	/**
	 * Returns the parent model index
	 * @param index Model index
	 * @return Parent model index for the given model index
	 */
	QModelIndex parent(const QModelIndex& index) const override;

	/**
	 * Returns the number of rows in the model given the parent model index
	 * @param parent Parent model index
	 * @return Number of rows in the model given the parent model index
	 */
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	/**
	 * Returns the number of columns in the model given the parent model index
	 * @param parent Parent model index
	 * @return Number of columns in the model given the parent model index
	 */
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	/**
	 * Inserts a layer into the model at the specified row give the parent index
	 * @param row Row at which to insert the layer
	 * @param layer Layer to insert
	 * @param parent Parent model index
	 * @return Whether the layer was successfully inserted
	 */
	bool insertLayer(int row, LayerNode* layer, const QModelIndex& parent = QModelIndex());

	/**
	 * Removes a layer at the specified model index
	 * @param index Model index of the layer to remove
	 * @return Whether the layer was successfully removed
	 */
	bool removeLayer(const QModelIndex& index);

	/**
	 * Determines whether a layer may be moved by delta
	 * @param index Model index to be moved
	 * @param delta Amount to be moved up/down
	 * @return Whether the layer may be moved up/down by delta
	 */
	bool mayMoveLayer(const QModelIndex& index, const int& delta) const;

	/**
	 * Move layer to a new destination
	 * @param sourceParent The parent model index at the source location
	 * @param sourceRow The row identifier at the source location
	  * @param targetParent The parent model index at the target location
	 * @param targetRow The row identifier at the target location
	 * @return Whether the layer was successfully moved
	 */
	bool moveLayer(const QModelIndex& sourceParent, const int& sourceRow, const QModelIndex& targetParent, int targetRow);
	
	/** Returns which drop actions are supported by the model */
	Qt::DropActions supportedDropActions() const
	{
		return Qt::MoveAction | Qt::CopyAction;
	}

public: // Miscellaneous

	/** Performs a one-time startup initialization */
	void initialize();

	/**
	 * Call this when a dataset selection has changed
	 * @param name Dataset name
	 */
	void selectionChanged(const QString& name, const Indices& indices);

	/**
	 * Select a single row
	 * @param row Row identifier
	 */
	void selectRow(const std::int32_t& row);

	/** Returns the selection model */
	QItemSelectionModel& selectionModel() { return _selectionModel; }

public: // MIME drag and drop

	/** Returns list of supported mime types (for drag and drop) */
	QStringList mimeTypes() const;

	/**
	 * Converts a list of model indices to mime data
	 * @param indexes List of model indices
	 * @return Mime data
	 */
	QMimeData* mimeData(const QModelIndexList& indexes) const;

	/**
	 * Invoked when mime data is dropped
	 * @param data Dropped mime data
	 * @param action Type of drop action
	 * @param row Row identifier where the drop takes place
	 * @param column Column identifier where the drop takes place
	 * @param parent Parent model index where the drop takes place
	 * @return Whether the mime data was properly dropped
	 */
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

public:
	/**
	 * Returns a layer by model index
	 * @param index Model index of the layer to obtain
	 * @return Layer node
	 */
	LayerNode* getLayer(const QModelIndex& index) const;

private:
	QItemSelectionModel		_selectionModel;	/** Selection model */
	RootLayer*				_root;				/** Root layer */
};