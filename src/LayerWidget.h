#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
    class LayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/**
 * Layer widget class
 *
 * This widget class provides the user interface for editing layer common properties
 *
 * @author Thomas Kroes
 */
class LayerWidget : public QWidget
{
public: // Construction

    /**
     * Constructor
     * @param parent Parent widget
     */
    LayerWidget(QWidget* parent);

public: // Initialization and update

    /**
     * Initializes the widget
     * @param imageViewerPlugin Pointer to the image viewer plugin
     */
    void initialize(ImageViewerPlugin* imageViewerPlugin);

    /**
     * Updates the UI with model indices ranging from \p begin to \p end
     * @param begin Start of model index range
     * @param end End of model index range
     * @param roles Data roles
     */
    void updateData(const QModelIndex& begin, const QModelIndex& end, const QVector<int>& roles = QVector<int>());

private:
    ImageViewerPlugin*                  _imageViewerPlugin;     /** Pointer to the image viewer plugin */
    std::unique_ptr<Ui::LayerWidget>    _ui;                    /** User interface as produced by Qt designer */
    LayersModel*                        _layersModel;           /** Pointer to the layers model */
};