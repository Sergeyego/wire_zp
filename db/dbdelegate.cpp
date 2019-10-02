#include "dbdelegate.h"
#include <QLineEdit>

DbDelegate::DbDelegate(QObject *parent)
               : QItemDelegate(parent)
{

}

QWidget *DbDelegate::createEditor (QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{

    const DbTableModel *sqlModel = qobject_cast<const DbTableModel *>(index.model());
    if (!sqlModel) return QItemDelegate::createEditor(parent, option, index);
    QWidget *editor=NULL;
    if (sqlModel->relation(index.column())){
        QAbstractItemModel *childModel=sqlModel->relation(index.column())->model();
        if (!childModel){
            editor=QItemDelegate::createEditor(parent, option, index);
        } else {
            QComboBox *combo = new QComboBox(parent);
            combo->setLineEdit(new ComboLineEdit(parent));
            editor=combo;
        }
    } else {
        switch (sqlModel->columnType(index.column())){
            case TYPE_BOOL:
            {
                editor=NULL;
                break;
            }
            case TYPE_INTBOOL:
            {
                editor=NULL;
                break;
            }
            case TYPE_STRING:
            {
                editor=new QLineEdit(parent);
                break;
            }
            case TYPE_INT:
            {
                editor=new QLineEdit(parent);
                break;
            }
            case TYPE_DOUBLE:
            {
                editor=new QLineEdit(parent);
                break;
            }
            case TYPE_DATE:
            {
                QDateEdit *dateEdit = new QDateEdit(parent);
                dateEdit->setCalendarPopup(true);
                QCalendarWidget * pCW = new QCalendarWidget(parent);
                pCW->setFirstDayOfWeek( Qt::Monday );
                dateEdit->setCalendarWidget( pCW );
                editor=dateEdit;
                break;
            }
            default:
            {
                editor=QItemDelegate::createEditor(parent, option, index);
                break;
            }
        }
    }
    if (editor) editor->installEventFilter(const_cast<DbDelegate *>(this));
    return editor;
}

void DbDelegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const
{
    const DbTableModel *sqlModel = qobject_cast<const DbTableModel *>(index.model());
    if (sqlModel && sqlModel->relation(index.column())){
        QComboBox *combo = qobject_cast<QComboBox *>(editor);
        if (combo) {
            QAbstractItemModel *childModel=sqlModel->relation(index.column())->model();
            if (childModel){
                combo->setModel(sqlModel->relation(index.column())->proxyModel());
                combo->setModelColumn(sqlModel->relation(index.column())->columnDisplay());
                combo->setEditable(true);
                CustomCompletter *c = new CustomCompletter(combo);
                c->setModel(sqlModel->relation(index.column())->proxyModel());
                c->setCompletionColumn(sqlModel->relation(index.column())->columnDisplay());
                combo->setCompleter(c);
                int pos=combo->findText(sqlModel->data(index).toString());
                if (pos!=-1){
                    combo->setCurrentIndex(pos);
                } else {
                    combo->lineEdit()->setText(sqlModel->data(index).toString());
                }
                return;
            }
        } else {
            QLineEdit *le = qobject_cast<QLineEdit *>(editor);
            if (le){
                le->setText(sqlModel->data(index,Qt::DisplayRole).toString());
                return;
            }
        }
    }
    if (sqlModel && sqlModel->validator(index.column())){
        QLineEdit *line = qobject_cast<QLineEdit *>(editor);
        if (line) line->setValidator(sqlModel->validator(index.column()));
    }
    return QItemDelegate::setEditorData(editor, index);
}

void DbDelegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
    if (!index.isValid())
        return;
    DbTableModel *sqlModel = qobject_cast<DbTableModel *>(model);
    if (sqlModel && sqlModel->relation(index.column())){
        QComboBox *combo = qobject_cast<QComboBox *>(editor);
        if (combo) {
            combo->setCurrentIndex(combo->findText(combo->currentText()));
            QVariant v=combo->model()->data(combo->model()->index(combo->currentIndex(),sqlModel->relation(index.column())->columnKey()),Qt::EditRole);
            QVariant val = v.isNull() ? QVariant() : v;
            sqlModel->setData(index,val,Qt::EditRole);
        } else {
            return;
            //return QItemDelegate::setModelData(editor, model, index);
        }
    } else {
        return QItemDelegate::setModelData(editor, model, index);
    }
}

void DbDelegate::updateEditorGeometry(
            QWidget *editor,
            const QStyleOptionViewItem &option,
            const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}

bool DbDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type()== QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QLineEdit *line = qobject_cast<QLineEdit *>(object);
        if (line && line->validator()) {
            const QDoubleValidator *val = qobject_cast< const QDoubleValidator *>(line->validator());
            if (val){
                if (keyEvent->text()==",") {
                    line->insert(".");
                    return true;
                }
            }
        }

        if(keyEvent->key()==Qt::Key_Tab || keyEvent->key()==Qt::Key_Down || keyEvent->key()==Qt::Key_Up){
            //qDebug()<<"Key_Tab";
            QWidget *editor = qobject_cast<QWidget*>(object);
            emit commitData(editor);
            emit closeEditor(editor);
            return false;
        }
    }
    return QItemDelegate::eventFilter(object,event);
}

CustomCompletter::CustomCompletter(QObject *parent):QCompleter(parent)
{
    setCompletionMode(QCompleter::PopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);
}

bool CustomCompletter::eventFilter(QObject *o, QEvent *e)
{
    if (e->type()==QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        if (keyEvent->key()==Qt::Key_Tab) {
            this->popup()->close();
            return false;
        }
    }
    return QCompleter::eventFilter(o,e);
}

ComboLineEdit::ComboLineEdit(QWidget *parent) : QLineEdit(parent)
{

}

void ComboLineEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->text()==",") {
        insert(".");
        return;
    } else {
        return QLineEdit::keyPressEvent(e);
    }
}
