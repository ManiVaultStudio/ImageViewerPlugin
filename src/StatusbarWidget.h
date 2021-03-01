#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
    class StatusbarWidget;
}

class ImageViewerPlugin;

/**
 * TODO: Add description
 *
 * @author Thomas Kroes
 */
class StatusbarWidget : public QWidget
{
public: // Construction

    /**
     * Constructor
     * @param parent Parent widget
     */
    StatusbarWidget(QWidget* parent);

private:
    std::unique_ptr<Ui::StatusbarWidget> _ui;       /** User interface as produced by Qt designer */
};