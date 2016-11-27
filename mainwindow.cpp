#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

int cpuCores;

vector<int> last_total_jiffies;
vector<int> last_work_jiffies;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cpuCores = getCpuCores();

    for(int i = 0; i <= cpuCores; i++) {
        last_total_jiffies.push_back(0);
        last_work_jiffies.push_back(0);
    }

    initializeCpuGraph();

    initializeMemoryGraph();

    initializeChargeGraph();

    initializeDischargeGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeCpuGraph() {
    srand (time(NULL));
    cpuCores -= 2;

    for(int i = 0; i <= cpuCores; i++) {
        int r = rand() % 255 + 1;
        int g = rand() % 255 + 1;
        int b = rand() % 255 + 1;

        ui->widgetCpu->addGraph();
        ui->widgetCpu->graph(i)->setPen(QPen(QColor(r, g, b)));
    }

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->widgetCpu->xAxis->setTicker(timeTicker);
    ui->widgetCpu->axisRect()->setupFullAxesBox();
    ui->widgetCpu->yAxis->setRange(0, 100);

    connect(ui->widgetCpu->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetCpu->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widgetCpu->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetCpu->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(2);
}

void MainWindow::initializeMemoryGraph() {
    srand (time(NULL));

    int r = rand() % 255 + 1;
    int g = rand() % 255 + 1;
    int b = rand() % 255 + 1;

    ui->widgetMemory->addGraph();
    ui->widgetMemory->graph(0)->setPen(QPen(QColor(r, g, b)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->widgetMemory->xAxis->setTicker(timeTicker);
    ui->widgetMemory->axisRect()->setupFullAxesBox();
    ui->widgetMemory->yAxis->setRange(0, 100);

    connect(ui->widgetMemory->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetMemory->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widgetMemory->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetMemory->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlotMemory()));
    dataTimer.start(1);
}

void MainWindow::initializeChargeGraph() {
    srand (time(NULL));

    int r = rand() % 255 + 1;
    int g = rand() % 255 + 1;
    int b = rand() % 255 + 1;

    ui->widgetCharge->addGraph();
    ui->widgetCharge->graph(0)->setPen(QPen(QColor(r, g, b)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->widgetCharge->xAxis->setTicker(timeTicker);
    ui->widgetCharge->axisRect()->setupFullAxesBox();
    ui->widgetCharge->yAxis->setRange(0, 100);

    connect(ui->widgetCharge->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetMemory->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widgetCharge->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetMemory->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlotCharge()));
    dataTimer.start(1);
}

void MainWindow::initializeDischargeGraph() {
    srand (time(NULL));

    int r = rand() % 255 + 1;
    int g = rand() % 255 + 1;
    int b = rand() % 255 + 1;

    ui->widgetDischarge->addGraph();
    ui->widgetDischarge->graph(0)->setPen(QPen(QColor(r, g, b)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->widgetDischarge->xAxis->setTicker(timeTicker);
    ui->widgetDischarge->axisRect()->setupFullAxesBox();
    ui->widgetDischarge->yAxis->setRange(0, 20);

    connect(ui->widgetDischarge->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetDischarge->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widgetDischarge->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetDischarge->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlotDischarge()));
    dataTimer.start(1);
}

void MainWindow::realtimeDataSlotMemory() {
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;

    std::ifstream proc_stat("/proc/meminfo");
    std::string str;
    int lineCount = 0;
    long int total_mem, free_mem;
    while (std::getline(proc_stat, str))
    {
       vector<string> memInfoLine{split(str, ' ')};

       if (lineCount == 0) {
           total_mem = std::stoi(memInfoLine[8]);
       } else if (lineCount == 1) {
           free_mem = std::stoi(memInfoLine[9]);
       } else {
           break;
       }
       lineCount++;
    }

    long int used_mem = total_mem - free_mem;

    float mem_usage = (100 * used_mem) / total_mem;

    ui->widgetMemory->graph(0)->addData(key, mem_usage);

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->widgetMemory->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->widgetMemory->replot();
}

void MainWindow::realtimeDataSlotCharge() {
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;

    std::ifstream battery_stat("/sys/class/power_supply/BAT1/uevent");
    std::string str;
    int lineCount = 0;
    long int percentCharge;
    while (std::getline(battery_stat, str))
    {
       if (lineCount == 11) {
           vector<string> batteryInfoLine{split(str, '=')};

           percentCharge = std::stoi(batteryInfoLine[1]);

           break;
       }
       lineCount++;
    }

    ui->widgetCharge->graph(0)->addData(key, percentCharge);

    ui->widgetCharge->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->widgetCharge->replot();
}

void MainWindow::realtimeDataSlotDischarge() {
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;

    std::ifstream battery_stat("/sys/class/power_supply/BAT1/uevent");
    std::string str;
    int lineCount = 0;
    long int powerSupplyChargeFull;
    long int powerSupplyChargeNow;
    long int powerSupplyCurrentNow;
    while (std::getline(battery_stat, str))
    {
       vector<string> batteryInfoLine{split(str, '=')};

       if (lineCount == 7) {
           powerSupplyCurrentNow = std::stoi(batteryInfoLine[1]);
       } else if (lineCount == 9) {
           powerSupplyChargeFull = std::stoi(batteryInfoLine[1]);
       } else if (lineCount == 10) {
           powerSupplyChargeNow = std::stoi(batteryInfoLine[1]);
       }
       lineCount++;
    }

    float dischargeTime = (powerSupplyChargeFull - powerSupplyChargeNow) / powerSupplyCurrentNow;

    ui->widgetDischarge->graph(0)->addData(key, dischargeTime);

    ui->widgetDischarge->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->widgetDischarge->replot();
}

int MainWindow::randomColorNumber() {
    srand (time(NULL));
    int random = (int) rand() % 256 + 1;

    return random;
}

int MainWindow::getCpuCores() {
    FILE* pipe = popen("cat /proc/stat", "r");
    if (!pipe) return 0;

    char buffer[128];
    int cpuCount = 0;

    while (!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL) {
            vector<string> v{split(buffer, ' ')};

            if (v[0].find("cpu") != std::string::npos) {
                cpuCount += 1;
            }
        }
    }

    pclose(pipe);

    return cpuCount;
}

void MainWindow::realtimeDataSlot()
{
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;

    getCpuUsage(key);

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->widgetCpu->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->widgetCpu->replot();
}

void MainWindow::getCpuUsage(double key) {
    FILE* pipe = popen("cat /proc/stat", "r");

    char buffer[128];
    int cpuIterator;
    vector<int> total_jiffies (4, 0);
    vector<int> work_jiffies (4, 0);

    cpuIterator = -1;
    while (!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL) {
            if (cpuIterator >= 0 && cpuIterator <= cpuCores) {
                vector<string> v{split(buffer, ' ')};

                for(int i = 1; i < v.size(); i++) {
                    if (i == 4) {
                        work_jiffies[cpuIterator] = std::stoi(v[i]);
                    }
                    total_jiffies[cpuIterator] += std::stoi(v[i]);
                }
            }
            cpuIterator++;
        }
    }

    pclose(pipe);

    for (int i = 0; i <= cpuCores; i++) {
        float work_over_period = work_jiffies[i] - last_work_jiffies[i];

        float total_over_period = total_jiffies[i] - last_total_jiffies[i];

        float cpu = (work_over_period / total_over_period) * 100;

        ui->widgetCpu->graph(i)->addData(key, cpu);

        last_total_jiffies.at(i) = total_jiffies.at(i);
        last_work_jiffies.at(i) = work_jiffies.at(i);
    }
}

/**
 * This function was copied by: http://code.runnable.com/VHb0hWMZp-ws1gAr/splitting-a-string-into-a-vector-for-c%2B%2B
 *
 * @brief MainWindow::split
 * @param str
 * @param delimiter
 * @return
 */
vector<string> MainWindow::split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}
