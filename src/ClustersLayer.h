#pragma once

#include "LayerNode.h"

class ClustersDataset;

/**
 * Clusters layer class
 *
 * Layer for cluster data
 *
 * @author Thomas Kroes
 */
class ClustersLayer : public LayerNode
{
public:
	
	/**  Columns */
	enum class Column {
		Start = ult(LayerNode::Column::End) + 1,
		End = Start
	};

public:

	/**
	 * Constructor
	 * @param dataset Name of the cluster dataset
	 * @param id Layer identifier
	 * @param name Layer name
	 * @param flags Configuration bit flags
	 */
	ClustersLayer(const QString& dataset, const QString& id, const QString& name, const int& flags);

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
};