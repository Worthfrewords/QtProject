#include "StudentTableModel.h"
#include "StudentService.h"
#include <QDebug>

StudentTableModel::StudentTableModel(QObject *parent)
    : QAbstractTableModel(parent) {
}

void StudentTableModel::setDataList(const QList<StudentEntity>& data) {
    beginResetModel();   // 通知视图：即将全部刷新
    m_data = data;
    endResetModel();     // 通知视图：刷新完成
}

int StudentTableModel::rowCount(const QModelIndex&) const {
    return m_data.size();
}
int StudentTableModel::columnCount(const QModelIndex&) const {
    return m_headers.size();
}

// 获取数据：根据角色返回不同内容
QVariant StudentTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_data.size()) {
        return QVariant();
    }

    const StudentEntity& s = m_data.at(index.row());

    // 显示/编辑角色
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0: return s.studentNo;
        case 1: return s.name;
        case 2: return (s.gender == 'M') ? "男" : "女";
        case 3: return QDate::currentDate().year() - s.birthday.year();
        case 4: return s.className;
        default: return QVariant();
        }
    }

    // 用户自定义角色：返回学生ID，用于删除等操作
    if (role == Qt::UserRole) {
        return s.id;
    }
    //所有列居中对齐
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }
    return {};
}

// 获取表头数据
QVariant StudentTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section >= 0 && section < m_headers.size()) {
            return m_headers.at(section);
        }
    }
    return QVariant();
}

// 编辑数据（双击修改单元格）
bool StudentTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    // 只处理编辑角色，且索引有效
    if (role != Qt::EditRole || !index.isValid()) return false;
    if (index.row() >= m_data.size()) return false;

    StudentEntity s = m_data.at(index.row());
    // 根据列更新对应的字段
    switch (index.column()) {
    case 0: s.studentNo = value.toString(); break;
    case 1: s.name = value.toString(); break;
    case 2: {
        QString genderStr = value.toString();
        if (genderStr == "男") s.gender = 'M';
        else if (genderStr == "女") s.gender = 'F';
        else return false;
        break;
    }
    //年龄不可编辑
    case 3: return false;
    case 4: s.className = value.toString(); break;
    default: return false;
    }

    // 调用业务层更新数据库
    if (StudentService::getInstance().updateStudent(s)) {
        //更新成功后，修改内部数据并通知视图刷新
        m_data[index.row()] = s;
        emit dataChanged(index, index, {Qt::DisplayRole});
        return true;
    } else {
        qDebug() << "StudentTableModel: 更新失败，可能无权限或数据库错误";
        return false;
    }
}

// 设置有效单元格（是否可编辑、可选中）
Qt::ItemFlags StudentTableModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    flags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    // 年龄列（第3列）不可编辑，其余列允许编辑
    if (index.column() != 3) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}
