#pragma once

#include "ImageData/Images.h"

#include <QImage>

class Dataset;

/**
 * Layer class
 * @author Thomas Kroes
 */
class ImageLayer : public QObject
{
	Q_OBJECT

public:
	/** Constructor */
	ImageLayer(Dataset* dataset, const QString& name);

	/** Destructor */
	virtual ~ImageLayer();

	/** Returns the parent dataset */
	Dataset* dataset();

	/** Returns the dataset name */
	QString name() const;

	/**
	 * Sets the name
	  @param name Name of the layer
	 */
	void setName(const QString& name);

	/** Returns the name relative to the layer */
	QString fullName() const;

	/** Returns the order */
	std::uint32_t order() const;

	/**
	 * Sets the order
	 * @param order Order of the layer
	 */
	void setOrder(const std::uint32_t& order);

	/** Returns the opacity */
	float opacity() const;

	/**
	 * Sets the opacity
	 * @param opacity Opacity of the layer
	 */
	void setOpacity(const float& opacity);

	/**
	 * Sets the image
	 * @param image Layer image
	 */
	void setImage(const QImage& image);

	/** Returns the image min and max values */
	QPair<float, float> imageRange() const;

	/** Returns the image min and max values */
	QPair<float, float> displayRange() const;

	/** Returns the normalized display window */
	float windowNormalized() const;

	/** Returns the display window */
	float window() const;

	/** Returns the normalized display level */
	float levelNormalized() const;

	/** Returns the display level */
	float level() const;

	/**
	 * TODO
	 */
	void setWindowLevel(const float& window, const float& level);

protected:
	/**
	 * TODO
	 */
	void computeImageRange();

	/**
	 * TODO
	 */
	void computeDisplayRange();

signals:
	/**
	 * Signals the name changed
	 * @param name Name
	 */
	void nameChanged(const QString& name);

	/**
	 * Signals the order changed
	 * @param order Order of the layer
	 */
	void orderChanged(const std::uint32_t& order);

	/**
	 * TODO
	 */
	void imageChanged(const QImage& image);

	/**
	 * TODO
	 */
	void opacityChanged(const float& opacity);

	/**
	 * TODO
	 */
	void imageRangeChanged(const float& min, const float& max);

	/**
	 * TODO
	 */
	void displayRangeChanged(const float& min, const float& max);


private:
	Dataset*				_dataset;				/** Parent dataset */
	QString					_name;					/** Name of the layer */
	std::uint32_t			_order;					/** Depth at which to render the layer */
	float					_opacity;				/** Opacity of the layer */
	QImage					_image;					/** Image */
	QPair<float, float>		_imageRange;			/** Image range */
	QPair<float, float>		_displayRange;			/** Display range */
	float					_windowNormalized;		/** Normalized display window */
	float					_levelNormalized;		/** Normalized display level */
	float					_window;				/** Display window */
	float					_level;					/** Display level */
};