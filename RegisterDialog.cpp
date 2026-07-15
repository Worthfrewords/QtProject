#include "RegisterDialog.h"
#include "ui_RegisterDialog.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDebug>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog) {
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &RegisterDialog::onRegister);
}

RegisterDialog::~RegisterDialog() {
    delete ui;
}

void RegisterDialog::onRegister() {
    QString username = ui->editUsername->text().trimmed();
    QString password = ui->editPassword->text().trimmed();
    QString confirm = ui->editPasswordConfirm->text().trimmed();

    if (username.isEmpty() || password.isEmpty() || confirm.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "所有字段都不能为空！");
        return;
    }
    if (password != confirm) {
        QMessageBox::warning(this, "密码错误", "两次输入的密码不一致！");
        return;
    }
    if (password.length() < 6) {
        QMessageBox::warning(this, "密码太短", "密码长度至少为 6 位！");
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "数据库错误", "数据库未连接！");
        return;
    }

    // 检查用户名是否已存在
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM users WHERE username = ?");
    checkQuery.addBindValue(username);
    if (!checkQuery.exec()) {
        QMessageBox::critical(this, "查询错误", checkQuery.lastError().text());
        return;
    }
    if (checkQuery.next()) {
        QMessageBox::warning(this, "用户名已存在", "该用户名已被注册，请换一个。");
        ui->editUsername->clear();
        ui->editUsername->setFocus();
        return;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
    insertQuery.addBindValue(username);
    QString hashedPwd = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
    insertQuery.addBindValue(hashedPwd);
    insertQuery.addBindValue("user");

    if (!insertQuery.exec()) {
        QMessageBox::critical(this, "注册失败", insertQuery.lastError().text());
        return;
    }

    QMessageBox::information(this, "注册成功", "账号注册成功！请返回登录。");
    accept(); // 关闭注册对话框
}
