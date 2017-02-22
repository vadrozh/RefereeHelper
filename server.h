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
#include <QDesktopWidget>
#include <QTextStream>
#include <qtabwidget.h>
#include <QVBoxLayout>
#include <QShortcut>
#include <QFileDialog>
#include <QCloseEvent>
#include <QTimer>
#include <QMessageBox>
#include <math.h>
#include <cmath>
#include <signal.h>
#include <QThread>
#include <QLibraryInfo>
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
    QFile log;
    QTabWidget *tab = new QTabWidget();
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
    void CreateServerClicked();
    void replyFinished();
    void on_pbSend_clicked();
    void encChecked(bool);
    void textChanged();
    void nickChanged();
    void tabCloseRequested(int);
    void sendNomination(QString, QTcpSocket* = NULL);
    void sendCriteria(QTcpSocket * = NULL);
    void sendMembers(QStringList, QTcpSocket* = NULL);
    void AddNomination(QString nomination);
    void OpenConfigFile();
    void UpdateReferee();
    void CustomMenu(const QPoint&);
    void disconnectClientForcibly();
    void SendToNewClient();
    void ChangeResult(QTableWidget *table, QString);
    void SelectWinners(QTableWidgetItem *table);
    void AddClientToTables(QString nickName);
    void AddClientToList(QTcpSocket* clientSocket);
    void RemoveClientFromTables(int clientNum);
    void RemoveClientFromList(int clientNum);
    void ChangeClientNickName(int index);
protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::server *ui;
    QStringList data;
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QValidator *valid = new QRegExpValidator(QRegExp("[a-zA-Z0-9А-Яа-я]+"),this);
    QValidator *validPort = new QRegExpValidator(QRegExp("^(([0-9]{1,4})|([1-5][0-9]{4})|(6[0-4][0-9]{3})|(65[0-4][0-9]{2})|(655[0-2][0-9])|(6553[0-5]))$"),this);
    bool clientRequested = false;
    bool TableCreated = false;
    QShortcut *keyOpen = new QShortcut(this);
    QShortcut *keyOpenTab = new QShortcut(tab);
    QStringList Nominations;
    QStringList Criteria;
    QList<double> CriteriaMinValue;
    QList<double> CriteriaMaxValue;
    QList<double> CriteriaMultiplier;
    QVector<QStringList> Members;
    QString CompetitionName;
    QPoint globalPos;
    QMenu *menu = new QMenu(this);
    QAction *remove = new QAction("Отключить клиента",this);
    QFile File;
    QString DisqualificationPointer = "DISQUALIFICATION";
    QColor DEFAULT_COLOR;
};

QList<double> distribution(QList<double> list);
void handler(int signum);

#endif // SERVER_H
