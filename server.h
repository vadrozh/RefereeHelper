#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QMainWindow>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextCodec>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QIcon>
#include <QStringList>
#include <QValidator>
#include <QNetworkInterface>
#include <qmath.h>
#include <QRegExpValidator>
#include <QTableWidgetItem>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <qtabwidget.h>
#include <QVBoxLayout>
#include <QShortcut>
#include <QFileDialog>
#include <QCloseEvent>
#include <QTimer>
namespace Ui {
class server;
}

class server : public QMainWindow
{
    Q_OBJECT

public:
    explicit server(QWidget *parent = 0);
    ~server();
    QTcpServer *srv = new QTcpServer(this);
    QTcpSocket *cli = new QTcpSocket(this);
    QList<QTcpSocket *> clientList;
    QStringList nickList;
    QStringList ipList;
public slots:
    void addNewClient();
    void disconnectClient();
    void readFromClient();
    void sendToClient(QTcpSocket* client, QString message);
    void readFromServer();
    void serverConnected();
    void sendToServer();
    void createClient(QString address, int port);
    void createServer(int port);
private slots:
    void on_pbServer_clicked();
    void on_pbServer_create_clicked();
    void replyFinished();
    void on_pbSend_clicked();
    void changeEvent(QEvent*);
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void trayActionExecute();
    void setTrayIconActions();
    void showTrayIcon();
    void encChecked(bool);
    void textChanged();
    void nickChanged();
    void tabCloseRequested(int);
    void sendNomination(QString);
    void sendCriteria(QStringList);
    void sendMembers(QStringList);
    void AddNomination(QString nomination);
    void OpenConfigFile();
    void UpdateReferee();
    void CustomMenu(const QPoint&);
    void disconnectClientForcibly();
protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::server *ui;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QStringList data;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QValidator *valid = new QRegExpValidator(QRegExp("[a-zA-Z0-9А-Яа-я ]+"),this);
    bool clientRequested = false;
    bool TableCreated = false;
    QFile log;
    QShortcut *keyOpen = new QShortcut(this);
    QStringList Nominations;
    QStringList Criteria;
    QVector<QStringList> Members;
    QString CompetitionName;
    QPoint globalPos;
    QMenu *menu = new QMenu(this);
    QAction *remove = new QAction("Отключить клиента",this);
    QFile File;
};

#endif // SERVER_H
