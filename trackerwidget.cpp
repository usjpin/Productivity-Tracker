#include "trackerwidget.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

#include <QtGui>
#include <QtWidgets>
#include <random>
#include <ctime>

TrackerWidget::TrackerWidget(QWidget *parent) : QWidget(parent) {
    srand(time(NULL));
    system("mkdir AppData");
    system("mkdir BackUpFiles");
    today = QDate().currentDate();
    mainDate = new QLabel(today.toString("MMMM dd, yyyy"));
    mainDate->setStyleSheet("*{font-size:22px;}");
    mainDateEdit = new QDateEdit(today);
    mainDateEdit->setFixedSize(80, 20);
    QWidget::connect(mainDateEdit, SIGNAL(dateChanged(const QDate&)), this, SLOT(changedDate(const QDate&)));
    QHBoxLayout* dateLayout = new QHBoxLayout();
    dateLayout->addWidget(mainDate);
    dateLayout->addWidget(mainDateEdit);

    prev = today;
    slotAvailable = false;

    QLabel* wakeLabel = new QLabel(QString("Ideal Wake Up Time"));
    wakeLine = new QLineEdit();
    QWidget::connect(wakeLine, SIGNAL(textChanged(const QString&)), this, SLOT(valuesChanged()));
    QVBoxLayout* wakeLayout = new QVBoxLayout();
    wakeLayout->addWidget(wakeLabel);
    wakeLayout->addWidget(wakeLine);

    QLabel* musicLabel = new QLabel(QString("Music of the Day"));
    musicLine = new QLineEdit();
    QWidget::connect(musicLine, SIGNAL(textChanged(const QString&)), this, SLOT(valuesChanged()));
    QVBoxLayout* musicLayout = new QVBoxLayout();
    musicLayout->addWidget(musicLabel);
    musicLayout->addWidget(musicLine);

    QVBoxLayout* lineEditLayout = new QVBoxLayout();
    lineEditLayout->addLayout(wakeLayout);
    lineEditLayout->addLayout(musicLayout);

    QLabel* activityLabel = new QLabel(QString("Activity Box"));
    activityBox = new QTextEdit();
    QWidget::connect(activityBox, SIGNAL(textChanged()), this, SLOT(valuesChanged()));
    QVBoxLayout* activityLayout = new QVBoxLayout();
    activityLayout->addWidget(activityLabel);
    activityLayout->addWidget(activityBox);

    QVBoxLayout* leftMiddleLayout = new QVBoxLayout();
    leftMiddleLayout->addLayout(lineEditLayout);
    leftMiddleLayout->addLayout(activityLayout);

    calendar = new QCalendarWidget();
    calendar->setMinimumSize(QSize(250, 250));
    calendar->setCurrentPage(today.year(), today.month());
    calendar->setSelectedDate(today);
    QWidget::connect(calendar, SIGNAL(activated(const QDate&)), this, SLOT(changedDate(const QDate&)));

    QLabel* prodLabel = new QLabel(QString("Productivity Level"));
    prod1Level = new QSlider();
    prod2Level = new QSpinBox();
    prod2Level->setFixedSize(80, 20);
    QWidget::connect(prod1Level, SIGNAL(valueChanged(int)), prod2Level, SLOT(setValue(int)));
    QWidget::connect(prod2Level, SIGNAL(valueChanged(int)), prod1Level, SLOT(setValue(int)));
    QWidget::connect(prod1Level, SIGNAL(valueChanged(int)), this, SLOT(valuesChanged()));
    QHBoxLayout* progressLayout = new QHBoxLayout();
    progressLayout->addWidget(prodLabel);
    progressLayout->addWidget(prod1Level);
    progressLayout->addSpacerItem(new QSpacerItem(40, 0));
    progressLayout->addWidget(prod2Level);

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addLayout(dateLayout);
    leftLayout->addLayout(leftMiddleLayout);
    leftLayout->addWidget(calendar);
    leftLayout->addLayout(progressLayout);
    leftLayout->addSpacerItem(new QSpacerItem(0, 10));

    QLabel* tableLabel = new QLabel(QString("Plan for this Day"));
    QPushButton* clearButton = new QPushButton(QString("Clear"));
    QWidget::connect(clearButton, SIGNAL(clicked()), this, SLOT(clearAction()));
    table = new QTableWidget(10, 3);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 3; j++) {
            table->setCellWidget(i, j, new QTextEdit());
            QTextEdit* data = (QTextEdit*) table->cellWidget(i, j);
            QWidget::connect(data, SIGNAL(textChanged()), this, SLOT(valuesChanged()));
        }
    }
    table->verticalHeader()->setDefaultSectionSize(table->height() / 8);
    table->horizontalHeader()->setDefaultSectionSize(table->width() / 5);
    table->setWordWrap(true);
    QWidget::connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(valuesChanged()));
    errorLabel = new QLabel(QString(""));
    errorLabel->setStyleSheet("*{color:red;}");
    errorLabel->setAlignment(Qt::AlignCenter);
    QHBoxLayout* planLayout = new QHBoxLayout();
    planLayout->addWidget(tableLabel);
    planLayout->addWidget(clearButton);
    QVBoxLayout* tableLayout = new QVBoxLayout();
    tableLayout->addLayout(planLayout);
    tableLayout->addWidget(table);
    tableLayout->addWidget(errorLabel);

    QHBoxLayout* fullLayout = new QHBoxLayout();
    fullLayout->addLayout(leftLayout);
    fullLayout->addLayout(tableLayout);

    readFromFile(today);

    slotAvailable = true;

    setLayout(fullLayout);
    setMinimumSize(QSize(900, 500));
}

void TrackerWidget::changedDate(const QDate& date) {
    if (slotAvailable) {
        writeToFile(prev);
        slotAvailable = false;
        clearEntries();
        readFromFile(date);
        slotAvailable = true;
        errorLabel->setText(QString("Data has been Saved/Loaded"));
        prev = date;
    }
}

void TrackerWidget::clearAction() {
    QMessageBox::StandardButton prompt =
            QMessageBox::question(this, "Confirm", "Clear All Entries for Today?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (prompt == QMessageBox::Yes) clearEntries();
}

void TrackerWidget::clearEntries() {
    wakeLine->setText(QString(""));
    musicLine->setText(QString(""));
    activityBox->setText(QString(""));
    prod1Level->setValue(0);
    for (int i = 0; i < table->rowCount(); i++) {
        for (int j = 0; j < table->columnCount(); j++) {
            QTextEdit* data = (QTextEdit*) table->cellWidget(i, j);
            data->setText(QString(""));
        }
    }
}

void TrackerWidget::valuesChanged() {
    if (slotAvailable) {
        writeToFile(prev);
        errorLabel->setText(QString("Input has been Auto-Saved"));
    }
}

void TrackerWidget::writeToFile(const QDate& date) {
    try {
        std::string filePath = date.toString("MMddyy").toUtf8().constData();
        filePath = "/AppData/" + filePath;
        filePath = QApplication::applicationDirPath().toUtf8().constData() + filePath;
        filePath += ".ufd";
        std::ofstream file(filePath);
        file << "# Date: " << date.toString("MMddyy").toUtf8().constData() << std::endl;
        if (wakeLine->text().compare(QString("")) != 0) {
            file << "> Wake" << std::endl;
            file << wakeLine->text().toUtf8().constData() << std::endl;
        }
        if (musicLine->text().compare(QString("")) != 0) {
            file << "> Music" << std::endl;
            file << musicLine->text().toUtf8().constData() << std::endl;
        }
        if (activityBox->toPlainText().compare(QString("")) != 0) {
            QString data = activityBox->toPlainText();
            QStringList activityList = data.split(QRegExp("[\n]"),QString::SkipEmptyParts);
            file << "> Activity " << activityList.size() << std::endl;
            for (int i = 0; i < activityList.size(); i++) {
                file << activityList[i].toUtf8().constData() << std::endl;
            }
        }
        file << "> Productivity" << std::endl;
        file << prod1Level->value() << std::endl;
        file << "> Table " << table->rowCount() << " " << table->columnCount() << std::endl;
        for (int i = 0; i < table->rowCount(); i++) {
            for (int j = 0; j < table->columnCount(); j++) {
                QTextEdit* data = (QTextEdit*) table->cellWidget(i, j);
                QStringList cellList = data->toPlainText().split(QRegExp("[\n]"),QString::SkipEmptyParts);
                file << cellList.size() << std::endl;
                if (cellList.size()) {
                    for (int k = 0; k < cellList.size(); k++) {
                        file << cellList[k].toUtf8().constData() << std::endl;
                    }
                }
            }
        }
        file << "#";
        file.close();

        int r = rand() % 100;
        std::string bakFilePath = date.toString("MMddyy").toUtf8().constData();
        bakFilePath = "/BackUpFiles/" + bakFilePath;
        bakFilePath += r;
        bakFilePath = QApplication::applicationDirPath().toUtf8().constData() + bakFilePath;
        bakFilePath += ".ufd";
        std::ofstream bakFile(bakFilePath);
        bakFile << "# Date: " << date.toString("MMddyy").toUtf8().constData() << std::endl;
        if (wakeLine->text().compare(QString("")) != 0) {
            bakFile << "> Wake" << std::endl;
            bakFile << wakeLine->text().toUtf8().constData() << std::endl;
        }
        if (musicLine->text().compare(QString("")) != 0) {
            bakFile << "> Music" << std::endl;
            bakFile << musicLine->text().toUtf8().constData() << std::endl;
        }
        if (activityBox->toPlainText().compare(QString("")) != 0) {
            QString data = activityBox->toPlainText();
            QStringList activityList = data.split(QRegExp("[\n]"),QString::SkipEmptyParts);
            bakFile << "> Activity " << activityList.size() << std::endl;
            for (int i = 0; i < activityList.size(); i++) {
                bakFile << activityList[i].toUtf8().constData() << std::endl;
            }
        }
        bakFile << "> Productivity" << std::endl;
        bakFile << prod1Level->value() << std::endl;
        bakFile << "> Table " << table->rowCount() << " " << table->columnCount() << std::endl;
        for (int i = 0; i < table->rowCount(); i++) {
            for (int j = 0; j < table->columnCount(); j++) {
                QTextEdit* data = (QTextEdit*) table->cellWidget(i, j);
                QStringList cellList = data->toPlainText().split(QRegExp("[\n]"),QString::SkipEmptyParts);
                bakFile << cellList.size() << std::endl;
                if (cellList.size()) {
                    for (int k = 0; k < cellList.size(); k++) {
                        bakFile << cellList[k].toUtf8().constData() << std::endl;
                    }
                }
            }
        }
        bakFile << "#";
    } catch(int e) {
        std::cout << "Error Writing File" << std::endl;
        errorLabel->setText("Error. Contact Support");
    }
}

void TrackerWidget::readFromFile(const QDate& date) {
    try {
        mainDate->setText(date.toString("MMMM dd, yyyy"));
        calendar->setCurrentPage(date.year(), date.month());
        mainDateEdit->setDate(date);

        std::string filePath = date.toString("MMddyy").toUtf8().constData();
        filePath = "/AppData/" + filePath;
        filePath = QApplication::applicationDirPath().toUtf8().constData() + filePath;
        filePath += ".ufd";
        std::ifstream file(filePath);
        if (file.is_open()) {
            while(!file.eof()) {
                std::string input; file >> input;
                if (input[0] == '#') {
                    std::string dummy;
                    getline(file, dummy, '\n');
                    continue;
                }
                std::string format; file >> format;
                if (format == "Wake") {
                    std::string val;
                    getline(file, val, '\n');
                    getline(file, val, '\n');
                    wakeLine->setText(QString::fromStdString(val));
                } else if (format == "Music") {
                    std::string val;
                    getline(file, val, '\n');
                    getline(file, val, '\n');
                    musicLine->setText(QString::fromStdString(val));
                } else if (format == "Activity") {
                    int num; file >> num;
                    std::string val;
                    getline(file, val, '\n');
                    for (int i = 0; i < num; i++) {
                        getline(file, val, '\n');
                        activityBox->append(QString::fromStdString(val));
                    }
                } else if (format == "Productivity") {
                    int val; file >> val;
                    prod1Level->setValue(val);
                } else if (format == "Table") {
                    int row, col; file >> row >> col;
                    std::string val;
                    getline(file, val, '\n');
                    for (int i = 0; i < row; i++) {
                        for (int j = 0; j < col; j++) {
                            int n; file >> n;
                            getline(file, val, '\n');
                            QTextEdit* data = (QTextEdit*) table->cellWidget(i, j);
                            data->setText("");
                            for (int k = 0; k < n; k++) {
                                getline(file, val, '\n');
                                data->append(QString::fromStdString(val));
                            }
                        }
                    }
                } else {
                    errorLabel->setText("File has been Curropted");
                }
            }
        } else {
            errorLabel->setText("File does not Exist");
        }
    } catch(int e) {
        std::cout << "Error Reading File" << std::endl;
        errorLabel->setText("Error. Contact Support");
    }
}

void TrackerWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    table->verticalHeader()->setDefaultSectionSize(table->height() / 10);
    table->horizontalHeader()->setDefaultSectionSize(table->width() / 3.2);
}

TrackerWidget::~TrackerWidget() {
    delete mainDate;
    delete mainDateEdit;
    delete wakeLine;
    delete musicLine;
    delete calendar;
    delete prod1Level;
    delete prod2Level;
    delete table;
    delete errorLabel;
}

