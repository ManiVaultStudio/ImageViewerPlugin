#pragma once

#include "Common.h"

#include "QuadRenderer.h"

class SelectionRenderer : public QuadRenderer
{
public:
	SelectionRenderer();

public:
	void render() override;

	void initializeProgram();

public:
	void setImage(std::shared_ptr<QImage> image);
	void setOpacity(const float& opacity);

private:
	QVector4D	_selectionColor;
};