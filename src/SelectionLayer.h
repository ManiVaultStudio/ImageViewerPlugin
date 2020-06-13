#pragma once

#include "Layer.h"
#include "Channel.h"

#include "ImageData/Images.h"

class QPainter;

class Points;

/**
 * Selection layer class
 *
 * Layer for selection visualization and manipulation
 *
 * @author Thomas Kroes
 */
class SelectionLayer : public Layer, public Channels<std::uint8_t>
{
	Q_OBJECT

public: // Enumerations

	/** Channel indices */
	enum class ChannelIndex {
		Selection,		/** Selection channel */

		Count = Selection + 1
	};

	/**  Columns */
	enum class Column {
		PixelSelectionType = ult(Layer::Column::End) + 1,		// Type of pixel selection
		PixelSelectionModifier,									// Pixel selection modifier
		BrushRadius,											// Brush radius
		SelectAll,												// Select all pixels
		SelectNone,												// Select no pixels
		InvertSelection,										// Invert the pixel selection
		AutoZoomToSelection,									// Zoom automatically to the pixel selection
		ZoomToSelection,										// Zoom to the pixel selection
		CreateSubset,											// Create subset
		OverlayColor,											// Selection overlay color

		Start = PixelSelectionType,
		End = OverlayColor
	};

public: // Construction

	/**
	 * Constructor
	 * @param datasetName Name of the dataset
	 * @param id Layer identifier
	 * @param name Layer name
	 * @param flags Configuration bit flags
	 */
	SelectionLayer(const QString& datasetName, const QString& id, const QString& name, const int& flags);

protected: // Initialization

	/** Initializes the layer */
	void init();

public: // Miscellaneous

	/**
	 * Paints the layer
	 * @param painter Pointer to painter
	 */
	void paint(QPainter* painter) override;

	/**
	 * Handles a widget event
	 * @param event Event
	 * @param index Model index
	 */
	void handleEvent(QEvent* event, const QModelIndex& index) override;

	/** Creates a subset from the selected pixels */
	void subsetFromSelectedPixels();

	/** Creates a subset from the selection bounds */
	void subsetFromSelectionBounds();

	/**
	* Returns the image size
	* @return Image size in variant form
	*/
	QSize imageSize() const override;

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
	 * Returns the pixel selection type
	 * @param role Data role
	 * @return Pixel selection type in variant form
	 */
	QVariant pixelSelectionType(const int& role) const;

	/**
	 * Sets the pixel selection type
	 * @param pixelSelectionType Pixel selection type
	 */
	void setPixelSelectionType(const SelectionType& pixelSelectionType);

	/**
	 * Returns the pixel selection modifier
	 * @param role Data role
	 * @return Pixel selection modifier in variant form
	 */
	QVariant selectionModifier(const int& role) const;

	/**
	 * Sets the pixel selection modifier
	 * @param pixelSelectionModifier Pixel selection modifier
	 */
	void pixelSelectionModifier(const SelectionModifier& pixelSelectionModifier);

	/**
	 * Returns the brush radius
	 * @param role Data role
	 * @return Brush radius in variant form
	 */
	QVariant brushRadius(const int& role) const;

	/**
	 * Sets the brush radius
	 * @param brushRadius Brush radius
	 */
	void setBrushRadius(const float& brushRadius);

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

	/** Returns hints that pertain to the layer */
	Hints hints() const override;

private: // Miscellaneous

	/**
	 * Computes a specific channel
	 * @param channelIndex Channel identifier
	 */
	void computeChannel(const ChannelIndex& channelIndex);

	/** Selects all pixels */
	void selectAll();

	/** De-selects all pixels */
	void selectNone();

	/** Inverts the pixel selection */
	void invertSelection();

	/** Zoom to selected pixels */
	void zoomToSelection();

	/** Takes the pixels that were selected by the selection tools and publishes it to HDPS */
	void publishSelection();

	/** Computes the selection bounding rectangle */
	void computeSelectionBounds();

private:
	Points*						_pointsDataset;				/** Points dataset to which the layer refers */
	QImage						_image;						/** Selection image */
	std::vector<std::int32_t>	_indices;					/** Indices */
	SelectionType				_selectionType;				/** Pixel selection type (e.g. rectangle, brush) */
	SelectionModifier			_selectionModifier;			/** Pixel selection modifier (e.g. replace, add) */
	float						_brushRadius;				/** Brush radius */
	QColor						_overlayColor;				/** Selection overlay color */
	bool						_autoZoomToSelection;		/** Automatically zoom to selection */
	QRect						_pixelBounds;				/** Visible pixels bounding rectangle */
	QRect						_selectionBounds;			/** Pixel selection bounding rectangle */

public:
	static const QColor toolColorForeground;	/** Foreground tool color for brushes and pens */
	static const QColor toolColorBackground;	/** Background tool color for brushes and pens */
	static const QColor fillColor;				/** Fill color */
	static const float minBrushRadius;			/** Minimum brush radius */
	static const float maxBrushRadius;			/** Maximum brush radius */
	static const float defaultBrushRadius;		/** Default brush radius */
	static const float controlPointSize;		/** Size of control points */
	static const float perimeterLineWidth;		/** Line width of the pixel selection tool perimeter */
	static const QPoint textPosition;			/** Position of explanatory text */
};
