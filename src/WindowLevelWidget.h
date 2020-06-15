#pragma once

#include <memory>

#include <QDialog>
#include <QModelIndex>

class ImageViewerPlugin;

namespace Ui {
	class WindowLevelWidget;
}

/**
 * Window/level settings popup widget class
 *
 * @author Thomas Kroes
 */
class WindowLevelWidget : public QWidget
{
public: // Construction/destruction

	/**
	 * Constructor
	 * @param parent Parent widget
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 * @param windowIndex Window model index
	 * @param levelIndex Level model index
	 */
	WindowLevelWidget(QWidget* parent, ImageViewerPlugin* imageViewerPlugin, const QModelIndex& windowIndex, const QModelIndex& levelIndex);

	/** Destructor */
	~WindowLevelWidget();

protected: // Miscellaneous

	/**
	 * Update user interface from a range of indices
	 * @param topLeft Top-left model index
	 * @param bottomRight Bottom-right model index
	 * @param roles Data roles
	 */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::WindowLevelWidget>	_ui;					/** User interface as produced by Qt designer */
	QModelIndex								_windowIndex;			/** Model index of the window setting */
	QModelIndex								_levelIndex;			/** Model index of the level setting */
};