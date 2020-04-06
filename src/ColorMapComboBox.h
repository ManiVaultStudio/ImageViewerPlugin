#pragma once

#include <QComboBox>

class QListView;

class ColorMapModel;

/** TODO */
class ColorMapComboBox : public QComboBox
{
public:
	/** TODO */
	ColorMapComboBox(QWidget* parent);

	/** TODO */
	void setModel(ColorMapModel* colorMapModel);

private:
	ColorMapModel*	_colorMapModel;		/** TODO */
	QListView*		_view;				/** TODO */
};