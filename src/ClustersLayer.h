#pragma once

#include "Layer.h"

class QPaintEvent;

class Clusters;

/**
 * Clusters layer class
 *
 * Layer for cluster data
 *
 * @author Thomas Kroes
 */
class ClustersLayer : public Layer
{
public:

	/**  Columns */
	enum class Column {
		Start = ult(Layer::Column::End) + 1,
		End = Start
	};

public:

	/**
	 * Constructor
	 * @param clusterDatasetName Name of the cluster dataset
	 * @param id Layer identifier
	 * @param name Layer name
	 * @param flags Configuration bit flags
	 */
	ClustersLayer(const QString& clusterDatasetName, const QString& id, const QString& name, const int& flags);

	/**
	 * Paints the layer
	 * @param painter Pointer to painter
	 */
	void paint(QPainter* painter) override;

public: // Inherited MVC

	/** Returns the number of columns */
	int columnCount() const override { return ult(Column::End) + 1; }

	/**
	 * Returns the item flags for the given model index
	 * @param index Model index
	 * @return Item flags for the index
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/**
	 * Returns the data for the given model index and data role
	 * @param index Model index
	 * @param role Data role
	 * @return Data in variant form
	 */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/**
	 * Sets the data value for the given model index and data role
	 * @param index Model index
	 * @param value Data value in variant form
	 * @param role Data role
	 * @return Model indices that are affected by the operation
	 */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

public: // Getters/setters

	/**
	 * Returns the name of the clusters dataset to which the layer refers
	 * @param role Data role
	 * @return Name of the clusters dataset to which the layer refers in variant form
	 */
	QVariant clustersDatasetName(const int& role = Qt::DisplayRole) const;

private:
	QString			_clustersDatasetName;		/** Name of the clusters dataset to which the layer refers */
	Clusters*		_clustersDataset;			/** Clusters dataset to which the layer refers */
};