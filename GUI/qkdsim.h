
#ifndef QKDSIM_H
#define QKDSIM_H

#include "Alg/Network.h"
#include "progressbar.h"

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class QKDSim;
}
QT_END_NAMESPACE

class QKDSim : public QMainWindow

{
    Q_OBJECT

public:
    QKDSim(QWidget *parent = nullptr);
    ~QKDSim();

private slots:
    void open_net();
    void open_dem();
    void save_net();
    void save_dem();


    void on_bt_start_clicked();

    void on_bt_begin_clicked();

    void on_bt_next_clicked();

    void on_bt_next10_clicked();

    void on_bt_show_node_clicked();

    void showOutput();

    //    void on_tableWidget_dem_cellChanged(int row, int column);

    void on_tableWidget_in_cellChanged(int row, int column);

    void on_box_input_currentIndexChanged(int index);

private:
    Ui::QKDSim *ui;
    QGraphicsScene *scene;  // 将scene作为成员变量

    CNetwork* net;
    void next_step();

    QTimer *timer;
    void Connections();

//    void readNetTable();
//    void readDemTable();

    // 加载进度条
    LoadingDialog *loadingDialog;

    // 用于存储读取的csv文件数据
    vector<tuple<int, int, int, double, double, double, double, double>> network;
    vector<tuple<int, int, int, double, double>> demand;

    enum Kind
    {
        Network,
        Demand
    };
    void loadCSV(const QString &fileName, Kind kind);   // 加载csv文件至数据结构
    void showCSV(Kind kind);    // 将数据结构存储的数据显示到表格上
    void readCSV(Kind kind);    // 将存储的数据结构转化为初始化CNetwork
    void readTable(Kind kind);  // 读取表格并存储为数据结构

    void showNodeGraph();
};

#endif // QKDSIM_H
