#pragma once

#include "ankiconnectlib_global.h"

#include <QtCore>
#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>

/*!
    \class RequestManager

    \brief The RequestManager class provides methods
    to send requests to a host. Responses are processed in the handleFinished slot.
*/
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