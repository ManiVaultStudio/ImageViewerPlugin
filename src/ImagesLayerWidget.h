#pragma once

#include "ModelWidget.h"

#include <memory>

namespace Ui {
	class ImagesLayerWidget;
}

/** TODO */
class ImagesLayerWidget : public ModelWidget
{
public:
	/** TODO */
	ImagesLayerWidget(QWidget* parent);

	/** TODO */
	void initialize(LayersModel* layersModel);

protected: // Inherited

	/** TODO */
	void updateData(const QModelIndex& index);

	/** TODO */
	bool shouldUpdate(const QModelIndex& index) const;

private:
	std::unique_ptr<Ui::ImagesLayerWidget>	_ui;	/** TODO */
};