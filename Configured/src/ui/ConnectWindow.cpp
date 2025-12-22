#include "ConnectWindow.hpp"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

ConnectWindow::ConnectWindow(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle("Connect Source");
    resize(520, 320);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    title_ = new QLabel("Connect Source", this);
    QFont f = title_->font();
    f.setPointSize(f.pointSize() + 4);
    f.setBold(true);
    title_->setFont(f);

    layout->addWidget(title_);

	auto* grid = new QGridLayout();
	grid->setHorizontalSpacing(8);
	grid->setVerticalSpacing(8);

	auto* localFileButton = new QPushButton("Local File", this);
	auto* usbDeviceButton = new QPushButton("USB Device", this);
	auto* networkLocationButton = new QPushButton("Network Location", this);
	auto* gitRepository = new QPushButton("Git Repository", this);
	
    for (auto* b : { localFileButton, usbDeviceButton, networkLocationButton, gitRepository })
    {
        b->setMinimumHeight(40);
        
	}

	grid->addWidget(localFileButton, 0, 0);
	grid->addWidget(usbDeviceButton, 0, 1);
	grid->addWidget(networkLocationButton, 1, 0);
	grid->addWidget(gitRepository, 1, 1);

	grid->setColumnStretch(0, 1);
	grid->setColumnStretch(1, 1);

	layout->addLayout(grid);
	layout->addStretch(1);


    closeBtn_ = new QPushButton("Close", this);
    layout->addStretch(1);
    layout->addWidget(closeBtn_);

    connect(localFileButton, &QPushButton::clicked,
        this, &ConnectWindow::onLocalConfiguredFileClicked);

    connect(closeBtn_, &QPushButton::clicked, this, &QWidget::close);
}

void ConnectWindow::onLocalConfiguredFileClicked()
{
    const QString file = QFileDialog::getOpenFileName(
        this,
        tr("Select a .configured file"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Configured files (*.configured)")
    );

    if (file.isEmpty())
        return;

    QFileInfo fi(file);
    if (fi.suffix().compare("configured", Qt::CaseInsensitive) != 0)
        return;


}
