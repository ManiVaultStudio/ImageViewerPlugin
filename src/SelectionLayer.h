#pragma once

#include "LayerNode.h"

/**
 * Clusters layer class
 *
 * Layer for cluster data
 *
 * @author Thomas Kroes
 */
class SelectionLayer : public LayerNode
{
	Q_OBJECT

public:

	/**  Columns */
	enum class Column {
		Start = ult(LayerNode::Column::End) + 1,
		End = Start
	};

public:

	/**
	 * Constructor
	 * @param datasetName Name of the dataset
	 * @param id Layer identifier
	 * @param name Layer name
	 * @param flags Configuration bit flags
	 */
	SelectionLayer(const QString& datasetName, const QString& id, const QString& name, const int& flags);

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
	 * Returns the selection image
	 * @param role Data role
	 * @return Selection image in variant form
	 */
	QVariant image(const int& role) const;

	/**
	 * Sets the selection image
	 * @param image Selection image
	 */
	void setImage(const QImage& image);

signals:

	/**
	 * Signals that the selection image changed
	 * @param image Selection image
	 */
	void imageChanged(const QImage& image);

private:
	QImage		_image;				/** Selection image */
};