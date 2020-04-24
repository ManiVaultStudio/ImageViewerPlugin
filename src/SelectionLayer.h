#pragma once

#include "LayerNode.h"

class Points;
class Images;

/**
 * Selection layer class
 *
 * TODO
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

	/** Initializes the layer */
	void init();

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

	/** Computes the selection image */
	void computeImage();

signals:

	/**
	 * Signals that the image changed
	 * @param image Image
	 */
	void imageChanged(const QImage& image);

private:
	Points*					_pointsDataset;			/** Points dataset to which the layer refers */
	Images*					_imagesDataset;			/** Images dataset from which the points dataset originates */
	QImage					_image;					/** Selection image */
	QVector<std::uint8_t>	_imageData;				/** Image data buffer */
};