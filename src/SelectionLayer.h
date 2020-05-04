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
		PixelSelectionType = ult(LayerNode::Column::End) + 1,		// Type of pixel selection e.g. rectangle, brush
		SelectAll,													// Select all pixels
		SelectNone,													// Select no pixels
		InvertSelection,											// Invert the pixel selection
		AutoZoomToSelection,										// Zoom automatically to the pixel selection
		ZoomToSelection,											// Zoom to the pixel selection
		OverlayColor = ult(LayerNode::Column::End) + 1,				// Selection overlay color

		Start = PixelSelectionType,
		End = OverlayColor
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

	/**
	 * Returns the selection overlay color
	 * @param role Data role
	 * @return Overlay color in variant form
	 */
	QVariant overlayColor(const int& role) const;

	/**
	 * Sets the selection overlay color
	 * @param overlayColor Overlay color
	 */
	void setOverlayColor(const QColor& overlayColor);

	/**
	 * Returns whether auto zoom is enabled
	 * @param role Data role
	 * @return whether auto zoom is enabled in variant form
	 */
	QVariant autoZoomToSelection(const int& role) const;

	/**
	 * Sets whether auto zoom is enabled
	 * @param autoZoomToSelection Whether auto zoom is enabled
	 */
	void setAutoZoomToSelection(const bool& autoZoomToSelection);

protected:

	/**
	* Returns the image size
	* @return Image size in variant form
	*/
	QSize imageSize() const override;

private: // Miscellaneous

	/** Computes the selection image */
	void computeImage();

	/** Selects all pixels */
	void selectAll();

	/** De-selects all pixels */
	void selectNone();

	/** Inverts the pixel selection */
	void invertSelection();

signals:

	/**
	 * Signals that the image changed
	 * @param image Image
	 */
	void imageChanged(const QImage& image);

private:
	Points*					_pointsDataset;				/** Points dataset to which the layer refers */
	Images*					_imagesDataset;				/** Images dataset from which the points dataset originates */
	QImage					_image;						/** Selection image */
	QVector<std::uint8_t>	_imageData;					/** Image data buffer */
	QColor					_overlayColor;				/** Selection overlay color */
	bool					_autoZoomToSelection;		/** Automatically zoom to selection */
};