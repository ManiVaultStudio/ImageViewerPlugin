#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"

#include "PointsPlugin.h"

#include <vector>

#include <QSize>
#include <QDebug>

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageViewerPlugin(imageViewerPlugin),
	_texture(QOpenGLTexture::Target2D)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageIdsChanged, this, &ImageViewerWidget::onDisplayImageIdsChanged);
}

void ImageViewerWidget::onDisplayImageIdsChanged()
{
	const auto imageSize		= _imageViewerPlugin->imageSize();
	const auto noPixels			= imageSize.width() * imageSize.height();
	const auto type				= _imageViewerPlugin->imageCollectionType();
	const auto displayImageIds	= _imageViewerPlugin->displayImageIds();
	const auto noDisplayImages	= displayImageIds.size();
	const auto noImages			= _imageViewerPlugin->noImages();
	const auto width			= imageSize.width();
	const auto height			= imageSize.height();
	
	if (QSize(_texture.width(), _texture.height()) != imageSize) {
		setupTexture(imageSize);
	}

	PointsPlugin& pointsData = _imageViewerPlugin->pointsData();
	
	std::vector<unsigned char> image;

	image.resize(noPixels * 3);

	if (type == "SEQUENCE") {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;
				
				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImageIds) {
					const auto imageOffset	= displayImageId * noPixels;
					const auto pointId		= imageOffset + pixelId;

					pixelValue += pointsData.data[pointId];
				}
				
				pixelValue /= static_cast<float>(noDisplayImages);

				image[pixelId * 3 + 0] = pixelValue;
				image[pixelId * 3 + 1] = pixelValue;
				image[pixelId * 3 + 2] = pixelValue;
			}
		}
	}

	if (type == "STACK") {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;

				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImageIds) {
					const auto pointId = (pixelId * noImages) + displayImageId;

					pixelValue += pointsData.data[pointId];
				}

				pixelValue /= static_cast<float>(noDisplayImages);

				image[pixelId * 3 + 0] = pixelValue;
				image[pixelId * 3 + 1] = pixelValue;
				image[pixelId * 3 + 2] = pixelValue;
			}
		}
	}

	_texture.setData(QOpenGLTexture::PixelFormat::RGB, QOpenGLTexture::PixelType::UInt8, static_cast<void*>(&image[0]));

	update();

	//qDebug() << _texture.isCreated();
	//qDebug() << _texture.isStorageAllocated();
}

void ImageViewerWidget::setupTexture(const QSize& imageSize)
{
	if (imageSize.width() == 0 || imageSize.height() == 0)
		return;
	
	qDebug() << "Setup texture: " << QString("%1x%2").arg(QString::number(imageSize.width()), QString::number(imageSize.height()));

	_texture.destroy();
	_texture.create();
	_texture.setSize(imageSize.width(), imageSize.height(), 1);
	_texture.setFormat(QOpenGLTexture::RGBA8_UNorm);
	_texture.allocateStorage();
}

void ImageViewerWidget::initializeGL()
{
	initializeOpenGLFunctions();
}

void ImageViewerWidget::resizeGL(int w, int h)
{
	qDebug() << "Resizing image viewer";
}

void ImageViewerWidget::paintGL()
{
	if (!_texture.isCreated())
		return;


	const auto size = 2;



	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-size, size, -size, size, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();


	glLoadIdentity();
	glDisable(GL_LIGHTING);


	//glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);

	_texture.bind();


	// Draw a textured quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
	glTexCoord2f(0, 1); glVertex3f(0, 1, 0);
	glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
	glTexCoord2f(1, 0); glVertex3f(1, 0, 0);
	glEnd();


	glDisable(GL_TEXTURE_2D);
	glPopMatrix();


	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

void ImageViewerWidget::mousePressEvent(QMouseEvent *event)
{
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
}