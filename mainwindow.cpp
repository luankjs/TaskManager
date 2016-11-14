#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

int cpuCores;

vector<int> last_total_jiffies;
vector<int> last_work_jiffies;
vector<int> total_jiffies;
vector<int> work_jiffies;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cpuCores = getCpuCores();

    initializeCpuGraph();
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

        last_total_jiffies.push_back(0);
        last_work_jiffies.push_back(0);
        total_jiffies.push_back(0);
        work_jiffies.push_back(0);
    }

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->widgetCpu->xAxis->setTicker(timeTicker);
    ui->widgetCpu->axisRect()->setupFullAxesBox();
    ui->widgetCpu->yAxis->setRange(0, 100);

    connect(ui->widgetCpu->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetCpu->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->widgetCpu->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->widgetCpu->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(5);
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
    static double lastPointKey = 0;

    if (key-lastPointKey > 0.5) {
      for(int i = 0; i <= cpuCores; i++) {
        cout << getCpuUsage(i) << endl;
        ui->widgetCpu->graph(i)->addData(key, getCpuUsage(i));
      }

      lastPointKey = key;
    }

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->widgetCpu->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->widgetCpu->replot();
}

float MainWindow::getCpuUsage(int cpu_id) {
    FILE* pipe = popen("cat /proc/stat", "r");
    if (!pipe) return 0;

    char buffer[128];
    string cpu_str = "cpu" + std::to_string(cpu_id);

    while (!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL) {
            vector<string> v{split(buffer, ' ')};

            if (v[0].find(cpu_str) != std::string::npos) {
                for(int i = 1; i < v.size(); i++) {
                    if (i <= 3) {
                        work_jiffies[cpu_id] += std::stoi(v[i]);
                    }
                    total_jiffies[cpu_id] += std::stoi(v[i]);
                }
            }
        }
    }

    pclose(pipe);

    long int work_over_period = work_jiffies[cpu_id] - last_work_jiffies[cpu_id];

    long int total_over_period = total_jiffies[cpu_id] - last_total_jiffies[cpu_id];

    float cpu = (float) work_over_period / (float) total_over_period * 100;

    for (int i = 0; i < 4; i++) {
        last_work_jiffies[i] = work_jiffies[i];
        last_total_jiffies[i] = total_jiffies[i];
    }

    return cpu;
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
