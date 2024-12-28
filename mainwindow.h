#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <map>
#include <sstream>
#include "TextProcessor.h"
#include "Tests.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_processButton_clicked();
    void on_runTestsButton_clicked();

private:
    TextProcessor processor;
    void displayIndex(const TextIndex& index);
    void displayTestResults(const std::ostringstream& results);

    QTextEdit *textEdit;
    QSpinBox *pageSizeSpinBox;
    QPushButton *processButton;
    QPushButton *runTestsButton;
    QTextEdit *resultTextEdit;
};

#endif // MAINWINDOW_H
