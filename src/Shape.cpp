#include "Shape.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QDebug>

Shape::Shape(const QString& name) :
	_name(name),
	_initialized(false),
	_shaderPrograms(),
	_vaos(),
	_vbos(),
	_textures()
{
}

Shape::~Shape() = default;

void Shape::destroy()
{
	qDebug() << "Destroying" << _name;

	for (auto key : _vaos.keys())
	{
		_vaos[key]->destroy();
	}

	for (auto key :_vbos.keys())
	{
		_vbos[key]->destroy();
	}

	for (auto key : _vaos.keys())
	{
		_textures[key]->destroy();
	}
}

void Shape::initialize()
{
	qDebug() << "Initializing" << _name;

	addShaderPrograms();
	addVAOs();
	addVBOs();
	addTextures();
}

bool Shape::isInitialized() const
{
	return _initialized;
}

void Shape::render()
{
	qDebug() << "Rendering" << _name << "shape";
}

void Shape::addShaderProgram(const QString& name, QSharedPointer<QOpenGLShaderProgram> shaderProgram)
{
	_shaderPrograms.insert(name, shaderProgram);
}

void Shape::addVAO(const QString& name, QSharedPointer<QOpenGLVertexArrayObject> vao)
{
	_vaos.insert(name, vao);
}

void Shape::addVBO(const QString& name, QSharedPointer<QOpenGLBuffer> vbo)
{
	_vbos.insert(name, vbo);
}

void Shape::addTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture)
{
	_textures.insert(name, texture);
}

QSharedPointer<QOpenGLShaderProgram> Shape::shaderProgram(const QString& name)
{
	return _shaderPrograms[name];
}

QSharedPointer<QOpenGLVertexArrayObject> Shape::vao(const QString& name)
{
	return _vaos[name];
}

QSharedPointer<QOpenGLBuffer> Shape::vbo(const QString& name)
{
	return _vbos[name];
}

QSharedPointer<QOpenGLTexture> Shape::texture(const QString& name)
{
	return _textures[name];
}