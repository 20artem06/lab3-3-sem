#include "mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    textEdit = new QTextEdit(this);
    pageSizeSpinBox = new QSpinBox(this);
    processButton = new QPushButton("Обработать", this);
    runTestsButton = new QPushButton("Запустить тесты", this);
    resultTextEdit = new QTextEdit(this);


    resultTextEdit->setReadOnly(true);


    this->setStyleSheet("background-color: #f0fff0;"); // светлый фон для всего окна
    textEdit->setStyleSheet("background-color: #e6ffe6; color: black; border: 1px solid #90ee90;"); // светло-зеленый фон
    pageSizeSpinBox->setStyleSheet("background-color: #e6ffe6; color: black; border: 1px solid #90ee90;");
    processButton->setStyleSheet("background-color: #90ee90; color: black; border: 1px solid #6fa05c;");
    runTestsButton->setStyleSheet("background-color: #90ee90; color: black; border: 1px solid #6fa05c;");
    resultTextEdit->setStyleSheet("background-color: #e6ffe6; color: black; border: 1px solid #90ee90;");


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Введите текст:"));
    layout->addWidget(textEdit);
    layout->addWidget(new QLabel("Размер страницы (в символах):"));
    layout->addWidget(pageSizeSpinBox);
    layout->addWidget(processButton);
    layout->addWidget(runTestsButton);
    layout->addWidget(new QLabel("Результаты:"));
    layout->addWidget(resultTextEdit);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);


    connect(processButton, &QPushButton::clicked, this, &MainWindow::on_processButton_clicked);
    connect(runTestsButton, &QPushButton::clicked, this, &MainWindow::on_runTestsButton_clicked);
}

MainWindow::~MainWindow() {
}

void MainWindow::on_processButton_clicked() {
    QString inputText = textEdit->toPlainText();
    int pageSize = pageSizeSpinBox->value();

    if (inputText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст для обработки.");
        return;
    }

    TextIndex index = processor.createTextIndex(inputText.toStdString(), pageSize);
    displayIndex(index);
}

void MainWindow::displayIndex(const TextIndex& index) {
    resultTextEdit->clear();

    QString result;
    for (const auto& word : index.getWords()) {
        result += QString::fromStdString(word) + " - ";

        auto locations = index.getWordLocations(word);
        for(const auto& loc : locations){
            result += QString("стр. ") + QString::number(loc.page) + ", поз. " + QString::number(loc.position) + " ";
        }
        result += "\n";
    }
    resultTextEdit->setPlainText(result);
}


void MainWindow::on_runTestsButton_clicked() {
    Tests tests;
    std::ostringstream resultStream;

    tests.runAllTests(resultStream);
    displayTestResults(resultStream);
}

void MainWindow::displayTestResults(const std::ostringstream& results) {
    resultTextEdit->clear();
    resultTextEdit->setPlainText(QString::fromStdString(results.str()));
}
