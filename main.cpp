#include <QApplication>
#include "ui/MainWindow.h"
#include "common/AppTheme.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setStyleSheet(AppTheme::globalStyle());

    MainWindow window;
    window.show();

    return app.exec();
}
