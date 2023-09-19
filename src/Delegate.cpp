#include "Delegate.h"
#include "ProxyModel.h"
#include "Model.h"

#include <QComboBox>
#include <QDateEdit>

Delegate::Delegate(QObject* parent)
	: QStyledItemDelegate(parent)
{
}

Delegate::~Delegate()
{
}

QWidget* Delegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(option);

	switch (index.column())
	{
	case 2:
		return createDateEditor(parent, index);
	case 3:
		return createStateEditor(parent);
	default:
		return nullptr;
	}
}

void Delegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	const ProxyModel* model = static_cast<const ProxyModel*>(index.model());
	const QVariant data = model->data(index, Qt::EditRole);

	switch (index.column())
	{
	case 2:
		setDateEditorData(editor, data);
		break;
	case 3:
		setStateEditorData(editor, data);
		break;
	default:
		break;
	}
}

void Delegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	Q_UNUSED(model);

	ProxyModel* myModel = const_cast<ProxyModel*>(static_cast<const ProxyModel*>(index.model()));
	assert(myModel);

	QVariant data;
	switch (index.column())
	{
	case 2:
		getDateData(editor, data);
		break;
	case 3:
		getStateData(editor, data);
		break;
	default:
		break;
	}

	QVariant currentValue = myModel->data(index, Qt::EditRole);

	if (currentValue != data)
	{
		myModel->setData(index, data, Qt::EditRole);
	}
}

QWidget* Delegate::createDateEditor(QWidget* parent, const QModelIndex& index) const
{
	const ProxyModel* model = static_cast<const ProxyModel*>(index.model());
	const QVariant currentDate = model->data(index, Qt::EditRole);

	QDateEdit* dateEditor = new QDateEdit(QDate::fromString(currentDate.toString(), "dd/MM/yyyy"), parent);
	dateEditor->setDisplayFormat("dd/MM/yyyy");
	dateEditor->setMinimumDate(QDate::currentDate());
	dateEditor->setCalendarPopup(true);

	return dateEditor;
}

void Delegate::getDateData(QWidget* editor, QVariant& data) const
{
	QDateEdit* dateEditor = dynamic_cast<QDateEdit*>(editor);
	assert(dateEditor);
	data = dateEditor->date().toString("dd/MM/yyyy");
}

void Delegate::setDateEditorData(QWidget* editor, const QVariant& data) const
{
	QDateEdit* dateEditor = dynamic_cast<QDateEdit*>(editor);
	assert(dateEditor);

	dateEditor->setDate(data.toDate());

	connect(dateEditor, SIGNAL(currentIndexChanged(int)),
		this, SLOT(commitAndCloseEditor()));
}

QWidget* Delegate::createStateEditor(QWidget* parent) const
{
	QComboBox* comboBox = new QComboBox(parent);

	comboBox->addItem(tr("In progress"));
	comboBox->addItem(tr("Done"));

	return comboBox;
}

void Delegate::getStateData(QWidget* editor, QVariant& data) const
{
	QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
	assert(comboBox);
	data = comboBox->itemText(comboBox->currentIndex());
}

void Delegate::setStateEditorData(QWidget* editor, const QVariant& data) const
{
	QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
	assert(comboBox);

	comboBox->setCurrentIndex(comboBox->findText(data.toString()));

	connect(comboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(commitAndCloseEditor()));
}
