#include "ToDoList.h"
#include "Model.h"
#include "ProxyModel.h"
#include "Delegate.h"

#include <QMessageBox>
#include <QCompleter>
#include <QCloseEvent>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

ToDoList::ToDoList(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	setWindowIcon(QIcon(":/ToDoList/ToDoList.png"));


	m_model = new Model(this);
	m_proxyModel = new ProxyModel(this);
	Delegate* del = new Delegate(this);
	m_filterParameters.clear();
	m_proxyModel->setFilterParameters(&m_filterParameters);
	m_proxyModel->setSourceModel(m_model);
	ui.tasksTable->setModel(m_proxyModel);
	ui.tasksTable->setItemDelegateForColumn(2, del);
	ui.tasksTable->setItemDelegateForColumn(3, del);
	ui.tasksTable->setWordWrap(true);

	load();

	setUpGUI();
}

ToDoList::~ToDoList()
{}

void ToDoList::onAddTask()
{
	ui.tasksTable->setUpdatesEnabled(false);

	addTask(
		ui.lineEditName->text(),
		ui.lineEditDescription->text(),
		ui.dateEdit->date().toString("dd/MM/yyyy"),
		tr("In progress")
	);

	ui.tasksTable->setUpdatesEnabled(true);
	ui.tasksTable->update();

	fillTable();
}

void ToDoList::addTask(const QString& name, const QString& description, const QString& date, const QString& state)
{
	int row = m_model->rowCount();
	m_model->insertRows(row, 1);
	m_model->setData(m_model->index(row, 0), name);
	m_model->setData(m_model->index(row, 1), description);
	m_model->setData(m_model->index(row, 2), date);
	m_model->setData(m_model->index(row, 3), state);
}

void ToDoList::onDeleteTask(int logicalIndex)
{
	if (logicalIndex < 0)
		return;

	ui.tasksTable->setUpdatesEnabled(false);
	m_model->removeRows(logicalIndex, 1);
	ui.tasksTable->setUpdatesEnabled(true);
	ui.tasksTable->update();

	fillTable();
}

void ToDoList::onAddFilter()
{
	assert(NULL != ui.avalilableFilterCombo);
	QString fieldTag = ui.avalilableFilterCombo->currentData().toString();

	QString value;
	if (fieldTag == "date")
	{
		QDate dateFrom = ui.dateEditFrom->date();
		QDate dateTo = ui.dateEditTo->date();

		value = dateFrom.toString("dd/MM/yyyy");
		if(dateFrom != dateTo)
			value += " - " + dateTo.toString("dd/MM/yyyy");
	}
	else
	{
		value = ui.valuesCombo->currentText();
	}
	
	if (value.isEmpty()) return;
	addFilter(ui.avalilableFilterCombo->currentText(), fieldTag, value);
	filter();

	ui.avalilableFilterCombo->setCurrentIndex(ui.avalilableFilterCombo->findData(fieldTag));
}

void ToDoList::onRemoveFilter(QVariant data)
{
	void* dataPtr = data.value<void*>();
	FilterCriterion* parameter = static_cast<FilterCriterion*>(dataPtr);
	ui.filterWidgetsContainerLayout->removeWidget(parameter->widget);

	delete parameter->widget;

	m_filterParameters.removeOne(parameter);

	filter();

	if (m_filterParameters.size() == 3)
	{
		ui.filterMainLayout->insertWidget(1, ui.scrollArea->takeWidget());
		ui.scrollArea->hide();
	}
}

void ToDoList::filter()
{
	m_proxyModel->invalidate();
	bool containsMatchedItems = m_proxyModel->rowCount() > 0;
	enableFilterWidgets(containsMatchedItems);

	ui.noMatchLabel->setVisible((!containsMatchedItems && !m_filterParameters.isEmpty()));
}

void ToDoList::fillTable()
{
	filter();
	populateValues();
}

void ToDoList::populateValues()
{
	if (ui.avalilableFilterCombo->currentData() == "date")
	{
		QDate currentMinimumDate;
		QDate currentMaximumDate;
		for (int i = 0; i < m_model->rowCount(); i++)
		{
			for (int j = 0; j < m_model->columnCount(); j++)
			{
				if (j == 2)
				{
					QDate tempDate = QDate::fromString(m_model->data(m_model->index(i, j, QModelIndex()), Qt::EditRole).toString(), "dd/MM/yyyy");
					if (currentMinimumDate.isNull())
						currentMinimumDate = tempDate;
					else if (tempDate < currentMinimumDate)
						currentMinimumDate = tempDate;

					if (currentMaximumDate.isNull())
						currentMaximumDate = tempDate;
					else if (tempDate > currentMaximumDate)
						currentMaximumDate = tempDate;
				}
			}
		}

		ui.dateEditFrom->setMinimumDate(currentMinimumDate);
		ui.dateEditFrom->setMaximumDate(currentMaximumDate);
		ui.dateEditFrom->setDate(currentMinimumDate);

		ui.dateEditTo->setMinimumDate(currentMinimumDate);
		ui.dateEditTo->setMaximumDate(currentMaximumDate);
		ui.dateEditTo->setDate(currentMaximumDate);
	}
	else
	{
		int fildIndex = ui.avalilableFilterCombo->currentIndex();
		QStringList values;
		for (int i = 0; i < m_model->rowCount(); i++)
		{
			for (int j = 0; j < m_model->columnCount(); j++)
			{
				if (fildIndex == j)
					values << m_model->data(m_model->index(i, j, QModelIndex()), Qt::EditRole).toString();
			}
		}

		ui.valuesCombo->clear();
		ui.valuesCombo->addItems(sortCaseInsensitive(values));
		ui.valuesCombo->setEditable(true);
		ui.valuesCombo->clearEditText();
		ui.valuesCombo->completer()->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
		ui.valuesCombo->setFocus();
	}
}

QStringList ToDoList::sortCaseInsensitive(const QStringList& list) const
{
	QMap<QString, QString> sortedList;
	foreach(QString str, list) sortedList.insert(str.toLower(), str);
	return sortedList.values();
}

void ToDoList::enableFilterWidgets(const bool enable)
{
	ui.addFilterBttn->setEnabled(enable);
	ui.dateEditFrom->setEnabled(enable);
	ui.dateEditTo->setEnabled(enable);
	ui.valuesCombo->setEnabled(enable);
	ui.filterValueshorizontalLayout->setEnabled(enable);
	ui.avalilableFilterCombo->setEnabled(enable);
}

void ToDoList::save() const
{
	QFile xmlFile("ToDoList.xml");
	if (!xmlFile.open(QIODevice::WriteOnly))
	{
		return;
	}

	QXmlStreamWriter stream(&xmlFile);
	stream.setAutoFormatting(true);
	stream.setAutoFormattingIndent(3);
	stream.writeStartDocument("1.0");
	stream.writeStartElement("ToDoList");

	for (int i = 0; i < m_model->rowCount(); i++)
	{
		stream.writeStartElement("row");
		for (int j = 0; j < m_model->columnCount(); j++)
		{
			stream.writeAttribute(m_model->headerData(j, Qt::Horizontal, Qt::EditRole).toString(), m_model->data(m_model->index(i, j, QModelIndex()), Qt::EditRole).toString());
		}
		stream.writeEndElement(); //row
	}

	stream.writeEndElement(); //ToDoList
	stream.writeEndDocument();

	xmlFile.close();
}

void ToDoList::load()
{
	QFile xmlFile("ToDoList.xml");
	if(!xmlFile.open(QIODevice::ReadOnly))
	{
		return;
	}
	QXmlStreamReader reader(&xmlFile);

	reader.readNextStartElement();
	if (reader.name().toString() == "ToDoList")
	{
		ui.tasksTable->setUpdatesEnabled(false);

		while (reader.readNextStartElement())
		{
			if (reader.name().toString() == "row") {
				addTask(
					reader.attributes().value("name").toString(),
					reader.attributes().value("description").toString(),
					reader.attributes().value("date").toString(),
					reader.attributes().value("state").toString()
				);
			}
			reader.readNext();
		}

		ui.tasksTable->setUpdatesEnabled(true);
		ui.tasksTable->update();

		fillTable();
	}
}

void ToDoList::onFieldIndexChange()
{
	if (ui.avalilableFilterCombo->currentData() == "date")
	{
		ui.valuesCombo->setVisible(false);
		ui.dateFilterValuesWidget->setVisible(true);
	}
	else
	{
		ui.dateFilterValuesWidget->setVisible(false);
		ui.valuesCombo->setVisible(true);
	}

	populateValues();
}

void ToDoList::onUpdateGUI()
{
	ui.addTaskBttn->setEnabled(!ui.lineEditName->text().isEmpty());

	if (ui.avalilableFilterCombo->currentData() == "date")
		ui.addFilterBttn->setEnabled(ui.dateEditFrom->date() <= ui.dateEditTo->date());
	else
		ui.addFilterBttn->setEnabled(!ui.valuesCombo->currentText().isEmpty());
}

void ToDoList::setUpGUI()
{
	ui.dateEdit->setMinimumDate(QDate::currentDate());
	ui.dateEdit->setDate(QDate::currentDate());

	ui.scrollArea->setMaximumHeight(this->height() * 0.4);
	ui.scrollArea->hide();
	ui.avalilableFilterCombo->addItem(tr("Name"), "name");
	ui.avalilableFilterCombo->addItem(tr("Description"), "description");
	ui.avalilableFilterCombo->addItem(tr("Date"), "date");
	ui.avalilableFilterCombo->addItem(tr("State"), "state");
	ui.avalilableFilterCombo->setCurrentIndex(0);
	ui.dateEditFrom->setCalendarPopup(true);
	ui.dateEditTo->setCalendarPopup(true);
	ui.noMatchLabel->setVisible(false);
	onFieldIndexChange();
	enableFilterWidgets(m_proxyModel->rowCount() > 0);

	ui.tasksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	
	connect(ui.addTaskBttn, &QPushButton::clicked, this, &ToDoList::onAddTask);
	connect(ui.addFilterBttn, &QPushButton::clicked, this, &ToDoList::onAddFilter);
	connect(ui.lineEditName, &QLineEdit::textChanged, this, &ToDoList::onUpdateGUI);
	connect(ui.valuesCombo, &QComboBox::currentTextChanged, this, &ToDoList::onUpdateGUI);
	connect(ui.dateEditFrom, &QDateEdit::userDateChanged, this, &ToDoList::onUpdateGUI);
	connect(ui.dateEditTo, &QDateEdit::userDateChanged, this, &ToDoList::onUpdateGUI);
	connect(ui.avalilableFilterCombo, &QComboBox::currentIndexChanged, this, &ToDoList::onFieldIndexChange);
	connect(m_model, &Model::dataChanged, this, &ToDoList::fillTable);
	connect(ui.tasksTable->verticalHeader(), SIGNAL(sectionPressed(int)), this, SLOT(onDeleteTask(int)));
}

void ToDoList::addFilter(QString fieldName, QString fieldTag, QString value)
{
	FilterCriterion* parameter = 0;

	foreach(FilterCriterion * param, m_filterParameters)
	{
		if (param->fieldTag == fieldTag)
		{
			parameter = param;
			if (parameter->value.split(ProxyModel::VALUE_SEPARATOR).contains(value))
			{
				QMessageBox::information(this, tr("Filter could not be added"), tr("This filter value is already applied."));
				return;
			}
			parameter->value += ProxyModel::VALUE_SEPARATOR;

			ui.filterWidgetsContainerLayout->removeWidget(parameter->widget);
			delete parameter->widget;

			break;
		}
	}

	if (!parameter)
	{
		parameter = new FilterCriterion;
		parameter->fieldTag = fieldTag;
		m_filterParameters.append(parameter);
	}

	parameter->value += value;
	parameter->widget = new QWidget();

	QVariant variant;
	variant.setValue(static_cast<void*>(parameter));

	PushButton* pushButton = new PushButton(QString(tr("Remove")), variant);
	connect(pushButton, SIGNAL(clicked(QVariant)), this, SLOT(onRemoveFilter(QVariant)));

	QHBoxLayout* hbl = new QHBoxLayout;
	QLabel* fieldLabelWidget = new QLabel(fieldName);
	QLabel* valueLabelWidget = new QLabel(parameter->value);
	fieldLabelWidget->setWordWrap(true);
	valueLabelWidget->setWordWrap(true);
	valueLabelWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	hbl->addWidget(fieldLabelWidget, 2);
	hbl->addWidget(valueLabelWidget, 8);
	hbl->addWidget(pushButton);
	hbl->setContentsMargins(0, 0, 0, 0);
	parameter->widget->setLayout(hbl);

	parameter->widget->setMinimumHeight(20);
	ui.filterWidgetsContainerLayout->addWidget(parameter->widget);
	if (m_filterParameters.size() == 4)
	{
		ui.scrollArea->setWidget(ui.filterWidgetsContainerWidget);
		ui.scrollArea->show();
	}
}

void ToDoList::closeEvent(QCloseEvent* event)
{
	save();
	event->accept();
}