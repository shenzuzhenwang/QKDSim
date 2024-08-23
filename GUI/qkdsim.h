
#ifndef QKDSIM_H
#define QKDSIM_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>


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

private:
    Ui::QKDSim *ui;

    int nodeNum;

    enum Kind
    {
        Network,
        Demand
    };
    void loadCSV(const QString &fileName, Kind kind, const QStringList &headers);

};

#endif // QKDSIM_H
