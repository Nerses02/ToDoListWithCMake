#pragma once

#include <QSortFilterProxyModel>

struct FilterCriterion
{
	QString fieldTag;
	QString value;
	QWidget* widget;
};

typedef QList<FilterCriterion*> Criteries;

class ProxyModel : public QSortFilterProxyModel
{
public:

	explicit ProxyModel(QObject* parent = nullptr);
	~ProxyModel() override;

	void setFilterParameters(Criteries* pFilterParameters);

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public:
	static QString VALUE_SEPARATOR;

private:
	
	Criteries* m_pFilterParameters;
};