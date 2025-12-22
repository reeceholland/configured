#include "ui/HomeScreenWidget.hpp"

#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <QSize>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

#include "ui/BottomTextButton.hpp"

HomeScreenWidget::HomeScreenWidget(QWidget* parent)
{
	auto* outer = new QVBoxLayout(this);
	outer->setContentsMargins(80, 80, 80, 80);
	outer->setSpacing(24);
	
	auto* title = new QLabel("CONFIGURED", this);
	QFont titleFont("Consolas");
	titleFont.setPointSize(36);
	titleFont.setBold(true);
	title->setFont(titleFont);
	title->setAlignment(Qt::AlignCenter);
	outer->addWidget(title);
	outer->addSpacing(24);

	auto* connectBtn = new BottomTextButton("CONNECT", this);
	QIcon connectIcon(":/resources/icons/usb_connection_96.png");
	connectBtn->setIcon(connectIcon);

	auto* createBtn = new BottomTextButton("CREATE", this);
	QIcon createIcon(":/resources/icons/add_plus_circle_128.png");
	createBtn->setIcon(createIcon);
	
	auto* settingsBtn = new BottomTextButton("SETTINGS", this);
	QIcon settingsIcon(":/resources/icons/settings_cog_128.png");
	settingsBtn->setIcon(settingsIcon);

	auto* helpBtn = new BottomTextButton("HELP", this);
	QIcon helpIcon(":/resources/icons/question_mark_circle_128.png");
	helpBtn->setIcon(helpIcon);

    for (auto* b : { connectBtn, createBtn, settingsBtn, helpBtn })
    {
        b->setProperty("cta", true);
        b->setMinimumHeight(130); // gives space for icon + bottom text
    }
    auto* grid = new QGridLayout();
    grid->setHorizontalSpacing(20);
    grid->setVerticalSpacing(20);

    grid->addWidget(connectBtn, 0, 0);
    grid->addWidget(createBtn, 0, 1);
    grid->addWidget(settingsBtn, 0, 2);
    grid->addWidget(helpBtn, 0, 3);

    for (int c = 0; c < 4; ++c) grid->setColumnStretch(c, 1);

    outer->addLayout(grid);
    outer->addStretch(1);

    setStyleSheet(R"(
      QPushButton[cta="true"] {
        background: #1f6feb;
        color: white;
        border: 1px solid #1b5fd1;
        border-radius: 14px;
        padding: 18px 22px;
        min-height: 110px;
        font-size: 18px;
        font-weight: 600;
      }
      QPushButton[cta="true"]:hover   { background: #2b78ff; }
      QPushButton[cta="true"]:pressed { background: #1a5fd1; }
    )");
	
	connect(connectBtn, &QPushButton::clicked, this, &HomeScreenWidget::connectClicked);
}

void HomeScreenWidget::connectClicked()
{
    if (!connectWindow_)
    {
        connectWindow_ = new ConnectWindow(nullptr);          // top-level window
        connectWindow_->setAttribute(Qt::WA_DeleteOnClose);   // deletes on close

        // when it deletes itself, clear our pointer
        connect(connectWindow_, &QObject::destroyed, this, [this]() {
            connectWindow_ = nullptr;
            });

        connectWindow_->show();
    }
    else
    {
        connectWindow_->show();
        connectWindow_->raise();
        connectWindow_->activateWindow();
    }
}



