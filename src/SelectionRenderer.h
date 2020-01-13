#pragma once

#include "Common.h"

#include "QuadRenderer.h"

class SelectionRenderer : public QuadRenderer
{
public:
	SelectionRenderer(const std::uint32_t& zIndex);

public:
	void render() override;

protected:
	void initializePrograms();

public:
	void setImage(std::shared_ptr<QImage> image);
	void setOpacity(const float& opacity);

protected:
	bool initialized() const;
	
private:
	std::unique_ptr<QOpenGLTexture>		_texture;
	QVector4D							_selectionColor;
};