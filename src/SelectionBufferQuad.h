#pragma once

#include "Quad.h"

#include <QColor>

/**
 * OpenGL Selection image quad class
 * @author Thomas Kroes
 */
class SelectionBufferQuad : public Quad
{
	Q_OBJECT

public:
	/** Constructor */
	SelectionBufferQuad(const QString& name = "SelectionBufferQuad", const float& z = 0.f);

	/** Renders the selection buffer quad */
	void render();

	/** Returns the quad size */
	QSize size() const;

	/**
	 * Sets the quad size
	 * @param size Quad size
	 */
	void setSize(const QSize& size);

	/** Returns the color */
	QColor color() const;

	/**
	 * Sets the color
	 * @param size Quad size
	 */
	void setColor(const QColor& color);

	/** Returns the opacity */
	float opacity() const;

	/**
	 * Sets the opacity
	 * @param opacity Opacity
	 */
	void setOpacity(const float& opacity);

	/** Returns the brush radius */
	float brushRadius() const;

	/**
	 * Sets the brush radius
	 * @param brushRadius Brush radius
	 */
	void setBrushRadius(const float& brushRadius);

	/** Returns the brush radius delta (amount to increasing/decreasing) */
	float brushRadiusDelta() const;

	/**
	 * Sets the brush radius delta (amount to increasing/decreasing)
	 * @param brushRadiusDelta Amount to add/remove
	 */
	void setBrushRadiusDelta(const float& brushRadiusDelta);

	/** Increase the brush size by _brushRadiusDelta */
	void brushSizeIncrease();

	/** Decrease the brush size by _brushRadiusDelta */
	void brushSizeDecrease();

	/** Updates the selection buffer
	 * @param mousePositions Mouse positions in world coordinates
	 * @param selectionType Selection type
	 */
	void update(std::vector<QVector3D> mousePositions, const SelectionType& selectionType);

	/** Reset the (temporary) selection buffer */
	void reset();

	/** Returns the current selection image */
	QSharedPointer<QImage> selectionImage() const;

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	void addShaderPrograms();

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

signals:
	/**
	 * Invoked when the size changed
	 * @param size Quad size
	 */
	void sizeChanged(const QSize& size);

	/**
	 * Invoked when the color changed
	 * @param color Color
	 */
	void colorChanged(const QColor& color);

	/**
	 * Invoked when the opacity changed
	 * @param opacity Selection opacity
	 */
	void opacityChanged(const float& opacity);

	/**
	 * Invoked when the brush radius changed
	 * @param brushRadius Brush radius
	 */
	void brushRadiusChanged(const float& brushRadius);

	/**
	 * Invoked when the brush radius delta changed
	 * @param brushRadiusDelta Brush radius delta
	 */
	void brushRadiusDeltaChanged(const float& brushRadiusDelta);

protected:
	QSize		_size;					/** Size of the quad */
	QColor		_color;					/** Selection color */
	float		_brushRadius;			/** Brush radius */
	float		_brushRadiusDelta;		/** Selection brush size increase/decrease delta */
};