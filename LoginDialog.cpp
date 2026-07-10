#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "DatabaseManager.h"
#include "SessionManager.h"
#include "StudentTableModel.h"
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlError>
#include <QDialog>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowTitleHint); // 固定大小

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LoginDialog::onLogin);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

LoginDialog::~LoginDialog() {
    delete ui;
}

void LoginDialog::onLogin() {
    QString username = ui->editUsername->text().trimmed();
    QString password = ui->editPassword->text().trimmed();
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "用户名和密码不能为空！");
        return;
    }

    // SHA256 哈希
    QByteArray pwdHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlDatabase db = DatabaseManager::getInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "数据库错误", "数据库未连接！");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, username, role FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(QString::fromUtf8(pwdHash));

    if (!query.exec()) {
        QMessageBox::critical(this, "查询错误", query.lastError().text());
        return;
    }

    if (query.next()) {
        int userId = query.value(0).toInt();
        QString userName = query.value(1).toString();
        QString role = query.value(2).toString();
        SessionManager::getInstance().login(userId, userName, role);
        accept();  // 关闭对话框并返回 Accepted
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误！");
        ui->editPassword->clear();
        ui->editPassword->setFocus();
    }
}
