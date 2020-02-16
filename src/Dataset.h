#pragma once

#include "ImageData/Images.h"

#include "Common.h"
#include "Layer.h"

class QMenu;

/**
 * Dataset class
 * @author Thomas Kroes
 */
class Dataset : public QObject
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param name Name of the dataset
	 * @param images Pointer to HDPS images dataset
	 */
	Dataset(const QString& name, Images* images);

	/** Destructor */
	virtual ~Dataset();

public:
	/** Returns the dataset name */
	QString name() const;

	/** Returns the image size */
	QSize imageSize() const;

	/** Returns the image names */
	QStringList imageNames() const;

	/** Returns the dimension names */
	QStringList dimensionNames() const;

	/** Returns the current image index */
	auto currentImageId() const;

	/**
	 * Sets the current image index
	 * @param currentImageIndex Current image index
	 */
	void setCurrentImageId(const std::int32_t& currentImageIndex);

	/** Returns the current dimension index */
	auto currentDimensionIndex() const;

	/**
	 * Sets the current dimension index
	 * @param currentDimensionIndex Current dimension index
	 */
	void setCurrentDimensionIndex(const std::int32_t& currentDimensionIndex);

	/** Returns whether images are averaged */
	bool average() const;

	/**
	 * Sets whether sequence images are averaged
	 * @param averageImages Whether sequence images are averaged
	 */
	void setAverage(const bool& averageImages);

	/** Returns whether this data set can be averaged */
	bool canAverage() const;

	/** Returns the selection opacity */
	float selectionOpacity() const;

	/**
	 * Sets the selection opacity
	 * @param selectionOpacity Selection opacity
	 */
	void setSelectionOpacity(const float& selectionOpacity);

	/** The dataset selection has changed */
	void setSelectionChanged();

	/** Returns the image collection type */
	ImageCollectionType imageCollectionType() const;

	/** Dataset context menu */
	QMenu* contextMenu() const;

public: // Selection

	/** Returns the selection */
	std::vector<std::uint32_t> selection() const;

	/** Returns the number of selected pixels */
	std::uint32_t noSelectedPixels() const;

	/** Returns whether a selection is present */
	bool hasSelection() const;

	/** Whether image data set can be selected */
	bool isPixelSelectionAllowed() const;

	/**
	 * Select pixels
	 * @param pixelCoordinates Pixel coordinates to select
	 * @param selectionModifier Selection modifier
	 */
	void selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier);

	/** Creates an image subset from the current selection */
	void createSubsetFromSelection();

public: // Layers

	/**
	* Add layer by name
	* @param name Name of the layer
	*/
	void addLayerByName(const QString& name)
	{
		_layers.insert(name, QSharedPointer<Layer>::create(this, name));
	}

	/** Returns const pointer to layer by name */
	const Layer* layerByName(const QString& name) const
	{
		return dynamic_cast<Layer*>(_layers[name].get());
	}

	/** Returns pointer to layer by name */
	Layer* layerByName(const QString& name)
	{
		const auto constThis = const_cast<const Dataset*>(this);
		return const_cast<Layer*>(constThis->layerByName(name));
	}

	/** Returns layer names */
	QStringList layerNames();

private:
	/** Computes the color image */
	void computeColorImage();

	/** Computes the selection image */
	void computeSelectionImage();

signals:
	/**
	 * Signals the current image index changed
	 * @param currentImageIndex Current image index
	 */
	void currentImageIndexChanged(const std::int32_t& currentImageIndex);

	/**
	 * Signals the current dimension index changed
	 * @param currentDimensionIndex Current dimension index
	 */
	void currentDimensionIndexChanged(const std::int32_t& currentDimensionIndex);

	/**
	 * Signals the average parameter changed
	 * @param average Whether to average images
	 */
	void averageChanged(const bool& average);

	/**
	 * Signals the selection opacity changed
	 * @param selectionOpacity Selection opacity
	 */
	void selectionOpacityChanged(const float& selectionOpacity);

	/** Signals the selection changed */
	void selectionChanged();

	/**
	 * 
	 */
	void addLayer(Layer* layer);

	/**
	 *
	 */
	void removeLayer(Layer* layer);

private:
	QString									_name;						/** Dataset name */
	Images*									_dataset;					/** Pointer to the Images HDPS data structure */
	QStringList								_imageNames;				/** Image names */
	std::int32_t							_currentImageIndex;			/** The index of the current image  */
	QStringList								_dimensionNames;			/** Dimension names */
	std::int32_t							_currentDimensionIndex;		/** The index of the current dimension */
	bool									_average;					/** Whether to average images */
	float									_selectionOpacity;			/** Selection opacity */
	QMap<QString, QSharedPointer<Layer>>	_layers;					/** Layers */
};