#include "RequestManager.h"
 
/*!
    \fn RequestManager::RequestManager(QObject* parent)

    Sets up a network access manager and connects the finished signal
    emitted when a response is received to the handleFinished slot.
*/
RequestManager::RequestManager(QObject* parent) : QObject(parent)
{
    // create network manager
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &RequestManager::handleFinished);
}

/*!
    \fn RequestManager::~RequestManager()

    Deletes networkManager upon destruction of a RequestManager object.
*/
RequestManager::~RequestManager()
{
    delete networkManager;
}

/*!
    \fn void RequestManager::POST(const QString hostName, const QString& data)

    Creates an HTTP POST request that includes JSON \a data and then posts to \a hostName.
    Response is processed in the handleFinished slot.
*/
void RequestManager::POST(const QString hostName, const QString& data)
{
    QNetworkRequest request = QNetworkRequest(hostName);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
    networkManager->post(request, data.toUtf8());
}

 /*!
     \fn void RequestManager::handleFinished(QNetworkReply* networkReply)

     A slot trgiggered when an HTTP network request has finished.
     Emits the sendSignal signal that includes the response string.
 */
void RequestManager::handleFinished(QNetworkReply* networkReply)
{
    QString result;
    // free later
    networkReply->deleteLater();

    // no error in request
    if (networkReply->error() == QNetworkReply::NoError)
    {
        // get HTTP status code
        qint32 httpStatusCode = networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // 200
        if (httpStatusCode >= 200 && httpStatusCode < 300) // OK
        {
            result = networkReply->readAll();
        }
        else if (httpStatusCode >= 300 && httpStatusCode < 400) // 300 Redirect
        {
            // Get new url, can be relative
            QUrl relativeUrl = networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

            // url can be relative, we use the previous url to resolve it
            QUrl redirectUrl = networkReply->url().resolved(relativeUrl);

            // redirect to new url
            networkReply->manager()->get(QNetworkRequest(redirectUrl));

            // maintain manager
            return;
        }
        else if (httpStatusCode >= 400 && httpStatusCode < 500) // 400 Error
        {
            qDebug() << httpStatusCode << " Error!";
        }
        else if (httpStatusCode >= 500 && httpStatusCode < 600) // 500 Internal Server Error
        {
            qDebug() << httpStatusCode << " Error!";
        }
        else
        {
            qDebug() << "Status code invalid! " << httpStatusCode;
        }
    }
    else
    {
        qDebug() << "errorString: " << networkReply->errorString();
    }
    emit sendSignal(result);

    networkReply->manager()->deleteLater();
}