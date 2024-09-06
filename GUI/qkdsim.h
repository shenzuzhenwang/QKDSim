
#ifndef QKDSIM_H
#define QKDSIM_H

#include "Alg/Network.h"

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

    void on_tableWidget_net_cellChanged(int row, int column);

    void on_tableWidget_dem_cellChanged(int row, int column);

    void on_bt_begin_clicked();

    void on_bt_next_clicked();

    void on_bt_next10_clicked();

    void on_bt_next100_clicked();

    void on_bt_show_node_clicked();

private:
    Ui::QKDSim *ui;
    QGraphicsScene *scene;  // 将scene作为成员变量

//    int nodeNum;
    CNetwork* net;
    QTimer *timer;
    void Connections();

    enum Kind
    {
        Network,
        Demand
    };
    void loadCSV(const QString &fileName, Kind kind);
    void readNetTable();
    void readDemTable();
    void showOutput();
    void showNodeGraph();
    void next_step();

//    int step;
};

#endif // QKDSIM_H
