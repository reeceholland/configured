#pragma once

#include <QPushButton>

class BottomTextButton : public QPushButton
{
    Q_OBJECT
public:
    explicit BottomTextButton(const QString& text, QWidget* parent = nullptr);

    void setBottomTextPadding(int px);   // space from bottom edge
    void setIconTopPadding(int px);      // space from top edge
    void setIconSizePx(int px);          // icon size (square)

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    int bottom_text_padding_ = 14;
    int icon_top_padding_ = 18;
    int icon_size_px_ = 44;
};
