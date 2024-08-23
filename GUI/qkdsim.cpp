
#include "qkdsim.h"
#include "ui_qkdsim.h"


QKDSim::QKDSim(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QKDSim)
{
    ui->setupUi(this);
    ui->tableWidget_net->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_dem->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 表格列宽自动伸缩

    Connections();
}

QKDSim::~QKDSim()
{
    delete ui;
}

void QKDSim::Connections()
{
    // 关于QT
    connect(ui->action_qt, &QAction::triggered, [this] {QMessageBox::aboutQt(this);});

//    // 帮助
//    connect(ui->action_help, &QAction::triggered, help_dialog, &QWidget::show);

    // 关闭程序
    connect(ui->action_exit, &QAction::triggered, this, &QWidget::close);

    // 保存打开文件（接收内容显示框）
    connect(ui->action_open_net, &QAction::triggered, this, &QKDSim::open_net);
    connect(ui->action_open_dem, &QAction::triggered, this, &QKDSim::open_dem);
    connect(ui->action_save_net, &QAction::triggered, this, &QKDSim::save_net);
    connect(ui->action_save_dem, &QAction::triggered, this, &QKDSim::save_dem);
}

void QKDSim::loadCSV(const QString &fileName, Kind kind, const QStringList &headers)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);

    QTableWidget *tableWidget;

    switch (kind)
    {
    case Network:
        tableWidget = ui->tableWidget_net;
        /********* 读取node个数 ************/
        in >> nodeNum;
        in.readLine();  // 略过第一行的换行符
        break;

    case Demand:
        tableWidget = ui->tableWidget_dem;
        break;

    default:
        break;
    }

    tableWidget->clear();
    tableWidget->setRowCount(0);
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    int row = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        tableWidget->insertRow(row);
        for (int column = 0; column < fields.size(); ++column)
        {
            tableWidget->setItem(row, column, new QTableWidgetItem(fields[column]));
        }
        row++;
    }

    file.close();
}

void QKDSim::open_net()
{
    // 加载文件至接收内容显示框
    QString filename = QFileDialog::getOpenFileName(this, tr("打开网络拓扑文件"));
    QStringList headers = {"linkId", "sourceId", "sinkId", "keyRate", "proDelay"};
    loadCSV(filename, Network, headers);
    ui->tabWidget->setCurrentIndex(0);
}

void QKDSim::open_dem()
{
    // 加载文件至接收内容显示框
    QString filename = QFileDialog::getOpenFileName(this, tr("打开需求文件"));
    QStringList headers = {"demandId", "sourceId", "sinkId", "demandVolume", "arriveTime"};
    loadCSV(filename, Demand, headers);
    ui->tabWidget->setCurrentIndex(1);
}

void QKDSim::save_net()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("保存网络拓扑"));
    QFile file(filename);
    if (!file.open(QFile::WriteOnly))
    {
        ui->statusbar->showMessage("Save file failed", 5000);
        return;
    }

    QTextStream out(&file);
    out << nodeNum << "\n"; // 第一行存nodenum
    for (int row = 0; row < ui->tableWidget_net->rowCount(); ++row)
    {
        QStringList rowData;
        for (int column = 0; column < ui->tableWidget_net->columnCount(); ++column)
        {
            QTableWidgetItem *item = ui->tableWidget_net->item(row, column);
            rowData << (item ? item->text() : "");
        }
        out << rowData.join(",") << "\n";
    }
    file.close();
    ui->statusbar->showMessage("Successfully save network", 5000);
}

void QKDSim::save_dem()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("保存网络需求"));
    QFile file(filename);
    if (!file.open(QFile::WriteOnly))
    {
        ui->statusbar->showMessage("Save file failed", 5000);
        return;
    }

    QTextStream out(&file);
    for (int row = 0; row < ui->tableWidget_dem->rowCount(); ++row)
    {
        QStringList rowData;
        for (int column = 0; column < ui->tableWidget_dem->columnCount(); ++column)
        {
            QTableWidgetItem *item = ui->tableWidget_dem->item(row, column);
            rowData << (item ? item->text() : "");
        }
        out << rowData.join(",") << "\n";
    }
    file.close();
    ui->statusbar->showMessage("Successfully save demand", 5000);
}

//void readTableData(QTableWidget *tableWidget)
//{
//    // 读取节点信息
//    UINT nodeNum = static_cast<UINT>(rowCount); // 假设 nodeNum 与行数一致
//    InitNodes(nodeNum);

//    // 逐行读取链路信息
//    for (int row = 0; row < tableWidget->rowCount(); ++row)
//    {
//        LINKID linkId;
//        NODEID sourceId, sinkId;
//        RATE keyRate;
//        TIME proDelay;

//        // 从表格的每一列读取数据
//        QTableWidgetItem *linkIdItem = ui->tableWidget_net->item(row, 0);
//        QTableWidgetItem *sourceIdItem = ui->tableWidget_net->item(row, 1);
//        QTableWidgetItem *sinkIdItem = ui->tableWidget_net->item(row, 2);
//        QTableWidgetItem *keyRateItem = ui->tableWidget_net->item(row, 3);
//        QTableWidgetItem *proDelayItem = ui->tableWidget_net->item(row, 4);

//        if (linkIdItem && sourceIdItem && sinkIdItem && keyRateItem && proDelayItem)
//        {
//            // 转换为相应的数据类型
//            linkId = linkIdItem->text().toUInt();
//            sourceId = sourceIdItem->text().toUInt();
//            sinkId = sinkIdItem->text().toUInt();
//            keyRate = keyRateItem->text().toDouble();  // 假设 keyRate 是一个双精度浮点数
//            proDelay = proDelayItem->text().toDouble(); // 假设 proDelay 是一个双精度浮点数

//            // 处理链路信息
//            CLink newLink;
//            newLink.SetLinkId(linkId);
//            newLink.SetSourceId(sourceId);
//            newLink.SetSinkId(sinkId);
//            newLink.SetQKDRate(keyRate);
//            newLink.SetLinkDelay(proDelay);
//            m_pNetwork->m_vAllLinks.push_back(newLink);
//            m_pNetwork->m_mNodePairToLink[make_pair(sourceId, sinkId)] = linkId;
//            m_pNetwork->m_mNodePairToLink[make_pair(sinkId, sourceId)] = linkId;
//            m_pNetwork->InitKeyManagerOverLink(linkId);
//        }
//        else
//        {
//            ui->statusbar->showMessage("Error: Missing data in table", 5000);
//        }
//    }

//    m_pNetwork->SetLinkNum(nodeNum);  // 假设链路数等于节点数
//    ui->statusbar->showMessage("Network data processed successfully", 5000);
//}




