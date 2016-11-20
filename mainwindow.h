#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <regex>
#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <thread>

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
    void getCpuUsage(double key);
    void cpuGraph();
    void memoryGraph();
    void initializeMemoryGraph();

public slots:
    void realtimeDataSlot();
    void realtimeDataSlotMemory();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
};

#endif // MAINWINDOW_H
