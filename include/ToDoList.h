#pragma once

#include <QtWidgets/QMainWindow>
#include <QPushButton>

#include "ui_ToDoList.h"
#include "ProxyModel.h"

class Model;

class ToDoList : public QMainWindow
{
    Q_OBJECT

public:
    ToDoList(QWidget *parent = nullptr);
    ~ToDoList();

signals:
	void updateGUI() const;

private slots:
	void onAddTask();
	void addTask(const QString& name, const QString& description, const QString& date, const QString& state);
	void onDeleteTask(int logicalIndex);
	void onAddFilter();
	void onRemoveFilter(QVariant data);

	void onUpdateGUI();
	void onFieldIndexChange();

	void fillTable();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void setUpGUI();
	void addFilter(QString fieldName, QString fieldTag, QString value);
	void filter();
	void populateValues();
	QStringList sortCaseInsensitive(const QStringList& list) const;
	void enableFilterWidgets(const bool enable);
	void save() const;
	void load();

private:
    Ui::ToDoListClass ui;

	Model* m_model;
	ProxyModel* m_proxyModel;
	Criteries m_filterParameters;
};

/**
	\class QPushButton that has an associated data
*/
class PushButton : public QPushButton
{
	Q_OBJECT
public:
	PushButton(const QString& text, QVariant data) :QPushButton(text), m_data(data)
	{
		connect(this, SIGNAL(clicked()), this, SLOT(clicked()));
	}
signals:
	void clicked(QVariant);
private slots:
	void clicked() { emit clicked(m_data); }
private:
	QVariant m_data;
};
