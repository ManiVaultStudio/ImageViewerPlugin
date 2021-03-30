#pragma once

#include <memory>

#include <QStackedWidget>

namespace Ui {
    class LayerWidget;
}

class ImageViewerPlugin;
class LayersModel;
class PointsLayerWidget;
class SelectionLayerWidget;

/**
 * Layer widget class
 *
 * This widget class provides the user interface for editing layer common properties
 *
 * @author Thomas Kroes
 */
class LayerWidget : public QWidget
{
public:
    class StackedWidget : public QStackedWidget {
    public:
        QSize sizeHint() const override { return currentWidget()->sizeHint(); }
        QSize minimumSizeHint() const override { return currentWidget()->minimumSizeHint(); }
    };

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
    ImageViewerPlugin*                  _imageViewerPlugin;         /** Pointer to the image viewer plugin */
    std::unique_ptr<Ui::LayerWidget>    _ui;                        /** User interface as produced by Qt designer */
    LayersModel*                        _layersModel;               /** Pointer to the layers model */
    StackedWidget*                      _stackedWidget;             /** TODO: only a temporary solution */
    PointsLayerWidget*                  _pointsLayerWidget;         /** TODO: only a temporary solution */
    SelectionLayerWidget*               _selectionLayerWidget;      /** TODO: only a temporary solution */
};