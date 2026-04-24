#include "utils/fileloader.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QFileDialog>
#include <QApplication>
#include <QStringConverter>

FileResult FileLoader::load(const QString& filePath)
{
    FileResult result;

    if (filePath.isEmpty()) {
        result.errorMessage = "No file path provided.";
        return result;
    }

    QFileInfo info(filePath);
    if (!info.exists()) {
        result.errorMessage = QString("File not found: %1").arg(filePath);
        return result;
    }
    if (!info.isReadable()) {
        result.errorMessage = QString("File is not readable: %1").arg(filePath);
        return result;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.errorMessage = QString("Cannot open file: %1\n%2")
        .arg(filePath)
            .arg(file.errorString());
        return result;
    }

    QTextStream stream(&file);
    stream.setAutoDetectUnicode(true);

    QString line;
    while (stream.readLineInto(&line)) {
        result.lines.append(line);
    }

    file.close();

    result.fullText = result.lines.join('\n');

    switch (stream.encoding()) {
    case QStringConverter::Utf8:    result.encoding = "UTF-8";     break;
    case QStringConverter::Utf16:   result.encoding = "UTF-16";    break;
    case QStringConverter::Utf16LE: result.encoding = "UTF-16 LE"; break;
    case QStringConverter::Utf16BE: result.encoding = "UTF-16 BE"; break;
    case QStringConverter::Utf32:   result.encoding = "UTF-32";    break;
    case QStringConverter::Latin1:  result.encoding = "Latin-1";   break;
    default:                        result.encoding = "UTF-8";     break;
    }

    result.success  = true;
    result.filePath = info.absoluteFilePath();
    result.fileName = info.fileName();
    result.fileSize = info.size();

    return result;
}

FileResult FileLoader::openWithDialog(QWidget* parent,
                                      const QString& caption,
                                      const QString& lastDirectory)
{
    const QString filters =
        "All Files (*);;"
        "Text Files (*.txt *.md *.rst *.log);;"
        "Source Code (*.cpp *.c *.h *.hpp *.cc *.cxx);;"
        "Web Files (*.html *.htm *.css *.js *.ts *.json *.xml);;"
        "Config Files (*.ini *.cfg *.conf *.yaml *.yml *.toml);;"
        "Scripts (*.py *.sh *.bash *.bat *.ps1)";

    const QString path = QFileDialog::getOpenFileName(
        parent,
        caption,
        lastDirectory,
        filters
        );

    if (path.isEmpty()) {
        FileResult cancelled;
        cancelled.success = false;
        return cancelled;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    FileResult result = load(path);
    QApplication::restoreOverrideCursor();

    return result;
}

QString FileLoader::formatSize(qint64 bytes)
{
    if (bytes < 1024)
        return QString("%1 B").arg(bytes);
    if (bytes < 1024 * 1024)
        return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    if (bytes < 1024 * 1024 * 1024)
        return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1);
    return QString("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 1);
}
