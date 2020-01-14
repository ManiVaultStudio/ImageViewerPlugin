#pragma once

#include "Common.h"

#include "QuadRenderer.h"

class ImageQuadRenderer : public QuadRenderer
{
public:
	ImageQuadRenderer(const std::uint32_t& zIndex);

public:
	void init() override;
	void render() override;

public:
	void setImage(std::shared_ptr<QImage> image);
	float window() const;
	float level() const;
	void setWindowLevel(const float& window, const float& level);
	void resetWindowLevel();

protected:
	bool initialized();
	void initializeShaderPrograms();
	void initializeTextures();

private:
	std::uint16_t						_imageMin;
	std::uint16_t						_imageMax;
	float								_window;
	float								_level;
	static std::uint32_t				_vertexAttribute;
	static std::uint32_t				_textureCoordinateAttribute;
};