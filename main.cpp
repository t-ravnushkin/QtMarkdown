#include <QApplication>

#include "editorwindow.h"

#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSettings settings("trollbisto", "markdown");
    EditorWindow* nw = new EditorWindow();
    if (settings.contains("filepath"))
        nw->openFile(settings.value("filepath").toString());
    nw->show();
    return a.exec();
}
