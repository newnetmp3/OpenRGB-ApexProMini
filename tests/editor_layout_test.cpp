#include "ApexMiniKeyboardEditor.h"
#include <QApplication>
#include <QScrollArea>
#include <QScrollBar>
#include <cassert>
#include <vector>

int main(int argc, char** argv)
{
    // Offscreen allows CI/build hosts to inspect Qt layout without a display.
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc,argv);
    apex_mini::KeyboardEditor editor(
        [](const std::vector<std::uint8_t>&,const QColor&) { return true; },
        [](std::uint8_t) { return QColor(24,42,72); },
        [](const std::vector<apex_mini::KeyColor>&) { return true; });
    auto* page=editor.findChild<QScrollArea*>(QStringLiteral("ApexSettingsScroll"));
    auto* keyboard=editor.findChild<QScrollArea*>(QStringLiteral("ApexKeyboardScroll"));
    assert(page && keyboard);
    assert(page->widgetResizable());
    assert(page->horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff);
    assert(keyboard->horizontalScrollBarPolicy() == Qt::ScrollBarAsNeeded);
    assert(keyboard->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff);

    for(const auto& size : {QSize(560,440),QSize(730,540),QSize(1080,760),QSize(510,390)})
    {
        editor.resize(size);
        editor.show();
        app.processEvents();
        assert(editor.width()==size.width());
        assert(page->viewport()->width()>0);
        assert(keyboard->viewport()->width()>0);
        assert(keyboard->widget()->width()>keyboard->viewport()->width()
               || size.width()>=1080);
        assert(page->verticalScrollBar()->maximum()>0 || size.height()>=760);
    }
    editor.StopActivity();
}
