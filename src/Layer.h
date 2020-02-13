#pragma once

#include "ImageData/Images.h"

#include <QImage>

class Dataset;

/**
 * Layer class
 * @author Thomas Kroes
 */
class Layer : public QObject
{
	Q_OBJECT

public:
	/** Constructor */
	Layer(Dataset* dataset);

	/** Destructor */
	virtual ~Layer();

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

	/** Returns the layer image */
	const QImage image() const;

	/**
	 * Sets the image
	 * @param image Layer image
	 */
	void setImage(const QImage& image);

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
	 * Signals the opacity changed
	 * @param opacity Opacity of the layer
	 */
	void opacityChanged(const float& opacity);

	/**
	 * Signals the image changed
	 * @param image Layer image
	 */
	void imageChanged(const QImage& image);

private:
	Dataset*			_dataset;	/** Parent dataset */
	QString				_name;		/** Name of the layer */
	std::uint32_t		_order;		/** Depth at which to render the layer */
	float				_opacity;	/** Opacity  of the layer */
	QImage				_image;		/** Layer image */
};