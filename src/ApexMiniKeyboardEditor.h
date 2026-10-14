#pragma once

#include "apex_pro_mini_protocol.h"
#include "apex_pro_mini_layout.h"
#include <QColorDialog>
#include <QFont>
#include <QSizePolicy>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>
#include <initializer_list>
#include <utility>

namespace apex_mini
{
// Widths are derived from the owner's ANSI 60%-keyboard photograph.
// This is a separate keycap-shaped editor, not the uniform OpenRGB LED grid.
struct VisualKey { std::uint8_t hid; const char* label; double units; };

class KeyboardEditor : public QWidget
{
public:
    using ApplyColor = std::function<bool(std::uint8_t, const QColor&)>;
    explicit KeyboardEditor(ApplyColor apply, QWidget* parent = nullptr)
        : QWidget(parent), apply_(std::move(apply))
    {
        auto* main = new QVBoxLayout(this);
        main->setSpacing(9);
        auto* heading = new QLabel(QStringLiteral("Apex Pro Mini — Key Lighting"));
        QFont title = heading->font();
        title.setBold(true);
        title.setPointSize(title.pointSize() + 2);
        heading->setFont(title);
        main->addWidget(heading);

        auto* note = new QLabel(QStringLiteral(
            "Choose a key, then set its color. Changes apply in Direct mode. "
            "Keycap dimensions follow the photographed ANSI layout; the SteelSeries key HID is provisional."));
        note->setWordWrap(true);
        main->addWidget(note);

        auto add_row = [this, main](std::initializer_list<VisualKey> keys)
        {
            auto* line = new QHBoxLayout();
            line->setSpacing(4);
            for(const auto& key : keys)
            {
                auto* button = new QPushButton(QString::fromUtf8(key.label));
                button->setToolTip(QStringLiteral("HID %1").arg(key.hid));
                button->setMinimumHeight(43);
                const int width = static_cast<int>(45.0 * key.units);
                button->setMinimumWidth(width);
                button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                line->addWidget(button, static_cast<int>(key.units * 100));
                connect(button, &QPushButton::clicked, this, [this, button, key]()
                {
                    if(selected_button_)
                        selected_button_->setStyleSheet(QString());
                    selected_button_ = button;
                    selected_ = key.hid;
                    button->setStyleSheet(QStringLiteral(
                        "QPushButton { border: 2px solid #35baff; background-color: #23475d; }"));
                    selection_->setText(QStringLiteral("Selected: %1 (HID %2)")
                        .arg(QString::fromUtf8(key.label)).arg(key.hid));
                });
            }
            main->addLayout(line);
        };

        add_row({
            {41,"Esc",1},{30,"1",1},{31,"2",1},{32,"3",1},{33,"4",1},
            {34,"5",1},{35,"6",1},{36,"7",1},{37,"8",1},{38,"9",1},
            {39,"0",1},{45,"-",1},{46,"=",1},{42,"Backspace",2}
        });
        add_row({
            {43,"Tab",1.5},{20,"Q",1},{26,"W",1},{8,"E",1},{21,"R",1},
            {23,"T",1},{28,"Y",1},{24,"U",1},{12,"I",1},{18,"O",1},
            {19,"P",1},{47,"[",1},{48,"]",1},{49,"\\",1.5}
        });
        add_row({
            {57,"Caps",1.75},{4,"A",1},{22,"S",1},{7,"D",1},{9,"F",1},
            {10,"G",1},{11,"H",1},{13,"J",1},{14,"K",1},{15,"L",1},
            {51,";",1},{52,"'",1},{40,"Enter",2.25}
        });
        add_row({
            {225,"Shift",2.25},{29,"Z",1},{27,"X",1},{6,"C",1},{25,"V",1},
            {5,"B",1},{17,"N",1},{16,"M",1},{54,",",1},{55,".",1},
            {56,"/",1},{229,"Shift",2.75}
        });
        add_row({
            {224,"Ctrl",1.25},{227,"Win",1.25},{226,"Alt",1.25},
            {44,"Space",6.25},{230,"Alt",1.25},{231,"Win",1.25},
            {240,"SteelSeries",1.25},{228,"Ctrl",1.25}
        });

        auto* tools = new QHBoxLayout();
        selection_ = new QLabel(QStringLiteral("Selected: none"));
        tools->addWidget(selection_, 1);
        auto* choose = new QPushButton(QStringLiteral("Set selected key color…"));
        tools->addWidget(choose);
        main->addLayout(tools);
        auto* footer = new QLabel(QStringLiteral(
            "The seven regional/firmware-only HID addresses remain accessible "
            "through OpenRGB's standard LED list."));
        footer->setWordWrap(true);
        main->addWidget(footer);
        connect(choose, &QPushButton::clicked, this, [this]()
        {
            if(!selected_button_) return;
            const QColor color = QColorDialog::getColor(Qt::blue, this, QStringLiteral("Key color"));
            if(color.isValid() && apply_)
            {
                const bool applied = apply_(selected_, color);
                selection_->setText(applied
                    ? QStringLiteral("Applied color to HID %1").arg(selected_)
                    : QStringLiteral("Cannot apply: select Direct mode and verify device connection"));
            }
        });
        setMinimumWidth(820);
    }

private:
    ApplyColor apply_;
    QLabel* selection_ = nullptr;
    QPushButton* selected_button_ = nullptr;
    std::uint8_t selected_ = 0;
};
} // namespace apex_mini
