#include "include/ui/setting/Icon.hpp"

#include "include/global/Configs.hpp"

#include <QPainter>
#ifdef Q_OS_MACOS
#define DIR ":/systray/"
#else
#define DIR "public/"
#endif

QPixmap Icon::GetTrayIcon(TrayIconStatus status) {
    QPixmap pixmap;

    if (status == NONE)
    {
        auto pixmap_read = QPixmap(QString(DIR) + "Off" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == RUNNING)
    {
        auto pixmap_read = QPixmap(QString(DIR) + "On" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == SYSTEM_PROXY_DNS)
    {
        auto pixmap_read = QPixmap(QString(DIR) + "Proxy-Dns" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == SYSTEM_PROXY)
    {
        auto pixmap_read = QPixmap(QString(DIR) + "Proxy" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == DNS)
    {
        auto pixmap_read = QPixmap(QString(DIR) + "Dns" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == VPN)
    {
        auto pixmap_read = QPixmap(QString(DIR) + "Tun" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else
    {
        MW_show_log("Icon::GetTrayIcon: Unknown status");
        auto pixmap_read = QPixmap(QString(DIR) + "Off" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    }

    return pixmap;
}
