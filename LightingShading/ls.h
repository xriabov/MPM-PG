#ifndef LS_H
#define LS_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class LS; }
QT_END_NAMESPACE

class LS : public QMainWindow
{
    Q_OBJECT

public:
    LS(QWidget *parent = nullptr);
    ~LS();

private:
    Ui::LS *ui;
};
#endif // LS_H
