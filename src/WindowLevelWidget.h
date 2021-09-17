//#pragma once
//
//#include <memory>
//
//#include <QDialog>
//#include <QModelIndex>
//
//class ImageViewerPlugin;
//
//namespace Ui {
//    class WindowLevelWidget;
//}
//
///**
// * Window/level settings popup widget class
// *
// * @author Thomas Kroes
// */
//class WindowLevelWidget : public QWidget
//{
//public: // Construction/destruction
//
//    /**
//     * Constructor
//     * @param parent Parent widget
//     * @param imageViewerPlugin Pointer to the image viewer plugin
//     * @param windowIndex Window model index
//     * @param levelIndex Level model index
//     */
//    WindowLevelWidget(QWidget* parent, ImageViewerPlugin* imageViewerPlugin, const QModelIndex& windowIndex, const QModelIndex& levelIndex);
//
//    /** Destructor */
//    ~WindowLevelWidget();
//
//protected: // Miscellaneous
//
//    /**
//     * Updates the UI with model indices ranging from \p begin to \p end
//     * @param begin Start of model index range
//     * @param end End of model index range
//     * @param roles Data roles
//     */
//    void updateData(const QModelIndex& begin, const QModelIndex& end, const QVector<int>& roles = QVector<int>());
//
//private:
//    ImageViewerPlugin*                      _imageViewerPlugin;     /** Pointer to the image viewer plugin */
//    std::unique_ptr<Ui::WindowLevelWidget>  _ui;                    /** User interface as produced by Qt designer */
//    QModelIndex                             _windowIndex;           /** Model index of the window setting */
//    QModelIndex                             _levelIndex;            /** Model index of the level setting */
//};