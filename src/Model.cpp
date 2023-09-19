#include "Model.h"

Model::Model(QObject* parent)
    : QAbstractTableModel(parent)
{
	m_deleteIcon = QIcon(":/ToDoList/delete.png");
}

int Model::rowCount(const QModelIndex&/*parent*/) const
{
    return m_rowList.size();
}

int Model::columnCount(const QModelIndex&/*parent*/) const
{
	return 4;
}

QVariant Model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole)
		return m_rowList[index.row()][index.column()];
    else
        return QVariant();
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && (role == Qt::DisplayRole || role == Qt::EditRole))
	{
		switch (section)
		{
        case 0: return role == Qt::DisplayRole ? QString(tr("Name")) : "name";
		case 1: return role == Qt::DisplayRole ? QString(tr("Description")) : "description";
		case 2: return role == Qt::DisplayRole ? QString(tr("Date")) : "date";
		case 3: return role == Qt::DisplayRole ? QString(tr("State")) : "state";
		default: return QVariant();
		}
	}
	else if (orientation == Qt::Vertical && role == Qt::DecorationRole)
	{
		return m_deleteIcon;
	}
	return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool Model::setData(const QModelIndex& index,
    const QVariant& value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    m_rowList[index.row()][index.column()] = value;
   
    emit dataChanged(index, index);
    return true;
}

bool Model::insertRows(int position, int rows, const QModelIndex& parent)
{
    int columns = columnCount();
    beginInsertRows(parent, position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
		QList<QVariant> items;
		for (int column = 0; column < columns; ++column)
		{
			items.append(QVariant());
		}
        m_rowList.insert(position, items);
    }

    endInsertRows();
    return true;
}

bool Model::removeRows(int position, int rows, const QModelIndex& parent)
{
    beginRemoveRows(parent, position, position + rows - 1);

    for (int row = 0; row < rows; ++row) {
        m_rowList.removeAt(position);
    }

    endRemoveRows();
    return true;
}