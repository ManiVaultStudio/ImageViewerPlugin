#pragma once

#include "Common.h"

#include "QuadRenderer.h"

class SelectionRenderer : public QuadRenderer
{
public:
	SelectionRenderer(const std::uint32_t& zIndex);

public:
	void render() override;

public:
	void setImage(std::shared_ptr<QImage> image);
	void setOpacity(const float& opacity);

protected:
	bool initialized() override;
	void initializeShaderPrograms() override;
	void initializeTextures() override;
	
private:
	QVector4D	_selectionColor;
};