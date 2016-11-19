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
    bool get_cpu_times(size_t &idle_time, size_t &total_time);
    std::vector<size_t> get_cpu_times();

public slots:
    void realtimeDataSlot();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
};

#endif // MAINWINDOW_H
