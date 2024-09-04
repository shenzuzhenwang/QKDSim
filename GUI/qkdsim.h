
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

    void Connections();

private slots:
    void open_net();
    void open_dem();
    void save_net();
    void save_dem();


    void on_bt_start_clicked();

    void on_bt_next_clicked();

    void on_tableWidget_net_cellChanged(int row, int column);

    void on_tableWidget_dem_cellChanged(int row, int column);

private:
    Ui::QKDSim *ui;
    QGraphicsScene *scene;  // 将scene作为成员变量

//    int nodeNum;
    CNetwork* net;

    enum Kind
    {
        Network,
        Demand
    };
    void loadCSV(const QString &fileName, Kind kind);
    void readNetTable();
    void readDemTable();
    void showOutput();
    void showNodeGraph(NODEID nodeId);
};

#endif // QKDSIM_H
