#pragma once

#include "Layer.h"
#include "Channel.h"

#include "ImageData/Images.h"

class QPainter;

/**
 * Images layer class
 *
 * Layer for images data
 *
 * @author Thomas Kroes
 */
class ImagesLayer : public Layer, public Channels<float>
{
public:

	/**  Columns */
	enum class Column {
		NoImages = ult(Layer::Column::End) + 1,		// Number of images in the dataset
		WindowNormalized,								// Normalized window
		LevelNormalized,								// Normalized level
		ImageNames,										// Image name(s)
		FilteredImageNames,								// Filtered image name(s) (depends on the selection)
		ImageIDs,										// Image identifiers
		ImageFilePaths,									// Absolute image file paths
		CurrentImageId,									// Current image identifier
		CurrentImageName,								// Current image name
		CurrentImageFilePath,							// Current image file path
		Average,										// Whether to average the images

		Start = NoImages,
		End = Average
	};

	/**
	 * Constructor
	 * @param dataset Name of the images dataset
	 * @param id Layer identifier
	 * @param name Layer name
	 * @param flags Configuration bit flags
	 */
	ImagesLayer(const QString& imagesDatasetName, const QString& id, const QString& name, const int& flags);

	/** Initializes the layer */
	void init();

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
	 * Returns the image data type (e.g. sequence or stack)
	 * @param role The data role
	 * @return Image data type in variant form
	 */
	QVariant imageDataType(const int& role) const;

	/**
	 * Sets the image data type
	 * @param imageDataType Image data type
	 */
	void setImageDataType(const ImageData::Type& imageDataType);

	/**
	 * Returns the number of images in the dataset
	 * @param role The data role
	 * @return Number of images in variant form
	 */
	QVariant noImages(const int& role) const;

	/**
	 * Returns the image names in the dataset
	 * @param role The data role
	 * @return Image names in variant form
	 */
	QVariant imageNames(const int& role) const;

	/**
	 * Returns the image identifiers in the dataset
	 * @param role The data role
	 * @return Image identifiers in variant form
	 */
	QVariant imageIds(const int& role) const;

	/**
	 * Sets the image names
	 * @param imageNames Image names
	 */
	void setImageNames(const QStringList& imageNames);

	/**
	 * Returns the image file paths in the dataset
	 * @param role The data role
	 * @return Image file paths in variant form
	 */
	QVariant imageFilePaths(const int& role) const;

	/**
	 * Sets the image file paths (absolute)
	 * @param imageFilePaths Image file paths
	 */
	void setImageFilePaths(const QStringList& imageFilePaths);

	/**
	 * Returns the referenced points dataset name
	 * @param role The data role
	 * @return Referenced points dataset name in variant form
	 */
	QVariant pointsName(const int& role) const;

	/**
	 * Sets the referenced points dataset name
	 * @param pointsName Referenced points dataset name
	 */
	void setPointsName(const QString& pointsName);

	/**
	 * Returns the filtered image names (based on selection)
	 * @param role The data role
	 * @return Filtered image names in variant form
	 */
	QVariant filteredImageNames(const int& role) const;

	/**
	 * Returns the current image identifier
	 * @param role The data role
	 * @return Current image identifier in variant form
	 */
	QVariant currentImageId(const int& role) const;

	/**
	 * Sets the current image identifier
	 * @param currentImageId Current image identifier
	 */
	void setCurrentImageId(const std::uint32_t& currentImageId);

	/**
	 * Returns the current image name
	 * @param role The data role
	 * @return Current image name in variant form
	 */
	QVariant currentImageName(const int& role) const;

	/**
	 * Sets the current image file path
	 * @param currentImage Current image file path
	 */
	QVariant currentImageFilePath(const int& role) const;

	/**
	 * Returns whether images are averaged
	 * @param role The data role
	 * @return Whether images are averaged in variant form
	 */
	QVariant average(const int& role) const;

	/**
	 * Sets whether images are averaged
	 * @param average Average images
	 */
	void setAverage(const bool& average);

protected:

	/**
	* Returns the image size
	* @return Image size in variant form
	*/
	QSize imageSize() const override;

	/** Returns hints that pertain to the layer */
	Hints hints() const override;

private:

	/**
	 * Computes image channel
	 * @param id Identifier
	 */
	void computeChannel(const std::uint32_t& id);

private:
	Images*				_images;				/** Pointer to the images plugin instance */
	ImageData::Type		_imageDataType;			/** Image data type (e.g. sequence or stack) */
	QStringList			_imageNames;			/** Image names */
	QStringList			_imageFilePaths;		/** Image file paths */
	QString				_pointsName;			/** Name of the referenced points dataset */
	std::int32_t		_currentImageId;		/** Current image identifier */
	bool				_average;				/** Whether to average images */
};