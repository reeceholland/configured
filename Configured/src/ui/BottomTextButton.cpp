#include "BottomTextButton.hpp"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

BottomTextButton::BottomTextButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
{
}

void BottomTextButton::setBottomTextPadding(int px) { bottom_text_padding_ = px; update(); }
void BottomTextButton::setIconTopPadding(int px) { icon_top_padding_ = px; update(); }
void BottomTextButton::setIconSizePx(int px) { icon_size_px_ = px; update(); }

void BottomTextButton::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);

    QStyleOptionButton opt;
    initStyleOption(&opt);

    // Let Qt draw the button background/border/hover/pressed, but NOT the label.
    const QString originalText = opt.text;
    const QIcon originalIcon = opt.icon;
    opt.text.clear();
    opt.icon = QIcon();

    QPainter p(this);
    style()->drawControl(QStyle::CE_PushButton, &opt, &p, this);

    // Draw icon top-center (optional)
    if (!originalIcon.isNull())
    {
        const int s = icon_size_px_;
        QRect iconRect((width() - s) / 2, icon_top_padding_, s, s);

        QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
        if (isDown()) mode = QIcon::Active;

        originalIcon.paint(&p, iconRect, Qt::AlignCenter, mode);
    }

    // Draw text bottom-center
    QRect textRect(8, 0, width() - 16, height() - bottom_text_padding_);
    p.setFont(font());
    p.setPen(palette().buttonText().color());
    p.drawText(textRect, Qt::AlignHCenter | Qt::AlignBottom, originalText);
}
