#include "ProxyModel.h"
#include "Model.h"

#include <QDate>

QString ProxyModel::VALUE_SEPARATOR = ", ";

ProxyModel::ProxyModel(QObject* parent)
	: QSortFilterProxyModel(parent)
	, m_pFilterParameters(nullptr)
{
}

bool ProxyModel::filterAcceptsRow(int sourceRow,
	const QModelIndex& sourceParent) const
{
	if (m_pFilterParameters)
	{
		Model* source = static_cast<Model*>(sourceModel());
		assert(nullptr != source);

		FilterCriterion* filterParameter;
		for (int i = 0; i < m_pFilterParameters->size(); i++)
		{
			filterParameter = m_pFilterParameters->at(i);
			QString fieldTag = filterParameter->fieldTag;
			QString values = filterParameter->value;
			QStringList valueList = values.split(VALUE_SEPARATOR);

			QModelIndex itemForField;
			if (fieldTag == "name")
			{
				itemForField = source->index(sourceRow, 0, sourceParent);
			}
			else if (fieldTag == "description")
			{
				itemForField = source->index(sourceRow, 1, sourceParent);
			}
			else if (fieldTag == "date")
			{
				itemForField = source->index(sourceRow, 2, sourceParent);
			}
			else if (fieldTag == "state")
			{
				itemForField = source->index(sourceRow, 3, sourceParent);
			}

			if (itemForField.isValid())
			{
				QString currentValue = source->data(itemForField, Qt::EditRole).toString();

				bool found = false;
				foreach(auto value, valueList)
				{
					if (fieldTag == "date")
					{
						QDate from = QDate::fromString(value.left(10), "dd/MM/yyyy");
						QDate to = QDate::fromString(value.right(10), "dd/MM/yyyy");
						QDate currentDate = QDate::fromString(currentValue, "dd/MM/yyyy");

						if(currentDate >= from && currentDate <= to)
						{
							found = true;
							break;
						}
					}
					else if (currentValue == value)
					{
						found = true;
						break;
					}
				}
				if (found)
				{
					continue;
				}
				else
				{
					return false;
				}
			}
		}
	}

	return true;
}

ProxyModel::~ProxyModel()
{
}

void ProxyModel::setFilterParameters(Criteries* pFilterParameters)
{
	m_pFilterParameters = pFilterParameters;
}