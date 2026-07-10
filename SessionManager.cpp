#include "SessionManager.h"
#include <QDebug>

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;  // 第一次调用时构造，程序结束时销毁
    return instance;
}

//用户登录成功时设置状态,由LoginDialog在验证账号密码通过后调用
void SessionManager::login(int userId, const QString& username, const QString& role) {
    m_userId = userId;
    m_username = username;
    m_role = role;
    qDebug() << "[Session] 登录成功 -> ID:" << userId
             << ", 用户:" << username
             << ", 角色:" << role;
}

//用户登出
void SessionManager::logout() {
    m_userId = 0;
    m_username.clear();
    m_role.clear();

    qDebug() << "[Session] 用户已登出，会话已清空";
}
//避免重复定义函数,仅在头文件中声明
QString SessionManager::getUsername() const {
    return m_username;
}
