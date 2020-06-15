#pragma once

#include <memory>

#include <QDialog>
#include <QModelIndex>

class ImageViewerPlugin;

namespace Ui {
	class WindowLevelWidget;
}

/**
 * TODO
 *
 * @author Thomas Kroes
 */
class WindowLevelWidget : public QWidget
{
public: // Construction/destruction

	/**
	 * Constructor
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	WindowLevelWidget(QWidget* parent, ImageViewerPlugin* imageViewerPlugin, const QModelIndex& windowIndex, const QModelIndex& levelIndex);

	/** Destructor */
	~WindowLevelWidget();

	/**
	 * Update from a range of indices
	 */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::WindowLevelWidget>	_ui;					/** User interface as produced by Qt designer */
	QModelIndex								_windowIndex;			/** Model index of the window setting */
	QModelIndex								_levelIndex;			/** Model index of the level setting */
};