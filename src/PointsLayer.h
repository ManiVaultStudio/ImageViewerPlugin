#pragma once

#include "Layer.h"
#include "Channel.h"

class QPainter;

class Points;
class Images;

/**
 * Points layer class
 *
 * Layer for points data
 *
 * @author Thomas Kroes
 */
class PointsLayer : public Layer, public virtual Channels<float>
{
	Q_OBJECT

public: // Enumerations

	/** TODO */
	enum class ChannelIndex {
		Channel1,		/** Channel 1 */
		Channel2,		/** Channel 2 */
		Channel3,		/** Channel 3 */
		Mask,			/** Mask channel */

		Count = Mask + 1
	};

	/** TODO */
	enum class PixelType {
		Intensity,		/** Intensity (color images) */
		Index			/** Index (index images) */
	};

	/** TODO */
	static QString pixelTypeName(const PixelType& pixelType)
	{
		switch (pixelType)
		{
			case PixelType::Intensity:
				return "Intensity";

			case PixelType::Index:
				return "Index";

			default:
				break;
		}

		return QString();
	}

	/**  Columns */
	enum class Column {
		Channel1Name = ult(Layer::Column::End) + 1,		/** First input channel name */
		Channel2Name,									/** Second input channel name */
		Channel3Name,									/** Third input channel name */
		Channel1DimensionId,							/** First input channel dimension identifier */
		Channel2DimensionId,							/** Second input channel dimension identifier */
		Channel3DimensionId,							/** Third input channel dimension identifier */
		Channel1Enabled,								/** First input channel enabled */
		Channel2Enabled,								/** Second input channel enabled */
		Channel3Enabled,								/** Third input channel enabled */
		NoChannels,										/** Occupied number of channels */
		MaxNoChannels,									/** The maximum number of channels */
		DimensionNames,									/** Dimension names */
		NoPoints,										/** Number of points in the dataset */
		NoDimensions,									/** Number of dimensions in the dataset */
		ColorSpace,										/** Color space (e.g. RGB, HSL and LAB) */
		ColorMap,										/** Color map image */
		UseConstantColor,								/** Whether to use constant colors for shading */
		ConstantColor,									/** Const color */
		PixelType,										/** Type of pixel (e.g. intensity, index) */
		LinkedPointsDatasetName,						/** Name of the linked points dataset */
		LinkedPointsSelection,							/** Selection of the linked points dataset */

		Start = Channel1Name,							/** Start column */
		End = LinkedPointsSelection						/** End column */
	};

public:

	/**
	 * Constructor
	 * @param pointsDatasetName Name of the points dataset
	 * @param id Layer identifier
	 * @param name Layer name
	 * @param flags Configuration bit flags
	 */
	PointsLayer(const QString& pointsDatasetName, const QString& id, const QString& name, const int& flags);

protected: // Initialization

	/** Initializes the layer */
	void init();

public:

	/**
	 * Adjust the layer scaling to fit into the supplied image size
	 * @param imageSize Size of the image to scale into
	 */
	void matchScaling(const QSize& targetImageSize);

public: // 2D painting

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
	 * Returns the number of points in the dataset
	 * @param role Data role
	 * @return Number of points in the dataset in variant form
	 */
	QVariant noPoints(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the number of points in the dataset
	 * @param noPoints Number of points in the dataset
	 */
	void setNoPoints(const std::uint32_t& noPoints);

	/**
	 * Returns the number of dimensions in the dataset
	 * @param role Data role
	 * @return Number of dimensions in the dataset in variant form
	 */
	QVariant noDimensions(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the number of dimensions in the dataset
	 * @param noDimensions Number of dimensions in the dataset
	 */
	void setNoDimensions(const std::uint32_t& noDimensions);

	/**
	 * Returns the dimension names in the dataset
	 * @param role Data role
	 * @return Dimension names in variant form
	 */
	QVariant dimensionNames(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the dimension names
	 * @param dimensionNames Dimension names
	 */
	void setDimensionNames(const QStringList& dimensionNames);

	/**
	 * Returns whether a channel is enabled
	 * @param id Channel identifier
	 * @param role Data role
	 * @return Whether a channel is enabled in variant form
	 */
	QVariant channelEnabled(const ChannelIndex& channelIndex, const int& role = Qt::DisplayRole) const;

	/**
	 * Sets whether a channel is enabled
	 * @param id Channel identifier
	 * @param enabled Whether the channel is enabled
	 */
	void setChannelEnabled(const ChannelIndex& channelIndex, const bool& enabled);

	/**
	 * Returns channel name by identifier
	 * @param id Channel identifier
	 * @param role Data role
	 * @return Channel name in variant form
	 */
	QVariant channelName(const ChannelIndex& channelIndex, const int& role = Qt::DisplayRole) const;

	/**
	 * Sets a channel dimension identifier
	 * @param id Channel identifier
	 * @param name Channel name
	 */
	void setChannelName(const int& id, const QString& name);

	/**
	 * Returns a channel by identifier
	 * @param id Channel identifier
	 * @param role Data role
	 * @return Channel identifier in variant form
	 */
	QVariant channelDimensionId(const ChannelIndex& channelIndex, const int& role = Qt::DisplayRole) const;

	/**
	 * Sets a channel dimension identifier
	 * @param id Channel identifier
	 * @param dimension Dimension identifier
	 */
	void setChannelDimensionId(const ChannelIndex& channelIndex, const std::uint32_t& dimensionId);

	/**
	 * Returns the maximum number of channels
	 * @param role Data role
	 * @return Maximum number of channels in variant form
	 */
	QVariant maxNoChannels(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the maximum number of channels
	 * @param maxNoChannels Maximum number of channels
	 */
	void setMaxNoChannels(const std::uint32_t& maxNoChannels);

	/**
	 * Returns the number of channels
	 * @param role Data role
	 * @return Number of channels in variant form
	 */
	QVariant noChannels(const int& role = Qt::DisplayRole) const;

	/**
	 * Returns the color space
	 * @param role Data role
	 * @return Color space in variant form
	 */
	QVariant colorSpace(const int& role) const;

	/**
	 * Sets the color space
	 * @param colorSpace Color space
	 */
	void setColorSpace(const ColorSpace& colorSpace);

	/**
	 * Returns the color map image
	 * @param role Data role
	 * @return Color map image in variant form
	 */
	QVariant colorMap(const int& role) const;

	/**
	 * Sets the color map image
	 * @param colorMap Color map image
	 */
	void setColorMap(const QImage& colorMap);

	/**
	 * Returns whether to shade using constant color
	 * @param role Data role
	 * @return Whether to shade using constant color in variant form
	 */
	QVariant useConstantColor(const int& role) const;

	/**
	 * Sets whether to shade using constant color
	 * @param useConstantColor Constant color on/off
	 */
	void setUseConstantColor(const bool& useConstantColor);

	/**
	 * Returns the constant color
	 * @param role Data role
	 * @return Constant color in variant form
	 */
	QVariant constantColor(const int& role) const;

	/**
	 * Sets the constant color
	 * @param constantColor Constant color
	 */
	void setConstantColor(const QColor& constantColor);

	/**
	 * Returns the pixel type
	 * @param role Data role
	 * @return Pixel type in variant form
	 */
	QVariant pixelType(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the pixel type
	 * @param pixelType Type of pixel
	 */
	void setPixelType(const PixelType& pixelType);

	/**
	 * Returns the name of the linked points dataset
	 * @param role Data role
	 * @return Name of the linked points dataset in variant form
	 */
	QVariant linkedPointsDatasetName(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the name of the linked points dataset
	 * @param linkedPointsDatasetName Name of the linked points dataset
	 */
	void setLinkedPointsDatasetName(const QString& linkedPointsDatasetName);

	/**
	 * Returns the selection of the linked points dataset
	 * @param role Data role
	 * @return Selection of the linked points dataset in variant form
	 */
	QVariant linkedPointsSelection(const int& role = Qt::DisplayRole) const;

	/**
	 * Sets the selection of the linked points dataset
	 * @param linkedPointsSelection Selection of the linked points dataset
	 */
	void setLinkedPointsSelection(const Indices& linkedPointsSelection);

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
	 * Computes a specific channel
	 * @param channelIndex Channel identifier
	 */
	void computeChannel(const ChannelIndex& channelIndex);

	/** Updates the channel names */
	void updateChannelNames();

signals:

	/**
	 * Signals that the color space has changed
	 * @param colorSpace Color space
	 */
	void colorSpaceChanged(const ColorSpace& colorSpace);

	/**
	 * Signals that the color map has changed
	 * @param colorMap Color map
	 */
	void colorMapChanged(const QImage& colorMap);

private:
	PixelType			_pixelType;						/** Type of pixel (e.g. intensity, index) */
	QString				_linkedPointsDatasetName;		/** Name of the linked points dataset */
	Indices				_linkedPointsSelection;			/** Selection of the linked points dataset */
	Points*				_pointsDataset;					/** Points dataset to which the layer refers */
	Images*				_imagesDataset;					/** Images dataset from which the points dataset originates */
	std::uint32_t		_noPoints;						/** Number of points in the dataset */
	std::uint32_t		_noDimensions;					/** Number of dimensions in the points dataset */
	QStringList			_dimensionNames;				/** Dimension names in the points dataset */
	std::uint32_t		_maxNoChannels;					/** Maximum number of channels (determined by the number of dimensions) */
	ColorSpace			_colorSpace;					/** Color space */
	QImage				_colorMap;						/** Color map (1D/2D) */
	bool				_useConstantColor;				/** Pixel color is constant and the alpha is modulated by the intensity of the selected channel */
	QColor				_constantColor;					/** Constant color */
};