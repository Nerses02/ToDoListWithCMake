#pragma once

#include <QStyledItemDelegate>

class Delegate : public QStyledItemDelegate
{
public:
	Delegate(QObject* parent = nullptr);
	~Delegate() override;

public: // overrides from QStyledItemDelegate
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

private:
	QWidget* createDateEditor(QWidget* parent, const QModelIndex& index)const;
	void getDateData(QWidget* editor, QVariant& data) const;
	void setDateEditorData(QWidget* editor, const QVariant& data) const;

	QWidget* createStateEditor(QWidget* parent)const;
	void getStateData(QWidget* editor, QVariant& data) const;
	void setStateEditorData(QWidget* editor, const QVariant& data) const;
};