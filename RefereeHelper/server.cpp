#include "server.h"
#include "ui_server.h"

#define LRed     QColor(255,120,100,150)
#define LGreen   QColor(120,255,100)
#define LYellow  QColor(255,255,100)
//---------------------------------------------------------
#define DRed     QColor(125,10,0)
#define DGreen   QColor(10,125,0)
#define DYellow  QColor(125,125,0)

server::server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::server)
{
    signal(SIGSEGV, handler);
    signal(SIGFPE, handler);
    signal(SIGILL, handler);

    ui->setupUi(this);
    ui->pbSend->setEnabled(false);
    ui->cbEnc->setEnabled(false);

    setFixedHeight(460);
    setFixedWidth(496);

    tab->setTabsClosable(true);
    tab->setGeometry(qApp->desktop()->availableGeometry());

    show();

    ui->listNick->setContextMenuPolicy(Qt::CustomContextMenu);

    //Если вдруг тот сайт не будет работать.
    //QNetworkRequest request(QUrl("http://ipinfo.io/ip"));
    QNetworkRequest request(QUrl("http://www.grio.ru/myip.php"));
    QNetworkReply *reply = manager->get(request);

    connect(reply,SIGNAL(finished()),SLOT(replyFinished()));
    connect(ui->cbEnc,SIGNAL(clicked(bool)),SLOT(encChecked(bool)));
    connect(ui->textBrowser,SIGNAL(textChanged()),SLOT(textChanged()));
    connect(ui->leNick,SIGNAL(editingFinished()),SLOT(nickChanged()));
    connect(tab,SIGNAL(tabCloseRequested(int)),SLOT(tabCloseRequested(int)));
    connect(ui->listNick,SIGNAL(customContextMenuRequested(QPoint)),SLOT(CustomMenu(QPoint)));
    connect(ui->pbServer_create,SIGNAL(clicked(bool)),SLOT(CreateServerClicked()));

    ui->leNick->setValidator(valid);
    ui->lePort_create->setValidator(validPort);

    QDateTime date(QDateTime::currentDateTime());
    log.setFileName("log_"+date.toString("dd.MM.yy")+"_"+date.toString("hh_mm")+".txt");
    log.open(QIODevice::Append);
    QString str = QT_VERSION_STR;
    QTextStream out(&log);
    out << "     RefereeHelper(Server)\n"
           "     Built on " __DATE__ " at " __TIME__ ".\n"
           "     Based on Qt "+str.toLatin1()+".\n"
           "     by vadrozh(vk.com/rozzhk) and ejik(vk.com/av.kochekov)\n";
    out << "     Builded for: "+QSysInfo::buildAbi()+"\n";
    out << "     Current Architecture: "+QSysInfo::currentCpuArchitecture()+"\n";
    QString data = QString::number(QSysInfo::windowsVersion()+QSysInfo::macVersion());
    out << "     Windows/Mac version: "+data+"\n";
    out << "     Is debug: "+QString::number(QLibraryInfo::isDebugBuild())+"\n";

    keyOpen->setKey(Qt::CTRL + Qt::Key_O);
    keyOpenTab->setKey(Qt::CTRL + Qt::Key_O);
    connect(keyOpen, SIGNAL(activated()), this, SLOT(OpenConfigFile()));
    connect(keyOpenTab, SIGNAL(activated()), this, SLOT(OpenConfigFile()));

    //Обрабатываем получение внутреннего IP
    QList<QHostAddress> addressList = QNetworkInterface::allAddresses();
    foreach(QHostAddress address, addressList)
    if (address.toString().startsWith("192.168")) ui->le_locIP->setText(address.toString());
    if (ui->le_locIP->text().isEmpty()){
        ui->le_locIP->hide();
        ui->label_IP->hide();
        ui->pbServer_create->setGeometry(180,130,91,41);
        ui->label_3->setGeometry(90,110,91,16);
    }
}
server::~server()
{
    QTextStream out(&log);
    out << QTime::currentTime().toString().toUtf8()+" - Successfully closed.";
    log.close();
    if (!ui->cbLog->isChecked()){
        log.remove();
    }
    delete ui;
}

void server::disconnectClientForcibly(){
    int clientNum = 0;
    for(int i = 0, l = ui->listNick->count(); i < l; i++)
         {
             if(ui->listNick->item(i)->text() == ui->listNick->currentItem()->text())
             {
                 clientNum = ui->listNick->row(ui->listNick->currentItem());
             }
         }
    QTcpSocket* clientdis = (QTcpSocket*)clientList.at(clientNum);
    for (int i=0; i<clientList.size(); i++){
        if (clientdis == clientList.at(i)){
            clientList.removeAt(i);
            foreach (QTcpSocket* client, clientList) {
                sendToClient(client, QTime::currentTime().toString().toUtf8()+" - [СЕРВЕР]Клиент "+nickList[i]+"("+ipList[i]+")"+" был отключён принудительно.");
            }
            nickList.removeAt(i);
            ipList.removeAt(i);
            QListWidgetItem* item = ui->listNick->takeItem(i);
            delete item;
        }
    }
    clientdis->disconnectFromHost();
    QTimer::singleShot(2000,clientdis,SLOT(deleteLater()));
}

void server::CustomMenu(const QPoint &pos){
    if (!ui->listNick->selectedItems().isEmpty() && ui->listNick->selectedItems().first() != ui->listNick->item(0)) {
    globalPos = ui->listNick->viewport()->mapToGlobal(pos);
    connect(remove,SIGNAL(triggered(bool)),this,SLOT(disconnectClientForcibly()));
    menu->addAction(remove);
    menu->exec(globalPos);
    }
}

void server::tabCloseRequested(int index){
    QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
    foreach (QTableWidget *Table, tableList) {
        if (Table->objectName() == ("Table_" + QString::number(index))){
            Table->deleteLater();
        }
    }
    tab->removeTab(index);
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
    QString str = ui->textBrowser->toPlainText();
    QStringList lst = str.split("\n");
    QTextStream out(&log);
    out << "[Chat] - "+lst.last()+"\n";
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
            QString data = codec->toUnicode(content.data());
            //Если другой сайт обрезаем пробел
            //data.chop(1);
            ui->le_Currentip->setText(data);
        }
      else {
          ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СИСТЕМА]Ошибка подключения к серверу определения внешнего IP: "+reply->errorString());
          ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СИСТЕМА]Вы точно подключены к интернету?");
      }
      reply->deleteLater();
}

void server::closeEvent(QCloseEvent *event){
    tab->close();
    File.close();
    event->accept();
}

void server::createServer(int port)
{
    if (!srv->listen(QHostAddress::Any, port)){
        ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СИСТЕМА]Ошибка запуска сервера: ");
        ui->textBrowser->append(srv->errorString());
        srv->close();
    } else {
        connect(srv,SIGNAL(newConnection()),this,SLOT(addNewClient()));
        QString msg = QString::number(port);
        ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СИСТЕМА]Сервер запущен. Порт для подключения - " + msg +". Ваш IP - "+ui->le_Currentip->text());
        ui->pbSend->setEnabled(true);
        ui->cbEnc->setEnabled(false);
        //!!!!!!!!!!
    }
}

void server::createClient(QString address, int port)
{
    cli->connectToHost(address,port);
    ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СИСТЕМА]Клиент запущен.");
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
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(disconnectClient()),Qt::DirectConnection);
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(readFromClient()),Qt::DirectConnection);
    AddClientToList(clientSocket);
    AddClientToTables(nickList.last());
    ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [СЕРВЕР]Новый клиент подключён.");
    QTimer::singleShot(2000,this,SLOT(SendToNewClient()));
}

void server::disconnectClient()
{
    QTcpSocket* client = (QTcpSocket*)sender();
    for (int i=0; i<clientList.size(); i++){
        if (client == clientList.at(i)){
            RemoveClientFromTables(i);
            RemoveClientFromList(i);
            QListWidgetItem* item = ui->listNick->takeItem(i);
            QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
            foreach (QTableWidget *Table, tableList) {
                for (int row = 0; row < Table->rowCount(); row++)
                {
                    ChangeResult(Table,QString::number(row));
                }
            }
            client->deleteLater();
            delete item;
            break;
        }
    }
}

void server::readFromClient()
{
    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    QString message;
    if(clientSocket->bytesAvailable()){
        if (clientRequested){
            message = QByteArray::fromBase64(clientSocket->readAll());
        } else {
        message = clientSocket->readLine();
        }
    }
    QTextStream out(&log);
    out << QTime::currentTime().toString().toUtf8()+" - [Readed from client] - "+message+"\n";
    if (message.startsWith("//data ")) {
        QString datames = message.mid(7);
        QStringList list = datames.split(" ");
        QString numofTab = list.at(0);
        QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
        foreach (QTableWidget *Table, tableList) {
            if (Table->objectName() == "Table_" + numofTab){
                QTableWidgetItem *itab = new QTableWidgetItem();
                QString result = list.at(2);
                QString membercount = list.at(1);
                itab->setText(result);
                Table->setItem(membercount.toInt(),clientList.indexOf(clientSocket)-1,itab);
                ChangeResult(Table,membercount);
            }
        }
    }
    if (message.startsWith("//nick_") && message.mid(7) != ""){
        int index = clientList.indexOf(clientSocket);
        QString mes = message.mid(7);
        nickList[index] = mes;
        ChangeClientNickName(index);
    } else
    if (!message.startsWith("//nick_")  && !message.startsWith(";//") && !message.startsWith("//data")){
    foreach (QTcpSocket* client, clientList) {
        QString nickTime;
        if (!message.contains("//criteria") && !message.contains("//nomination") && !message.contains("//member")){
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
        QTextStream out(&log);
        out << QTime::currentTime().toString().toUtf8()+" - [Readed from server] - "+message+"\n";
        if (message == "//encryptionEnabled"){
            clientRequested = true;
            ui->cbEnc->setChecked(true);
        } else if (message == "//encryptionDisabled"){
            clientRequested = false;
            ui->cbEnc->setChecked(false);
        } else if (!message.contains("//criteria") && !message.contains("//nomination") && !message.contains("//member")){
            ui->textBrowser->append(message);
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
        ui->textBrowser->append(QTime::currentTime().toString().toUtf8()+" - [ОШИБКА]Введите текст сообщения.");
    }
}

void server::CreateServerClicked()
{
    createServer(ui->lePort_create->text().toInt());
    createClient("localhost",ui->lePort_create->text().toInt());
    ui->pbServer_create->setEnabled(false);
}

void server::on_pbSend_clicked()
{
    sendToServer();
}

void server::AddNomination(QString nomination)
{
    //Номинация
    QWidget *page = new QWidget(this);
    QTableWidget *NomTab = new QTableWidget(this);
    QVBoxLayout *Layout = new QVBoxLayout();
    Layout->addWidget(NomTab);
    page->setLayout(Layout);
    tab->addTab(page,nomination);
    tab->setTabEnabled(tab->count()-1,true);
    NomTab->setObjectName("Table_"+QString::number(tab->count()));
    NomTab->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //Участники
    QStringList members = Members[tab->count()-1];
    QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
    foreach (QTableWidget *Table, tableList) {
        if (Table->objectName() == ("Table_" + QString::number(tab->count()))){
            Table->setRowCount(members.size());
            Table->setVerticalHeaderLabels(members);
        }
    }
    //Устанавливаем список судей
    UpdateReferee();
}

void server::sendNomination(QString nomination, QTcpSocket *socket)
{
    if (srv->isListening()) {
        QString msg = ";//nomination."+nomination;
        if (socket == NULL){
            foreach (QTcpSocket* client, clientList) {
                sendToClient(client, msg.toUtf8());
            }
        } else {
            sendToClient(socket,msg.toUtf8());
        }
    }
}

void server::sendCriteria(QTcpSocket *socket){
    QString msg;
    msg.clear();
    if (srv->isListening()) {
        msg = ";//criteria";
        for (int i = 0; i < Criteria.size(); i++){
            msg+="."+ Criteria.at(i) + ":" +
                      QString::number(CriteriaMinValue.at(i)) + ":" +
                      QString::number(CriteriaMaxValue.at(i)) + ":" +
                      QString::number(CriteriaMultiplier.at(i));
        }
    }

    msg = msg.simplified();
    if (socket != NULL){
        sendToClient(socket, msg.toUtf8());
    } else {
        foreach (QTcpSocket* client, clientList) {
            sendToClient(client, msg.toUtf8());
        }
    }
}

void server::sendMembers(QStringList members, QTcpSocket *socket){
    if (srv->isListening()) {
        QString msg = ";//member";
        foreach (QString string, members) {
            msg+="."+string;
        }
        msg = msg.simplified();
        if (socket == NULL){
            foreach (QTcpSocket* client, clientList) {
                sendToClient(client, msg.toUtf8());
            }
        } else {
            sendToClient(socket,msg.toUtf8());
        }
    }
}


void server::OpenConfigFile(){
    //int *i = NULL;
    //*i = 5;
    Nominations.clear();
    Criteria.clear();
    Members.clear();
    CompetitionName.clear();
    QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
    foreach (QTableWidget *Table, tableList) {
            disconnect(Table);
    }
    tab->clear();
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
                tab->setWindowTitle("Referee Helper - " + CompetitionName);
                // Список критериев
                if(tempString.startsWith("[disqualification]")){
                    DisqualificationPointer = tempString.split(" ").at(1);
                }
                if(tempString.startsWith("[criteria]")){
                    int multiplier = 1;
                    QStringList _criteriaList;
                    // Читаем следующую за идентификатором строку
                    tempString = QString(File.readLine().simplified());
                    while(tempString != "[/criteria]"){

                        if (!tempString.isEmpty() && !tempString.startsWith("#") && !tempString.startsWith("[criteria]")){
                            if (tempString.startsWith("[points]")){
                                multiplier = 1;
                            } else if (tempString.startsWith("[fines]")){
                                multiplier = -1;
                            } else {
                                _criteriaList = tempString.split("-");
                                Criteria.append(_criteriaList.at(0));
                                if (_criteriaList.size() != 1){
                                    QString MinMaxValues = _criteriaList.at(1).simplified();
                                    if (MinMaxValues != DisqualificationPointer){
                                        QStringList MinMaxList = MinMaxValues.split(":");
                                        if (MinMaxList.size() == 2){
                                            CriteriaMinValue.append(MinMaxList.at(0).toInt());
                                            if (MinMaxList.at(1).isEmpty()){
                                                CriteriaMaxValue.append(-1);
                                            } else {
                                                CriteriaMaxValue.append(MinMaxList.at(1).toInt());
                                            }
                                            CriteriaMultiplier.append(multiplier);
                                        } else {
                                            CriteriaMinValue.append(0);
                                            CriteriaMaxValue.append(-1);
                                            CriteriaMultiplier.append(multiplier);
                                        }
                                    } else {
                                        CriteriaMinValue.append(0);
                                        CriteriaMaxValue.append(1);
                                        CriteriaMultiplier.append(0);
                                    }
                                } else {
                                    CriteriaMinValue.append(0);
                                    CriteriaMaxValue.append(-1);
                                    CriteriaMultiplier.append(multiplier);
                                }
                            }
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
                        if (tempString.isEmpty()){
                            while (tempString.isEmpty() && !File.atEnd()){
                                tempString = QString(File.readLine().simplified());
                            }
                            if (tempString == "[/members]"){
                                break;
                            }
                            startIndex = true;
                        }
                        if (tempString.startsWith("[category]")){
                            tempString = tempString.split("[category]").at(1);
                            tempString.simplified();
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
    File.close();
    if (File.exists() && !Nominations.isEmpty() && !Members.isEmpty()){
        for (int i = 0; i < Nominations.size(); ++i) {
            AddNomination(Nominations[i]);
            sendNomination(Nominations[i]);
            sendMembers(Members[i]);
        }
        sendCriteria();
        TableCreated = true;
        tab->show();
        QTextStream out(&log);
        out << QTime::currentTime().toString().toUtf8()+" - [Succssesfully opened config] - "+CompetitionName+"\n";
        QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
        foreach (QTableWidget *Table, tableList) {
            connect(Table,SIGNAL(itemChanged(QTableWidgetItem*)),SLOT(SelectWinners(QTableWidgetItem*)));
        }
    } else if (File.exists()){
        QMessageBox::warning(this,"Ошибка","Данный файл не содержит информацию о соревновании.");
        OpenConfigFile();
    }
}

void server::UpdateReferee(){
    QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
    foreach (QTableWidget *Table, tableList) {
        QStringList referee = nickList;
        if (!referee.isEmpty()){
            referee.removeFirst();
        }
        Table->setColumnCount(referee.size()+1);
        Table->setHorizontalHeaderLabels(referee << "Всего");
        for (int col = 0; col < Table->columnCount(); col++){
            for (int row = 0; row < Table->rowCount(); row++) {
                Table->setItem(row,col,new QTableWidgetItem("0"));
            }
        }
     }
}


void server::SendToNewClient(){
    if (TableCreated) {
        QTcpSocket *client = clientList.last();
        for (int i = 0; i < Nominations.size(); ++i) {
            sendNomination(Nominations.at(i), client);
            sendMembers(Members.at(i), client);
       }
        sendCriteria(client);
    }
}

void server::ChangeResult(QTableWidget *table,QString _row){
    float result = 0;
    int row = _row.toInt();
    for (int i = 0; i < table->columnCount()-1; ++i) {
        result += table->item(row, i)->text().toDouble();
    }
    table->item(row,table->columnCount()-1)->setText(QString::number(result));
}

void server::SelectWinners(QTableWidgetItem *_item)
{
    QTableWidget *Table = _item->tableWidget();

    int col = Table->columnCount()-1;
    int row = Table->rowCount();

        QList<double> Results;
        Results.clear();
        for (int i=0; i<row; i++){
            QString RES = Table->item(i,col)->text();
            double RESD;
            if (RES == "0" || RES == "0.0")
            {
                RESD = 0;
            } else {
                RESD = RES.toDouble();
            }
            Results.append(RESD);
        }

        QList<int> Winners;
        Winners.clear();

        int placesMax = 3;

        for (int place=0; place<placesMax; place++){
            double maxResult = 0;
            double maxResultIndex = 0;
            for (int i=0; i<Results.size(); i++){
                if (Results[i]>maxResult){
                    maxResult = Results[i];
                    maxResultIndex = i;
                }
            }
            Winners << maxResultIndex;
            Results[maxResultIndex] = -1;
        }

        QList<float> Place;
        Place << Table->item(Winners[2],col)->text().toFloat();
        Place << Table->item(Winners[1],col)->text().toFloat();
        Place << Table->item(Winners[0],col)->text().toFloat();

        QFont Bold,Regular;
        Bold.setBold(true);
        Regular.setBold(false);

        for(int i=0; i<row; i++){
            QTableWidgetItem *CurrentItem = Table->item(i,col);
            if(CurrentItem){
                float CurrentItemValue = CurrentItem->text().toFloat();
                if (CurrentItemValue == Place.at(2)){
                    Table->verticalHeaderItem(i)->setForeground(QBrush(DGreen));
                    Table->verticalHeaderItem(i)->setFont(Bold);
                } else
                if (CurrentItemValue == Place.at(1)){
                    Table->verticalHeaderItem(i)->setForeground(QBrush(DYellow));
                    Table->verticalHeaderItem(i)->setFont(Bold);
                } else
                    if (CurrentItemValue == Place.at(0)){
                        Table->verticalHeaderItem(i)->setForeground(QBrush(DRed));
                        Table->verticalHeaderItem(i)->setFont(Bold);
                    } else {
                        Table->verticalHeaderItem(i)->setForeground(Qt::black);
                        Table->verticalHeaderItem(i)->setFont(Regular);
                }
                    if(CurrentItemValue == 0){
                        Table->verticalHeaderItem(i)->setForeground(Qt::black);
                        Table->verticalHeaderItem(i)->setFont(Regular);
                }
                    CurrentItem->setForeground(Table->verticalHeaderItem(i)->foreground());
            }
        }
}

void server::AddClientToTables(QString nickName){
    QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
    int col = nickList.size()-2;
    foreach (QTableWidget *Table, tableList) {
        Table->insertColumn(col);
        Table->setHorizontalHeaderItem(col, new QTableWidgetItem(nickName));
        for (int row = 0; row < Table->rowCount(); row++)
        {
            Table->setItem(row,col,new QTableWidgetItem("0"));
        }
    }
}

void server::AddClientToList(QTcpSocket *clientSocket)
{
    clientList.append(clientSocket);
    if (nickList.isEmpty()) {
        nickList.append("Сервер");
    } else {
        nickList.append("Неизвестно " + QString::number(nickList.size()));
    }
    QString addr = clientSocket->peerAddress().toString();
    if (addr == "::1" || addr == "::ffff:127.0.0.1")  {
        addr = "localhost";
    } else if (addr.startsWith("::ffff:")){
        addr = addr.mid(7);
    }
    ipList << addr;
    ui->listNick->addItem(nickList.last()+" ("+ipList.last()+")");
}

void server::RemoveClientFromTables(int clientNum){
    QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
    foreach (QTableWidget *Table, tableList) {
        Table->removeColumn(clientNum-1);
    }
}

void server::RemoveClientFromList(int clientNum)
{
    clientList.removeAt(clientNum);
    foreach (QTcpSocket* client, clientList) {
        sendToClient(client, QTime::currentTime().toString().toUtf8()+" - [СЕРВЕР]Клиент "+nickList[clientNum]+"("+ipList[clientNum]+")"+" отключился.");
        }
    nickList.removeAt(clientNum);
    ipList.removeAt(clientNum);
}

void server::ChangeClientNickName(int index){
    ui->listNick->item(index)->setText(nickList.at(index)+" ("+ipList[index]+")");
    QList<QTableWidget *> tableList = tab->findChildren<QTableWidget *>();
    foreach (QTableWidget *Table, tableList) {
        Table->setHorizontalHeaderItem(index-1,new QTableWidgetItem(nickList.at(index)));
    }
}

QList<double> distribution(QList<double> list){
    int placeCount = list.size();
    if (placeCount > 3){
        placeCount = 3;
    }
    QList<double> values = list;
    QList<int> placesDistribution;
    for (int place = 0; place < placeCount; place++){
        double maxValue = list.at(0);
        double maxIndex = 0;
        for (int i = 0; i < list.size(); i++) {
            double value = list.at(i);
            if (value > maxValue){
                maxValue = value;
                maxIndex = i;
            }
        }
        list[maxIndex] = -999999;
        placesDistribution.append(maxIndex);
    }
    int distributionIndex = 2;
    QList<double> places;
    places.clear();
    for (int i = 0; i < values.size(); i++){
        if (i == placesDistribution.at(distributionIndex)){
            places.append(distributionIndex + 1);
        } else {
            places.append(0);
        }
    }
    return values;
}

void handler(int signum)
{
    QString errorInfo = "Unknown("+QString::number(signum)+")";
    if (signum == 11)
    {
        errorInfo = "SIGSEGV("+QString::number(signum)+") - Segmentation Fault.";
    }
    if (signum == 8){
        errorInfo = "SIGFPE("+QString::number(signum)+") - Arithmetic Exception";
    }
    if (signum == 4){
        errorInfo = "SIGILL("+QString::number(signum)+") - Illegal Instruction.";
    }
    QMessageBox::warning(0,"Критическая ошибка", "Упс! Что-то пошло не так..."
                           "\nПриложение будет закрыто. Во всем виноват криворукий разработчик."
                           "\nИнформация об ошибке: \n"+errorInfo+
                           "\nИнформация записана в лог-файл.");

    QFile log;
    QDateTime date(QDateTime::currentDateTime());
    log.setFileName("Error_"+date.toString("dd.MM.yy")+"_"+date.toString("hh_mm")+".txt");
    log.open(QIODevice::Append);
    QString str = QT_VERSION_STR;
    QTextStream out(&log);
    out << "     RefereeHelper(Server)\n"
           "     Built on " __DATE__ " at " __TIME__ ".\n"
           "     Based on Qt "+str.toLatin1()+".\n"
           "     by vadrozh(vk.com/rozzhk) and ejik(vk.com/av.kochekov)\n";
    out << "     Builded for: "+QSysInfo::buildAbi()+"\n";
    out << "     Current Architecture: "+QSysInfo::currentCpuArchitecture()+"\n";
    QString data = QString::number(QSysInfo::windowsVersion())+"/"+QString::number(QSysInfo::macVersion());
    out << "     Windows/Mac version: "+data+"\n";
    out << "     Is debug: "+QString::number(QLibraryInfo::isDebugBuild())+"\n";
    out << QTime::currentTime().toString().toUtf8()+" - [Critical Error] - "+errorInfo+"\n";
    log.close();

    signal(signum, SIG_DFL);
    exit(signum);
}
