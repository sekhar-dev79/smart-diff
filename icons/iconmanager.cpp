#include "iconmanager.h"
#include "thememanager.h"

#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <QFile>

IconManager* IconManager::s_instance = nullptr;

IconManager* IconManager::instance()
{
    if (!s_instance)
        s_instance = new IconManager();
    return s_instance;
}

QIcon IconManager::icon(const QString& name,
                        const QColor&  color,
                        const QSize&   size) const
{
    const QString key = QString("%1|%2|%3x%4")
    .arg(name)
        .arg(color.name())
        .arg(size.width())
        .arg(size.height());

    if (m_cache.contains(key))
        return m_cache[key];

    const QString path = QString(":/icons/%1").arg(name);
    const QIcon   ico  = renderSvg(path, color, size);
    m_cache.insert(key, ico);
    return ico;
}

QIcon IconManager::icon(const QString& name, const QSize& size) const
{
    return icon(name, ThemeManager::instance()->colors().iconColor, size);
}

QIcon IconManager::mutedIcon(const QString& name, const QSize& size) const
{
    return icon(name, ThemeManager::instance()->colors().iconMuted, size);
}

void IconManager::clearCache()
{
    m_cache.clear();
}

QIcon IconManager::renderSvg(const QString& resourcePath,
                             const QColor&  color,
                             const QSize&   size) const
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QIcon{};
    }

    QByteArray svgData = file.readAll();
    svgData.replace("currentColor", color.name(QColor::HexRgb).toUtf8());

    QSvgRenderer renderer(svgData);
    if (!renderer.isValid()) {
        return QIcon{};
    }

    const QSize hiDpi = size * 2;
    QPixmap pixmap(hiDpi);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    renderer.render(&painter);
    painter.end();

    pixmap.setDevicePixelRatio(2.0);
    return QIcon(pixmap);
}
