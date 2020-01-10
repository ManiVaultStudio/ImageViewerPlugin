#pragma once

#include "Common.h"

#include "renderers/Renderer.h"

#include <QMatrix4x4>

class StackedRenderer : public hdps::Renderer
{
public:
	StackedRenderer(const std::uint32_t& zIndex);

public:
	void setModelViewProjection(const QMatrix4x4& modelViewProjection);

protected:
	virtual bool initialized() const = 0;

protected:
	std::uint32_t	_zIndex;
	QMatrix4x4		_modelViewProjection;
};