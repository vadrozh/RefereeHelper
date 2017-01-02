/*
 * TODO:
 *
 *
*/
#include "server.h"
#include "ui_server.h"

server::server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::server)
{
    ui->setupUi(this);
    ui->pbSend->setEnabled(false);
    ui->cbEnc->setEnabled(false);
    setTrayIconActions();
    showTrayIcon();
    show();
   // QNetworkRequest request(QUrl("http://ipinfo.io/ip"));
    ui->tabWidget->setTabsClosable(true);
    ui->listNick->setContextMenuPolicy(Qt::CustomContextMenu);
    QNetworkRequest request(QUrl("http://www.grio.ru/myip.php"));
    QNetworkReply *reply = manager->get(request);
    connect(reply,SIGNAL(finished()),this,SLOT(replyFinished()));
    connect(ui->cbEnc,SIGNAL(clicked(bool)),this,SLOT(encChecked(bool)));
    connect(ui->textBrowser,SIGNAL(textChanged()),this,SLOT(textChanged()));
    connect(ui->cbLog,SIGNAL(clicked(bool)),this,SLOT(textChanged()));
    connect(ui->leNick,SIGNAL(editingFinished()),this,SLOT(nickChanged()));
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(tabCloseRequested(int)));
    connect(ui->listNick,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(CustomMenu(QPoint)));
    QIcon ico(QApplication::applicationDirPath()+"/ico.png");
    setWindowIcon(ico);
    ui->leNick->setValidator(valid);
    QDateTime date(QDateTime::currentDateTime());
    log.setFileName("log_"+date.toString("dd.MM.yy")+"_"+date.toString("hh_mm")+".txt");
    keyOpen->setKey(Qt::CTRL + Qt::Key_O);
    connect(keyOpen, SIGNAL(activated()), this, SLOT(OpenConfigFile()));
    //Обрабатываем получение внутреннего IP
    QList<QHostAddress> addressList = QNetworkInterface::allAddresses();
        foreach(QHostAddress address, addressList)
            if (address.toString().startsWith("192.168")){
                ui->le_locIP->setText(address.toString());
            }
        if (ui->le_locIP->text().isEmpty()){
        ui->le_locIP->hide();
        ui->label_IP->hide();
        ui->pbServer_create->setGeometry(180,130,91,41);
        ui->label_3->setGeometry(90,110,91,16);
    }
}
server::~server()
{
    delete ui;
}

void server::disconnectClientForcibly(){
    int clientNum;
    QListWidgetItem *selected = ui->listNick->selectedItems().first();
    for(int i = 0, l = ui->listNick->count(); i < l; i++)
         {
             if(ui->listNick->item(i)->text() == selected->text())
             {
                 clientNum = ui->listNick->row(selected);
             }
         }
    QTcpSocket* clientdis = (QTcpSocket*)clientList[clientNum];
    sendToClient(clientdis,QTime::currentTime().toString().toUtf8()+" - [СЕРВЕР]Вы были принудительно отключены.");
    for (int i=0; i<clientList.size(); i++){
        if (clientdis == clientList.at(i)){
            clientList.removeAt(i);
            foreach (QTcpSocket* client, clientList) {
                sendToClient(client, QTime::currentTime().toString().toUtf8()+" - [СЕРВЕР]Клиент "+nickList[i]+"("+ipList[i]+")"+" отключён принудительно.");
            }
            nickList.removeAt(i);
            ipList.removeAt(i);
            QListWidgetItem* item = ui->listNick->takeItem(i);
            delete item;
        }
    }
    UpdateReferee();
    clientdis->disconnectFromHost();
    QTimer::singleShot(2000,clientdis,SLOT(deleteLater()));
    if (ui->listNick->count() == 0) {

    }
}

void server::CustomMenu(const QPoint &pos){
    globalPos = ui->listNick->viewport()->mapToGlobal(pos);
    connect(remove,SIGNAL(triggered(bool)),this,SLOT(disconnectClientForcibly()));
    menu->addAction(remove);
    menu->exec(globalPos);
}

void server::tabCloseRequested(int index){
    ui->tabWidget->removeTab(index);
}

void server::nickChanged(){
    if (cli->state() == QTcpSocket::ConnectedState){
    QString nick = "//nick_"+ui->leNick->text();
    if (clientRequested){
        cli->write(nick.toUtf8().toBase64());
    } else {
        cli->write(nick.toUtf8());
    }
    }
}

void server::textChanged(){
    if (ui->cbLog->isChecked())
    {
        log.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream writeStream(&log);
        writeStream << ui->textBrowser->toPlainText();
        log.close();
    }
}

void server::encChecked(bool isChecked){
    if (isChecked){
        foreach (QTcpSocket* client, clientList) {
                sendToClient(client, "//encryptionEnabled");
        }
    } else if (!isChecked) {
        foreach (QTcpSocket* client, clientList) {
                sendToClient(client, QString("//encryptionDisabled").toUtf8().toBase64());
        }
    }
}

void server::replyFinished(){
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
      if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray content= reply->readAll();
            QTextCodec *codec = QTextCodec::codecForName("cp1251");
            //ui->textBrowser->append("Current IP -"+codec->toUnicode(content.data()));
            QString data = codec->toUnicode(content.data());
            //data.chop(1);
            ui->le_Currentip->setText(data);
        }
      else {
          ui->textBrowser->append("[СИСТЕМА]Ошибка подключения к серверу определения внешнего IP: <br>"+reply->errorString());
          ui->textBrowser->append("Вы подключены к интернету?");
      }
      reply->deleteLater();
}

void server::closeEvent(QCloseEvent *event){
    QString message;
    if (clientRequested){
        cli->write(message.toUtf8().toBase64());
    } else {
        cli->write(message.toUtf8());
    }
    File.close();
    event->accept();
}

void server::createServer(int port)
{
    if (!srv->listen(QHostAddress::Any, port)){
        ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СИСТЕМА]Ошибка запуска сервера: ");
        ui->textBrowser->append(srv->errorString());
        srv->close();
    }
    connect(srv,SIGNAL(newConnection()),this,SLOT(addNewClient()));
    QString msg =  QString::number(port);
    ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СИСТЕМА]Сервер запущен. Порт для подключения - " + msg +". Ваш IP - "+ui->le_Currentip->text());
    ui->pbSend->setEnabled(true);
    ui->cbEnc->setEnabled(true);
}

void server::createClient(QString address, int port)
{
    cli->connectToHost(address,port);
    ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СИСТЕМА]Клиент запущен. IP сервера - " + ui->le_ip->text() + ", порт - " + QString::number(port));
    connect(cli, SIGNAL(connected()), SLOT(serverConnected()));
    connect(cli, SIGNAL(readyRead()), SLOT(readFromServer()));
    connect(ui->leMessage, SIGNAL(returnPressed()), SLOT(sendToServer()));
    QString nick = "//nick_"+ui->leNick->text();
    cli->write(nick.toUtf8());
    ui->pbSend->setEnabled(true);
}

void server::addNewClient()
{
    QTcpSocket* clientSocket = srv->nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(disconnectClient()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readFromClient()));
    clientList.append(clientSocket);
    nickList.append("Неизвестно " + QString::number(nickList.size()));
    QString addr = clientSocket->peerAddress().toString();
    if (addr == "::1") {
        addr = "localhost";
    } else if (addr.startsWith("::ffff:")){
        addr.chop(7);
    }
    ui->listNick->addItem(nickList.last()+" ("+addr+")");
    ipList << addr;
    UpdateReferee();
    ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СЕРВЕР]Новый клиент подключён.");
    sendToClient(clientSocket, QTime::currentTime().toString().toUtf8()+" - "+"[СЕРВЕР]Вы подключены.");
}

void server::disconnectClient()
{
    QTcpSocket* client = (QTcpSocket*)sender();
    for (int i=0; i<clientList.size(); i++){
        if (client == clientList.at(i)){
            clientList.removeAt(i);
            foreach (QTcpSocket* client, clientList) {
                sendToClient(client, QTime::currentTime().toString().toUtf8()+" - [СЕРВЕР]Клиент "+nickList[i]+"("+ipList[i]+")"+" отключился.");
                }
            nickList.removeAt(i);
            ipList.removeAt(i);
            QListWidgetItem* item = ui->listNick->takeItem(i);
            delete item;
        }
    }
    client->deleteLater();
    UpdateReferee();
}

void server::readFromClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QString message;
    if(clientSocket->bytesAvailable()){
        if (clientRequested){
            message = QByteArray::fromBase64(clientSocket->readAll());
        } else {
        message = clientSocket->readAll();
        }
    }
    if (message.startsWith("//data ")) {
        QString datames = message.mid(7);
        QStringList list = datames.split(" ");
        QString numofTab = list.at(0);
        QList<QTableWidget *> tableList = ui->tabWidget->findChildren<QTableWidget *>();
        foreach (QTableWidget *Table, tableList) {
            if (Table->objectName() == "Table_" + numofTab){
                QTableWidgetItem *itab = new QTableWidgetItem();
                QString result = list.at(2);
                QString membercount = list.at(1);
                itab->setText(result);
                Table->setItem(membercount.toInt(),clientList.indexOf(clientSocket),itab);
            }
        }
    }
    if (message.startsWith("//nick_") && message.mid(7) != ""){
        QString mes = message.mid(7);
        nickList[clientList.indexOf(clientSocket)] = mes;
        QString addr = clientSocket->peerAddress().toString();
        if (addr == "::1") {
            addr = "localhost";
        } else {
            addr.chop(8);
        }
        ui->listNick->item(clientList.indexOf(clientSocket))->setText(mes+" ("+addr+")");
        UpdateReferee();
    } else
    if (!message.startsWith("//nick_")){
    foreach (QTcpSocket* client, clientList) {
        QString nickTime;
        if ((!message.contains("//criteria")) && (!message.contains("//nomination")) && (!message.contains("//member"))){
        nickTime = QTime::currentTime().toString().toUtf8()+" - "+"["+nickList[(clientList.indexOf(clientSocket))]+"]";
        sendToClient(client, nickTime+message);
        } else {
            sendToClient(client, message);
            }
        }
    }
}

void server::sendToClient(QTcpSocket *client, QString message)
{
    if (ui->cbEnc->isChecked() && clientRequested){
        client->write(message.toUtf8().toBase64());
    } else {
        client->write(message.toUtf8());
    }
}

void server::readFromServer()
{
    QString message;
    if(cli->bytesAvailable()){
        if (clientRequested){
        message = QByteArray::fromBase64(cli->readAll());
        } else {
            message = cli->readAll();
        }
        if (message == "//encryptionEnabled"){
            clientRequested = true;
            ui->cbEnc->setChecked(true);
        } else if (message == "//encryptionDisabled"){
            clientRequested = false;
            ui->cbEnc->setChecked(false);
        } else {
        ui->textBrowser->append(message);
        trayIcon->showMessage("Новое сообщение",message,QSystemTrayIcon::Information,1000);
        }
        QApplication::alert(this);
    }
}

void server::serverConnected()
{
    ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [КЛИЕНТ]Подключился к серверу.");
    ui->pbSend->setEnabled(true);
}

void server::sendToServer()
{
    if (!ui->leMessage->text().isEmpty())
    {
    QString message = ui->leMessage->text();
    if (clientRequested){
        cli->write(message.toUtf8().toBase64());
    } else {
        cli->write(message.toUtf8());
    }
    ui->leMessage->clear();
    } else {
        ui->textBrowser->append("[ОШИБКА]Введите текст сообщения.");
    }
}

void server::on_pbServer_clicked()
{
    createClient(ui->le_ip->text(),ui->lePort->text().toInt());
    ui->pbServer_create->setEnabled(false);
    ui->pbServer->setEnabled(false);
    ui->cbEnc->setEnabled(false);
}

void server::on_pbServer_create_clicked()
{
    createServer(ui->lePort_create->text().toInt());
    createClient("localhost",ui->lePort_create->text().toInt());
    ui->pbServer_create->setEnabled(false);
    ui->pbServer->setEnabled(false);
}


void server::on_pbSend_clicked()
{
    sendToServer();
}

void server::showTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    QIcon trayImage(QApplication::applicationDirPath()+"/img/ico.png");
    trayIcon->setIcon(trayImage);
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void server::trayActionExecute()
{
    showNormal();
    setWindowState(Qt::WindowActive);
}

void server::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            trayActionExecute();
            break;
        default:
            break;
    }
}

void server::setTrayIconActions()
{
    restoreAction = new QAction("Восстановить", this);
    quitAction = new QAction("Выход", this);
    connect (restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect (quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);
}

void server::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange)
    {
        if (isMinimized())
        {
            hide();
            trayIcon->show();
        }
        if (isActiveWindow()){
            trayIcon->hide();
        }
    }
}

void server::AddNomination(QString nomination){
    //Номинация
    QWidget *page = new QWidget();
    QTableWidget *NomTab = new QTableWidget();
    QVBoxLayout *Layout = new QVBoxLayout();
    Layout->addWidget(NomTab);
    page->setLayout(Layout);
    ui->tabWidget->addTab(page,nomination);
    ui->tabWidget->setTabEnabled(ui->tabWidget->count()-1,true);
    NomTab->setObjectName("Table_"+QString::number(ui->tabWidget->count()));
    //Устанавливаем список судей
    UpdateReferee();
    //Участники
    QStringList members = Members[ui->tabWidget->count()-1];
    QList<QTableWidget *> tableList = ui->tabWidget->findChildren<QTableWidget *>();
    tableList = ui->tabWidget->findChildren<QTableWidget *>();
    foreach (QTableWidget *Table, tableList) {
        if (Table->objectName() == ("Table_" + QString::number(ui->tabWidget->count()))){
            Table->setRowCount(members.size());
            Table->setVerticalHeaderLabels(members);
        }
    }
}

void server::sendNomination(QString nomination){
    QString msg = ";//nomination."+nomination;
    foreach (QTcpSocket* client, clientList) {
        sendToClient(client, msg.toUtf8());
        }
}

void server::sendCriteria(QStringList criteria){
    QString msg = ";//criteria";
    foreach (QString string, criteria) {
        msg+="."+string;
    }
    msg.simplified();
    foreach (QTcpSocket* client, clientList) {
        sendToClient(client, msg.toUtf8());
        }
}

void server::sendMembers(QStringList members){
    QString msg = ";//member";
    foreach (QString string, members) {
        msg+="."+string;
    }
    msg.simplified();
    foreach (QTcpSocket* client, clientList) {
        sendToClient(client, msg.toUtf8());
        }
}


void server::OpenConfigFile(){
    Nominations.clear();
    Criteria.clear();
    Members.clear();
    CompetitionName.clear();
    ui->tabWidget->clear();
    TableCreated = false;
    QString FileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл соревнования"),
                                QDir::currentPath(),
                                "Text files (*.txt)");
    File.setFileName(FileName);
    if (File.exists() && File.open(QIODevice::ReadOnly)){
        QString tempString;
        tempString.clear();
        while(!File.atEnd()){
            // Записываем построчно в tempString данные из файла.
            tempString = QString(File.readLine().simplified());
            if (!tempString.isEmpty() && !tempString.startsWith("#"))
            {
                // Название соревнования
                if(tempString == "[competition]"){
                    // Читаем следующую за идентификатором строку.
                    tempString = QString(File.readLine().simplified());
                    if (!tempString.isEmpty() && !tempString.startsWith("#")){
                        CompetitionName = tempString;
                    }
                    if (CompetitionName.isEmpty()){
                        CompetitionName = QString(QDate::currentDate().toString("Competition_dd_MM_yyyy"));
                    }
                    tempString = QString(File.readLine().simplified());
                }
                this->setWindowTitle("Referee Helper - " + CompetitionName);
                // Список критериев
                if(tempString == "[criteria]"){
                    QStringList _criteriaList;
                    // Читаем следующую за идентификатором строку
                    tempString = QString(File.readLine().simplified());
                    while(tempString != "[/criteria]"){
                        if (!tempString.isEmpty() && !tempString.startsWith("#")){
                            _criteriaList = tempString.split(" ");
                            Criteria.append(_criteriaList.at(0));
                        }
                        tempString = QString(File.readLine().simplified());
                    }
                }
                // #Список критериев
                // Список участников
                if(tempString == "[members]"){
                    int startIndex = true;
                    // Читаем следующую за идентификатором строку
                    tempString = QString(File.readLine().simplified());
                    // Список участников для данной номинации
                    while(tempString != "[/members]"){
                        if (tempString.isEmpty() || tempString ==   "[category]"){
                            tempString = QString(File.readLine().simplified());
                            startIndex = true;
                        }
                        if (startIndex){
                            Nominations.append(tempString);
                            startIndex = false;
                            Members.append(QStringList());
                        } else {
                            Members[Nominations.size()-1].append(tempString);
                        }
                        tempString = QString(File.readLine().simplified());
                    }
                }
                // #Список участников
            }
        }
    }
    if (File.exists()){
        for (int i = 0; i < Nominations.size(); ++i) {
            AddNomination(Nominations[i]);
            sendNomination(Nominations[i]);
            sendMembers(Members[i]);
        }
        sendCriteria(Criteria);
        TableCreated = true;
    }
}

void server::UpdateReferee(){
    QList<QTableWidget *> tableList = ui->tabWidget->findChildren<QTableWidget *>();
    foreach (QTableWidget *Table, tableList) {
            QStringList referee = nickList;
            referee.removeFirst();
            Table->setColumnCount(referee.size());
            Table->setHorizontalHeaderLabels(referee);
    }
}
