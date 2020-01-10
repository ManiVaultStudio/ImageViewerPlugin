#pragma once

#include "Common.h"

#include "QuadRenderer.h"

class ImageQuadRenderer : public QuadRenderer
{
public:
	ImageQuadRenderer(const std::uint32_t& zIndex);

public:
	void render() override;

	void initializePrograms();

public:
	void setImage(std::shared_ptr<QImage> image);
	float window() const;
	float level() const;
	void setWindowLevel(const float& window, const float& level);
	void resetWindowLevel();

protected:
	bool initialized() const;

private:
	std::unique_ptr<QOpenGLTexture>		_texture;
	std::uint16_t						_imageMin;
	std::uint16_t						_imageMax;
	float								_window;
	float								_level;
};