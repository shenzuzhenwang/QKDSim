
#include "qkdsim.h"
#include "ui_qkdsim.h"


QKDSim::QKDSim(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QKDSim)
{
    ui->setupUi(this);
    ui->tableWidget_net->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 表格列宽自动伸缩
    ui->tableWidget_dem->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_out->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 读取csv文件
    loadCSV("../Input/network.csv", Network, {"linkId", "sourceId", "sinkId", "keyRate", "proDelay", "bandWidth", "weight"});
    loadCSV("../Input/demand.csv", Demand, {"demandId", "sourceId", "sinkId", "demandVolume", "arriveTime"});

    Connections();

    net = new CNetwork();
}

QKDSim::~QKDSim()
{
    delete net;
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
        int nodeNum;
        in >> nodeNum;
        ui->edit_node_num->setText(QString::number(nodeNum));   // 将显示的nodeNum也更改
        in.readLine();  // 略过第一行的换行符
        break;

    case Demand:
        tableWidget = ui->tableWidget_dem;
        break;

    default:
        break;
    }

    // 清空表格
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
    QStringList headers = {"linkId", "sourceId", "sinkId", "keyRate", "proDelay", "bandWidth", "weight"};
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
    out << net->GetNodeNum() << "\n"; // 第一行存nodenum
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

void QKDSim::readNetTable()
{
    // 读取节点信息
    QString nodeNumString = ui->edit_node_num->text();
    if (nodeNumString.isEmpty())
    {
        ui->statusbar->showMessage("Error: Missing data in node number", 5000);
        return;
    }
    int nodeNum = nodeNumString.toInt();
    net->SetNodeNum(nodeNum);
    net->InitNodes(nodeNum);

    // 逐行读取链路信息
    for (int row = 0; row < ui->tableWidget_net->rowCount(); ++row)
    {
        LINKID linkId;
        NODEID sourceId, sinkId;
        RATE keyRate, bandWidth;
        TIME proDelay;
        WEIGHT weight;

        // 从表格的每一列读取数据
        QTableWidgetItem *linkIdItem = ui->tableWidget_net->item(row, 0);
        QTableWidgetItem *sourceIdItem = ui->tableWidget_net->item(row, 1);
        QTableWidgetItem *sinkIdItem = ui->tableWidget_net->item(row, 2);
        QTableWidgetItem *keyRateItem = ui->tableWidget_net->item(row, 3);
        QTableWidgetItem *proDelayItem = ui->tableWidget_net->item(row, 4);
        QTableWidgetItem *bandWidthItem = ui->tableWidget_net->item(row, 5);
        QTableWidgetItem *weightItem = ui->tableWidget_net->item(row, 6);

        if (linkIdItem && sourceIdItem && sinkIdItem && keyRateItem && proDelayItem)
        {
            // 转换为相应的数据类型
            linkId = linkIdItem->text().toUInt();
            sourceId = sourceIdItem->text().toUInt();
            sinkId = sinkIdItem->text().toUInt();
            keyRate = keyRateItem->text().toDouble();  // 假设 keyRate 是一个双精度浮点数
            proDelay = proDelayItem->text().toDouble(); // 假设 proDelay 是一个双精度浮点数
            bandWidth = bandWidthItem->text().toDouble();
            weight = weightItem->text().toDouble();

            // 处理链路信息
            CLink newLink;
            newLink.SetLinkId(linkId);
            newLink.SetSourceId(sourceId);
            newLink.SetSinkId(sinkId);
            newLink.SetQKDRate(keyRate);
            newLink.SetLinkDelay(proDelay);
            newLink.SetBandwidth(bandWidth);
            newLink.SetWeight(weight);

            net->m_vAllLinks.push_back(newLink);
            net->m_mNodePairToLink[make_pair(sourceId, sinkId)] = linkId;
            net->m_mNodePairToLink[make_pair(sinkId, sourceId)] = linkId;
            net->InitKeyManagerOverLink(linkId);

            net->m_vAllNodes[sourceId].m_lAdjNodes.push_back(sinkId);
            net->m_vAllNodes[sinkId].m_lAdjNodes.push_back(sourceId);
        }
        else
        {
            ui->statusbar->showMessage("Error: Missing data in network table", 5000);
        }
    }
    net->SetLinkNum(ui->tableWidget_net->rowCount());
    ui->statusbar->showMessage("Network data processed successfully", 5000);
}

void QKDSim::readDemTable()
{
    // 逐行读取需求信息
    for (int row = 0; row < ui->tableWidget_dem->rowCount(); ++row)
    {
        DEMANDID demandId;
        NODEID sourceId, sinkId;
        VOLUME demandVolume;
        TIME arriveTime;

        // 从表格的每一列读取数据
        QTableWidgetItem *demandIdItem = ui->tableWidget_dem->item(row, 0);
        QTableWidgetItem *sourceIdItem = ui->tableWidget_dem->item(row, 1);
        QTableWidgetItem *sinkIdItem = ui->tableWidget_dem->item(row, 2);
        QTableWidgetItem *demandVolumeItem = ui->tableWidget_dem->item(row, 3);
        QTableWidgetItem *arriveTimeItem = ui->tableWidget_dem->item(row, 4);

        if (demandIdItem && sourceIdItem && sinkIdItem && demandVolumeItem && arriveTimeItem)
        {
            // 转换为相应的数据类型
            demandId = demandIdItem->text().toUInt();
            sourceId = sourceIdItem->text().toUInt();
            sinkId = sinkIdItem->text().toUInt();
            demandVolume = demandVolumeItem->text().toDouble();  // 假设 demandVolume 是一个双精度浮点数
            arriveTime = arriveTimeItem->text().toDouble();  // 假设 arriveTime 是一个双精度浮点数

            // 处理需求信息
            CDemand newDemand;
            newDemand.SetDemandId(demandId);
            newDemand.SetSourceId(sourceId);
            newDemand.SetSinkId(sinkId);
            newDemand.SetDemandVolume(demandVolume);
            newDemand.SetRemainingVolume(demandVolume);
            newDemand.SetArriveTime(arriveTime);
            newDemand.SetCompleteTime(INF); // 假设 INF 是一个定义好的常量，表示无限大
            net->m_vAllDemands.push_back(newDemand);
            net->m_vAllNodes[sourceId].m_mRelayVolume[demandId] = demandVolume;
            net->m_mDemandArriveTime.insert(make_pair(arriveTime, demandId));   // 增加m_mDemandArriveTime
        }
        else
        {
            ui->statusbar->showMessage("Error: Missing data in demand table", 5000);
        }
    }
    net->SetDemandNum(ui->tableWidget_dem->rowCount());
    ui->statusbar->showMessage("Demand data processed successfully", 5000);
}

void QKDSim::on_bt_start_clicked()
{
    readNetTable();
    readDemTable();

//    net->MainProcess();
    net->InitRelayPath();


    // 输出表格
    showOutput();
//    ui->tableWidget_out->setRowCount(net->m_vAllNodes.size());
//    QStringList headers = {"demandId", "sourceId", "sinkId", "demandVolume", "arriveTime"};
//    ui->tableWidget_out->setColumnCount(headers.size());
//    ui->tableWidget_out->setHorizontalHeaderLabels(headers);
}

void QKDSim::showOutput()
{
    ui->edit_time->setText(QString::number(net->CurrentTime(), 'f', 2));

    ui->tableWidget_out->clear();
    ui->tableWidget_out->setRowCount(0);    // 清空表格
    QStringList headers = {"nodeId", "nextNode", "minLink", "avaiableKeys", "demandId", "relayVolume", "isDelivered"};     // 尚未确定
    ui->tableWidget_out->setColumnCount(headers.size());
    ui->tableWidget_out->setHorizontalHeaderLabels(headers);

    for (NODEID nodeId = 0; nodeId < net->GetNodeNum(); nodeId++)
    {
        // 遍历每个节点上正在传输的数据量
        for (auto demandIter = net->m_vAllNodes[nodeId].m_mRelayVolume.begin(); demandIter != net->m_vAllNodes[nodeId].m_mRelayVolume.end(); demandIter++)
        {
            DEMANDID demandId = demandIter->first;
            VOLUME relayVolume = demandIter->second;
            bool isDelivered = net->m_vAllDemands[demandId].GetAllDelivered();
            // 对于每个需求，从其路径中找到下一个要中继到的节点 nextNode
            NODEID nextNode = net->m_vAllDemands[demandId].m_Path.m_mNextNode[nodeId];
            // 找到当前节点和下一个节点之间的链路 minLink
            LINKID minLink = net->m_mNodePairToLink[make_pair(nodeId, nextNode)];
            VOLUME avaiableKeys = net->m_vAllLinks[minLink].GetAvaialbeKeys();

            int newRow = ui->tableWidget_out->rowCount();
            ui->tableWidget_out->insertRow(newRow);    // 末尾增加一行

            QTableWidgetItem* nodeIdItem = new QTableWidgetItem(QString::number(nodeId));
            QTableWidgetItem* nextNodeItem = new QTableWidgetItem(QString::number(nextNode));
            QTableWidgetItem* minLinkItem = new QTableWidgetItem(QString::number(minLink));
            QTableWidgetItem* avaiableKeysItem = new QTableWidgetItem(QString::number(avaiableKeys, 'f', 2));
            QTableWidgetItem* demandIdItem = new QTableWidgetItem(QString::number(demandId));
            QTableWidgetItem* relayVolumeItem = new QTableWidgetItem(QString::number(relayVolume, 'f', 2));
            QTableWidgetItem* isDeliveredItem = new QTableWidgetItem(isDelivered ? "True" : "False");   // 第一次用这种方法

            ui->tableWidget_out->setItem(newRow, 0, nodeIdItem);
            ui->tableWidget_out->setItem(newRow, 1, nextNodeItem);
            ui->tableWidget_out->setItem(newRow, 2, minLinkItem);
            ui->tableWidget_out->setItem(newRow, 3, avaiableKeysItem);
            ui->tableWidget_out->setItem(newRow, 4, demandIdItem);
            ui->tableWidget_out->setItem(newRow, 5, relayVolumeItem);
            ui->tableWidget_out->setItem(newRow, 6, isDeliveredItem);
        }
    }
}

void QKDSim::on_bt_next_clicked()
{
    if (!net->AllDemandsDelivered())
    {
        TIME executeTime = net->OneTimeRelay();
        net->MoveSimTime(executeTime);

        /**********************************显示每一步的结果******************************/
        showOutput();
    }
    else
    {
        ui->statusbar->showMessage("All demand has benn delivered", 5000);
    }
}

// 自动加减行
void tableWidget_cellChanged(int row, int column, QTableWidget* tableWidget)
{
    // 最后一行添加数据，则再加一个空行
    int rowCount = tableWidget->rowCount();
    if (row == rowCount - 1)
    {
        tableWidget->insertRow(rowCount);
    }

    // 某一行删除数据，查看此行是否为空行，并删除
    QTableWidgetItem* item = tableWidget->item(row, column);
    if (item != nullptr && item->text().isEmpty())
    {
        // 如果单元格为空，检查整行是否也为空
        bool isRowEmpty = true;
        for (int j = 0; j < tableWidget->columnCount(); ++j)
        {
            QTableWidgetItem* cell = tableWidget->item(row, j);
            if (cell != nullptr && !cell->text().isEmpty())
            {
                isRowEmpty = false;
                break;
            }
        }
        if (isRowEmpty)
        {
            tableWidget->removeRow(row);  // 如果整行为空，则删除该行
        }
    }
}

void QKDSim::on_tableWidget_net_cellChanged(int row, int column)
{
    tableWidget_cellChanged(row, column, ui->tableWidget_net);
}

void QKDSim::on_tableWidget_dem_cellChanged(int row, int column)
{
    tableWidget_cellChanged(row, column, ui->tableWidget_dem);
}




