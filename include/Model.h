#pragma once

#include <QAbstractTableModel>
#include <QVariant>
#include <QIcon>

class Model : public QAbstractTableModel
{
public:
    Model(QObject* parent = 0);

    // overrides from QAbstractTableModel
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;

private:
    QList<QList<QVariant>> m_rowList;
    QIcon m_deleteIcon;
};
