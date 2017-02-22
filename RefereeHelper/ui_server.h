/********************************************************************************
** Form generated from reading UI file 'server.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVER_H
#define UI_SERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_server
{
public:
    QWidget *centralwidget;
    QTextBrowser *textBrowser;
    QGroupBox *groupBox;
    QLabel *label_6;
    QLineEdit *le_Currentip;
    QPushButton *pbServer_create;
    QLabel *label_5;
    QLineEdit *lePort_create;
    QLabel *label_3;
    QCheckBox *cbEnc;
    QCheckBox *cbLog;
    QListWidget *listNick;
    QLabel *label_7;
    QLabel *label_IP;
    QLineEdit *le_locIP;
    QPushButton *pbSend;
    QLineEdit *leNick;
    QLineEdit *leMessage;

    void setupUi(QMainWindow *server)
    {
        if (server->objectName().isEmpty())
            server->setObjectName(QStringLiteral("server"));
        server->resize(496, 460);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(server->sizePolicy().hasHeightForWidth());
        server->setSizePolicy(sizePolicy);
        server->setMinimumSize(QSize(496, 460));
        server->setMaximumSize(QSize(496, 460));
        server->setStyleSheet(QStringLiteral(""));
        centralwidget = new QWidget(server);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        textBrowser = new QTextBrowser(centralwidget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(10, 190, 481, 231));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 481, 171));
        groupBox->setStyleSheet(QStringLiteral(""));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 80, 31, 20));
        le_Currentip = new QLineEdit(groupBox);
        le_Currentip->setObjectName(QStringLiteral("le_Currentip"));
        le_Currentip->setGeometry(QRect(100, 60, 91, 20));
        le_Currentip->setReadOnly(true);
        pbServer_create = new QPushButton(groupBox);
        pbServer_create->setObjectName(QStringLiteral("pbServer_create"));
        pbServer_create->setGeometry(QRect(190, 40, 81, 61));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 60, 41, 21));
        lePort_create = new QLineEdit(groupBox);
        lePort_create->setObjectName(QStringLiteral("lePort_create"));
        lePort_create->setGeometry(QRect(100, 80, 91, 20));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(100, 20, 111, 16));
        cbEnc = new QCheckBox(groupBox);
        cbEnc->setObjectName(QStringLiteral("cbEnc"));
        cbEnc->setEnabled(false);
        cbEnc->setGeometry(QRect(30, 140, 281, 21));
        cbEnc->setTristate(false);
        cbLog = new QCheckBox(groupBox);
        cbLog->setObjectName(QStringLiteral("cbLog"));
        cbLog->setGeometry(QRect(30, 120, 131, 21));
        listNick = new QListWidget(groupBox);
        listNick->setObjectName(QStringLiteral("listNick"));
        listNick->setGeometry(QRect(300, 30, 171, 131));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(300, 10, 171, 16));
        label_IP = new QLabel(groupBox);
        label_IP->setObjectName(QStringLiteral("label_IP"));
        label_IP->setGeometry(QRect(20, 40, 81, 16));
        le_locIP = new QLineEdit(groupBox);
        le_locIP->setObjectName(QStringLiteral("le_locIP"));
        le_locIP->setGeometry(QRect(100, 40, 91, 21));
        le_locIP->setReadOnly(true);
        pbSend = new QPushButton(centralwidget);
        pbSend->setObjectName(QStringLiteral("pbSend"));
        pbSend->setGeometry(QRect(400, 430, 91, 21));
        leNick = new QLineEdit(centralwidget);
        leNick->setObjectName(QStringLiteral("leNick"));
        leNick->setGeometry(QRect(10, 430, 81, 21));
        leNick->setMaxLength(32);
        leMessage = new QLineEdit(centralwidget);
        leMessage->setObjectName(QStringLiteral("leMessage"));
        leMessage->setGeometry(QRect(90, 430, 311, 21));
        leMessage->setMaxLength(255);
        server->setCentralWidget(centralwidget);

        retranslateUi(server);

        QMetaObject::connectSlotsByName(server);
    } // setupUi

    void retranslateUi(QMainWindow *server)
    {
        server->setWindowTitle(QApplication::translate("server", "Referee Helper", Q_NULLPTR));
        textBrowser->setHtml(QApplication::translate("server", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">                                       RefereeHelper(Server) v0.9(Stable)</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">                   by vadrozh(vk.com/rozzhk) &amp; ejik(vk.com/av.kochekov)</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-weight:600;\"><br /></p></body></html>", Q_NULLPTR));
        textBrowser->setPlaceholderText(QString());
        groupBox->setTitle(QApplication::translate("server", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213 \320\277\320\276\320\264\320\272\320\273\321\216\321\207\320\265\320\275\320\270\321\217", Q_NULLPTR));
        label_6->setText(QApplication::translate("server", "\320\237\320\276\321\200\321\202", Q_NULLPTR));
        le_Currentip->setText(QApplication::translate("server", "\320\275\320\265\320\270\320\267\320\262\320\265\321\201\321\202\320\265\320\275", Q_NULLPTR));
        pbServer_create->setText(QApplication::translate("server", "\320\227\320\260\320\277\321\203\321\201\320\272\n"
"\321\201\320\265\321\200\320\262\320\265\321\200\320\260", Q_NULLPTR));
        label_5->setText(QApplication::translate("server", "\320\222\320\260\321\210 IP", Q_NULLPTR));
        lePort_create->setText(QApplication::translate("server", "20333", Q_NULLPTR));
        label_3->setText(QApplication::translate("server", "\320\227\320\260\320\277\321\203\321\201\320\272 \321\201\320\265\321\200\320\262\320\265\321\200\320\260", Q_NULLPTR));
        cbEnc->setText(QApplication::translate("server", "\320\222\320\272\320\273\321\216\321\207\320\270\321\202\321\214 \321\210\320\270\321\204\321\200\320\276\320\262\320\260\320\275\320\270\320\265(\320\277\320\276\320\272\320\260-\321\207\321\202\320\276 \320\275\320\265 \321\200\320\260\320\261\320\276\321\202\320\260\320\265\321\202)", Q_NULLPTR));
        cbLog->setText(QApplication::translate("server", "\320\241\320\276\321\205\321\200\320\260\320\275\321\217\321\202\321\214 \320\273\320\276\320\263-\321\204\320\260\320\271\320\273", Q_NULLPTR));
        label_7->setText(QApplication::translate("server", "\320\237\320\276\320\264\320\272\320\273\321\216\321\207\321\221\320\275\320\275\321\213\320\265 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260:", Q_NULLPTR));
        label_IP->setText(QApplication::translate("server", "\320\222\320\260\321\210 \320\262\320\275\321\203\321\202\321\200.  IP", Q_NULLPTR));
        le_locIP->setText(QString());
        pbSend->setText(QApplication::translate("server", "\320\236\321\202\320\277\321\200\320\260\320\262\320\270\321\202\321\214", Q_NULLPTR));
        leNick->setText(QString());
        leNick->setPlaceholderText(QApplication::translate("server", "\320\235\320\270\320\272\320\275\320\265\320\271\320\274", Q_NULLPTR));
        leMessage->setText(QString());
        leMessage->setPlaceholderText(QApplication::translate("server", "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \321\201\320\276\320\276\320\261\321\211\320\265\320\275\320\270\320\265", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class server: public Ui_server {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVER_H
