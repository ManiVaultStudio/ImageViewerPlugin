#pragma once

#include "LayerNode.h"
#include "Channel.h"

class Points;
class Images;

/**
 * Points layer class
 *
 * Layer for points data
 *
 * @author Thomas Kroes
 */
class PointsLayer : public LayerNode
{
	Q_OBJECT

public:

	/**  Columns */
	enum class Column {
		ImageSize = ult(LayerNode::Column::End) + 1,		// Image size
		Channel1DimensionId,								// First input channel dimension identifier
		Channel2DimensionId,								// Second input channel dimension identifier
		Channel3DimensionId,								// Third input channel dimension identifier
		Channel1Enabled,									// First input channel enabled
		Channel2Enabled,									// Second input channel enabled
		Channel3Enabled,									// Third input channel enabled
		NoChannels,											// Occupied number of channels
		MaxNoChannels,										// The maximum number of channels
		DimensionNames,										// Dimension names
		NoPoints,											// Number of points in the dataset
		NoDimensions,										// Number of dimensions in the dataset
		ColorMap,											// Color map image
		SolidColor,											// Solid color

		Start = ImageSize,
		End = SolidColor
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
	 * Returns the image size
	 * @param role Data role
	 * @return Image size in variant form
	 */
	QVariant imageSize(const int& role = Qt::DisplayRole) const;

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
	 * Returns channel by identifier
	 * @param id Channel identifier
	 * @return Channel
	 */
	Channel* channel(const std::uint32_t& id);

	/**
	 * Returns whether a channel is enabled
	 * @param id Channel identifier
	 * @param role Data role
	 * @return Whether a channel is enabled in variant form
	 */
	QVariant channelEnabled(const std::uint32_t& id, const int& role = Qt::DisplayRole) const;

	/**
	 * Sets whether a channel is enabled
	 * @param id Channel identifier
	 * @param enabled Whether the channel is enabled
	 */
	void setChannelEnabled(const int& id, const bool& enabled);

	/**
	 * Returns a channel by identifier
	 * @param id Channel identifier
	 * @param role Data role
	 * @return Channel variant form
	 */
	QVariant channelDimensionId(const std::uint32_t& id, const int& role = Qt::DisplayRole) const;

	/**
	 * Sets a channel dimension identifier
	 * @param id Channel identifier
	 * @param dimension Dimension identifier
	 */
	void setChannelDimensionId(const int& id, const std::uint32_t& dimensionId);

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
	 * Returns whether the solid color option is enabled
	 * @param role Data role
	 * @return Whether the solid color option is enabled in variant form
	 */
	QVariant solidColor(const int& role) const;

	/**
	 * Sets whether the solid color option is enabled
	 * @param colorMap Color map image
	 */
	void setSolidColor(const bool& solidColor);

private:

	/** Computes the channels image */
	void computeChannel(const std::uint32_t& id);

signals:

	/**
	 * Signals that the color map has changed
	 * @param colorMap Color map
	 */
	void colorMapChanged(const QImage& colorMap);

private:
	Points*				_pointsDataset;			/** Points dataset to which the layer refers */
	Images*				_imagesDataset;			/** Images dataset from which the points dataset originates */
	std::uint32_t		_noPoints;				/** Number of points in the dataset */
	std::uint32_t		_noDimensions;			/** Number of dimensions in the points dataset */
	QStringList			_dimensionNames;		/** Dimension names in the points dataset */
	QVector<Channel*>	_channels;				/** Channels */
	std::uint32_t		_maxNoChannels;			/** Maximum number of channels (determined by the number of dimensions) */
	QImage				_colorMap;				/** Color map (1D/2D) */
	bool				_solidColor;			/** Solid color */
};