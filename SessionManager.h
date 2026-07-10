#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QString>
//维护登录用户状态
class SessionManager {
public:
    //获取单例对象
    static SessionManager& getInstance();
    //会话操作
    void login(int userId, const QString& username, const QString& role);
    void logout();
    //查询接口(简单部分直接实现为内联函数)
    int getUserId() const { return m_userId; }
    QString getUsername() const;
    QString getRole() const { return m_role; }
    bool isAdmin() const { return m_role == "admin"; }
    bool isLoggedIn() const { return m_userId > 0; }

private:
    SessionManager() = default;
    ~SessionManager() = default;

    int m_userId = 0;
    QString m_username;
    QString m_role;
};

#endif // SESSIONMANAGER_H
