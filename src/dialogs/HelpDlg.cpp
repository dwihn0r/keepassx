
#include "HelpDlg.h"

HelpDlg::HelpDlg(QWidget* parent) : QDialog(parent) {
	setupUi(this);
	textBrowser->setSource(QUrl("qrc:/docs/index.html"));
	buttonPrevious->setIcon(getIcon("go-previous"));
	buttonNext->setIcon(getIcon("go-next"));
	buttonFirst->setIcon(getIcon("go-home"));
	showMaximized();
}
