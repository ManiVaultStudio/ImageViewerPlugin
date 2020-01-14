#pragma once

#include "Common.h"

#include "renderers/Renderer.h"

#include <QMatrix4x4>
#include <QMap>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class StackedRenderer : public hdps::Renderer
{
public:
	StackedRenderer(const std::uint32_t& zIndex);

public:
	void init() override;

	void setModelViewProjection(const QMatrix4x4& modelViewProjection);

protected:
	virtual bool initialized() = 0;
	virtual void initializeShaderPrograms() = 0;
	virtual void initializeTextures() = 0;

	std::shared_ptr<QOpenGLShaderProgram>& shaderProgram(const QString& name);
	std::shared_ptr<QOpenGLTexture>& texture(const QString& name);

protected:
	std::uint32_t											_zIndex;
	QMatrix4x4												_modelViewProjection;
	QMap<QString, std::shared_ptr<QOpenGLShaderProgram>>	_shaderPrograms;
	QMap<QString, std::shared_ptr<QOpenGLTexture>>			_textures;
};