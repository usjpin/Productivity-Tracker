#ifndef TRACKERWIDGET_H
#define TRACKERWIDGET_H

#include <QWidget>
#include <QtGui>
#include <QtWidgets>

class TrackerWidget : public QWidget
{
    Q_OBJECT

public:
    TrackerWidget(QWidget *parent = nullptr);
    ~TrackerWidget();
    void resizeEvent(QResizeEvent *event);
private slots:
    void changedDate(const QDate& date);
    void valuesChanged();
    void clearAction();
private:
    QDate today;
    QDate prev;
    QLabel* mainDate;
    QDateEdit* mainDateEdit;
    QLineEdit* wakeLine;
    QLineEdit* musicLine;
    QTextEdit* activityBox;
    QCalendarWidget* calendar;
    QSlider* prod1Level;
    QSpinBox* prod2Level;
    QTableWidget* table;
    QLabel* errorLabel;
    bool slotAvailable;
    void writeToFile(const QDate& date);
    void clearEntries();
    void readFromFile(const QDate& date);
};
#endif // TRACKERWIDGET_H
