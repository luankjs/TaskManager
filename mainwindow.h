#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <regex>
#include <string>
#include <ctime>
#include <vector>
#include<iostream>
#include<fstream>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initializeCpuGraph();
    int randomColorNumber();
    int getCpuCores();
    vector<string> split(string str, char delimiter);
    float getCpuUsage(int cpu_id);

public slots:
    void realtimeDataSlot();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
};

#endif // MAINWINDOW_H
