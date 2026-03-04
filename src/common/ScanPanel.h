#pragma once
#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>

class ScanPanel : public QWidget {
    Q_OBJECT
public:
    explicit ScanPanel(const QString& title, QWidget* parent = nullptr);

    void startScanning();
    void stopScanning();
    void addDevice(const QString& id, const QString& primary, const QString& secondary);
    void clearDevices();
    void showConnecting(const QString& msg = "Connecting…");
    void showConnected(const QString& name, bool allowDisconnect = true);
    void showDisconnected();
    void setContentVisible(bool visible);
    void setEmptyMessage(const QString& msg);

signals:
    void refreshClicked();
    void connectClicked(const QString& id);
    void disconnectClicked();

private:
    void buildUI(const QString& title);
    void tickDots();

    QPushButton* m_refreshBtn;
    QWidget*     m_devicesContainer;
    QVBoxLayout* m_devicesLayout;
    QScrollArea* m_devicesScroll;
    QWidget*     m_scanLoader;
    QWidget*     m_connectLoader;
    QWidget*     m_emptyState;
    QLabel*      m_emptyText;

    QTimer* m_dotTimer;
    int     m_dotCount = 0;
    QMap<QString, QWidget*> m_deviceWidgets;
};