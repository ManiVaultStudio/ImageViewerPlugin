#pragma once

#include "ImageData/Images.h"

#include "Common.h"

/**
 * Images dataset class
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

	/** Returns the image collection type */
	ImageCollectionType imageCollectionType() const;

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
	 * Signals the color image changed
	 * @param colorImage Color image
	 */
	void colorImageChanged(QSharedPointer<QImage> colorImage);

	/**
	 * Invoked when the selection image changes
	 * @param selectionImage Selection image
	 * @param selectionBounds Bounds of the selected pixels in the selection image
	 */
	void selectionImageChanged(QSharedPointer<QImage> selectionImage, const QRect& selectionBounds);

private:
	QString			_name;						/** Dataset name */
	Images*			_images;					/** Pointer to the Images HDPS data structure */
	QStringList		_imageNames;				/** Image names */
	std::int32_t	_currentImageIndex;			/** The index of the current image  */
	QStringList		_dimensionNames;			/** Dimension names */
	std::int32_t	_currentDimensionIndex;		/** The index of the current dimension */
	bool			_average;					/** Whether to average images */
	float			_selectionOpacity;			/** Selection opacity */
};