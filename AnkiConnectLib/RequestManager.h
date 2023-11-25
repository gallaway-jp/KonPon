#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include "ankiconnectlib_global.h"

#include <QtCore>
#include <QObject>
#include <QString>
#include <QtNetwork/QTcpSocket>
#include <QIODevice>
#include <QByteArray>
#include <QMap>
#include <QMetaObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QHttpMultiPart>
#include <QUrl>

class /*ANKICONNECTLIB_EXPORT*/ RequestManager : public QObject
{
    Q_OBJECT
public:
    explicit RequestManager(QObject* parent = 0);
    ~RequestManager();

    void POST(const QString hostName, const QString& data);
signals:
    void sendSignal(QString data);

public slots:
    void handleFinished(QNetworkReply* networkReply);

private:
    QNetworkAccessManager* networkManager;
};

#endif // REQUESTMANAGER_H