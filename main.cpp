#include <QApplication>
#include "ui/MainWindow.h"
#include "common/AppTheme.h"
#include "common/TouchScroll.h" 
#include "common/AudioFeedback.h" 

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    installTouchScroll(app);
    app.setStyle("Fusion");
    app.setStyleSheet(AppTheme::globalStyle());
    AudioFeedback::init(); 
    MainWindow window;
    window.show();

    return app.exec();
}
