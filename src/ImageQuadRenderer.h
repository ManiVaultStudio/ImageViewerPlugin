#pragma once

#include "Common.h"

#include "QuadRenderer.h"

class ImageQuadRenderer : public QuadRenderer
{
	Q_OBJECT

public:
	ImageQuadRenderer(const float& depth);

public:
	void init() override;
	void render() override;

	bool isInitialized() const;

public:
	void setImage(std::shared_ptr<QImage> image);
	std::uint16_t imageMin() const;
	std::uint16_t imageMax() const;
	void setImageMinMax(const std::uint16_t& imageMin, const std::uint16_t& imageMax);
	float window() const;
	float level() const;
	void setWindowLevel(const float& window, const float& level);
	void resetWindowLevel();

protected:
	void createShaderPrograms();
	void createTextures();

signals:
	void imageMinMaxChanged(const std::uint16_t& imageMin, const std::uint16_t& imageMax);
	void windowLevelChanged(const float& window, const float& level);

private:
	std::uint16_t	_imageMin;
	std::uint16_t	_imageMax;
	float			_window;
	float			_level;
};