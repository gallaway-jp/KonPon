#include "SettingsUtil.h"

#include <QDir>
#include <QDirIterator>
#include <QString>

void MoveFolderContents(const QString& fromDir, const QString& toDir) {
    QDirIterator it(fromDir, QDirIterator::Subdirectories);
    QDir dir(fromDir);
    const int absSourcePathLength = dir.absoluteFilePath(fromDir).length();

    while (it.hasNext()) {
        it.next();
        const auto fileInfo = it.fileInfo();
        if (!fileInfo.isHidden()) {
            const QString subPathStructure = fileInfo.absoluteFilePath().mid(absSourcePathLength);
            const QString constructedAbsolutePath = toDir + subPathStructure;

            if (fileInfo.isDir()) {
                dir.mkpath(constructedAbsolutePath);
            }
            else if (fileInfo.isFile()) {
                QFile::remove(constructedAbsolutePath);
                QFile::copy(fileInfo.absoluteFilePath(), constructedAbsolutePath);
            }
        }
    }
    dir.removeRecursively();
}