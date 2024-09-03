﻿
#include "qkdsim.h"
#include "ui_qkdsim.h"


QKDSim::QKDSim(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QKDSim)
{
    ui->setupUi(this);
    ui->tableWidget_net->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 表格列宽自动伸缩
    ui->tableWidget_dem->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_path->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_out->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 读取csv文件
    loadCSV("../../Input/network.csv", Network);
    loadCSV("../../Input/demand.csv", Demand);

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

void QKDSim::loadCSV(const QString &fileName, Kind kind)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);

    QTableWidget *tableWidget;

    QStringList headers;

    switch (kind)
    {
    case Network:
        tableWidget = ui->tableWidget_net;
        /********* 读取node个数 ************/
        int nodeNum;
        in >> nodeNum;
        ui->edit_node_num->setText(QString::number(nodeNum));   // 将显示的nodeNum也更改
        in.readLine();  // 略过第一行的换行符

        headers = {"linkId", "sourceId", "sinkId", "keyRate", "proDelay", "bandWidth", "weight", "faultTime"};
        break;

    case Demand:
        tableWidget = ui->tableWidget_dem;

        headers = {"demandId", "sourceId", "sinkId", "demandVolume", "arriveTime"};
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
    loadCSV(filename, Network);
    ui->tabWidget->setCurrentIndex(0);
}

void QKDSim::open_dem()
{
    // 加载文件至接收内容显示框
    QString filename = QFileDialog::getOpenFileName(this, tr("打开需求文件"));
    loadCSV(filename, Demand);
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
    out << ui->edit_node_num->text().toInt() << "\n"; // 第一行存nodenum
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
        TIME proDelay, faultTime;
        WEIGHT weight;

        int id_faultTime = 1000000+row;

        // 从表格的每一列读取数据
        QTableWidgetItem *linkIdItem = ui->tableWidget_net->item(row, 0);
        QTableWidgetItem *sourceIdItem = ui->tableWidget_net->item(row, 1);
        QTableWidgetItem *sinkIdItem = ui->tableWidget_net->item(row, 2);
        QTableWidgetItem *keyRateItem = ui->tableWidget_net->item(row, 3);
        QTableWidgetItem *proDelayItem = ui->tableWidget_net->item(row, 4);
        QTableWidgetItem *bandWidthItem = ui->tableWidget_net->item(row, 5);
        QTableWidgetItem *weightItem = ui->tableWidget_net->item(row, 6);
        QTableWidgetItem *faultTimeItem = ui->tableWidget_net->item(row, 7);

        if (linkIdItem && sourceIdItem && sinkIdItem && keyRateItem && proDelayItem && bandWidthItem && weightItem)
        {
            // 转换为相应的数据类型
            linkId = linkIdItem->text().toUInt();
            sourceId = sourceIdItem->text().toUInt();
            sinkId = sinkIdItem->text().toUInt();
            keyRate = keyRateItem->text().toDouble();  // 假设 keyRate 是一个双精度浮点数
            proDelay = proDelayItem->text().toDouble(); // 假设 proDelay 是一个双精度浮点数
            bandWidth = bandWidthItem->text().toDouble();
            weight = weightItem->text().toDouble();
            faultTime = faultTimeItem->text() == "" ? -1 : faultTimeItem->text().toDouble();    // 没有故障，则为-1

            // 处理链路信息
            CLink newLink;
            newLink.SetLinkId(linkId);
            newLink.SetSourceId(sourceId);
            newLink.SetSinkId(sinkId);
            newLink.SetQKDRate(keyRate);
            newLink.SetLinkDelay(proDelay);
            newLink.SetBandwidth(bandWidth);
            newLink.SetWeight(weight);
            newLink.SetFaultTime(faultTime);

            net->m_vAllLinks.push_back(newLink);
            net->m_mNodePairToLink[make_pair(sourceId, sinkId)] = linkId;
            net->m_mNodePairToLink[make_pair(sinkId, sourceId)] = linkId;


            net->m_vAllNodes[sourceId].m_lAdjNodes.push_back(sinkId);
            net->m_vAllNodes[sinkId].m_lAdjNodes.push_back(sourceId);

            /**********如何赋值**************/
            net->m_mDemandArriveTime.insert(make_pair(faultTime, id_faultTime));   // 增加m_mDemandArriveTime视为增加故障点
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
            net->m_vAllNodes[sourceId].m_mRelayVolume[demandId] = demandVolume;   //对m_mRelayVolume做初始化
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

void QKDSim::showOutput()
{
    ui->edit_time->setText(QString::number(net->CurrentTime(), 'f', 2));

    ui->tableWidget_out->clear();
    ui->tableWidget_out->setRowCount(0);    // 清空表格
    QStringList headers = {"demandId", "nodeId", "nextNode", "minLink", "avaiableKeys", "relayVolume", "isDelivered"};     // 尚未确定
    ui->tableWidget_out->setColumnCount(headers.size());
    ui->tableWidget_out->setHorizontalHeaderLabels(headers);

    // 遍历每个节点上正在传输的数据量
    for (NODEID nodeId = 0; nodeId < net->GetNodeNum(); nodeId++)
    {
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

            QTableWidgetItem* demandIdItem = new QTableWidgetItem(QString::number(demandId));
            QTableWidgetItem* nodeIdItem = new QTableWidgetItem(QString::number(nodeId));
            QTableWidgetItem* nextNodeItem = new QTableWidgetItem(QString::number(nextNode));
            QTableWidgetItem* minLinkItem = new QTableWidgetItem(QString::number(minLink));
            QTableWidgetItem* avaiableKeysItem = new QTableWidgetItem(QString::number(avaiableKeys, 'f', 2));
            QTableWidgetItem* relayVolumeItem = new QTableWidgetItem(QString::number(relayVolume, 'f', 2));
            QTableWidgetItem* isDeliveredItem = new QTableWidgetItem(isDelivered ? "True" : "False");

            ui->tableWidget_out->setItem(newRow, 0, demandIdItem);
            ui->tableWidget_out->setItem(newRow, 1, nodeIdItem);
            ui->tableWidget_out->setItem(newRow, 2, nextNodeItem);
            ui->tableWidget_out->setItem(newRow, 3, minLinkItem);
            ui->tableWidget_out->setItem(newRow, 4, avaiableKeysItem);
            ui->tableWidget_out->setItem(newRow, 5, relayVolumeItem);
            ui->tableWidget_out->setItem(newRow, 6, isDeliveredItem);
        }
    }
    // 显示已传输完毕的数据
    for (auto demandIter = net->m_vAllDemands.begin(); demandIter != net->m_vAllDemands.end(); demandIter++)
    {
        if (demandIter->GetAllDelivered())
        {
            NODEID nodeId = demandIter->GetSinkId();
            DEMANDID demandId = demandIter->GetDemandId();
            VOLUME relayVolume = 0;
            bool isDelivered = demandIter->GetAllDelivered();
//            NODEID nextNode = demandIter->GetSinkId();
//            LINKID minLink = net->m_mNodePairToLink[make_pair(nodeId, nextNode)];
//            VOLUME avaiableKeys = net->m_vAllLinks[minLink].GetAvaialbeKeys();

            int newRow = ui->tableWidget_out->rowCount();
            ui->tableWidget_out->insertRow(newRow);    // 末尾增加一行

            QTableWidgetItem* demandIdItem = new QTableWidgetItem(QString::number(demandId));
            QTableWidgetItem* nodeIdItem = new QTableWidgetItem(QString::number(nodeId));
            QTableWidgetItem* nextNodeItem = new QTableWidgetItem();
            QTableWidgetItem* minLinkItem = new QTableWidgetItem();
            QTableWidgetItem* avaiableKeysItem = new QTableWidgetItem();
            QTableWidgetItem* relayVolumeItem = new QTableWidgetItem(QString::number(relayVolume, 'f', 2));
            QTableWidgetItem* isDeliveredItem = new QTableWidgetItem(isDelivered ? "True" : "False");

            ui->tableWidget_out->setItem(newRow, 0, demandIdItem);
            ui->tableWidget_out->setItem(newRow, 1, nodeIdItem);
            ui->tableWidget_out->setItem(newRow, 2, nextNodeItem);
            ui->tableWidget_out->setItem(newRow, 3, minLinkItem);
            ui->tableWidget_out->setItem(newRow, 4, avaiableKeysItem);
            ui->tableWidget_out->setItem(newRow, 5, relayVolumeItem);
            ui->tableWidget_out->setItem(newRow, 6, isDeliveredItem);
        }
    }

    ui->tableWidget_path->clear();
    ui->tableWidget_path->setRowCount(0);    // 清空表格
    QStringList headers_path = {"demandId", "Node1", "Node2"};     // 尚未确定
    ui->tableWidget_path->setColumnCount(headers_path.size());
    ui->tableWidget_path->setHorizontalHeaderLabels(headers_path);
    // 显示每个需求的路由的最短路径
    for (auto demandIter = net->m_vAllDemands.begin(); demandIter != net->m_vAllDemands.end(); demandIter++)
    {
        if (demandIter->GetRouted())
        {
            DEMANDID demandId = demandIter->GetDemandId();
            list<NODEID> node_path = net->m_vAllDemands[demandId].m_Path.m_lTraversedNodes;

            // 如果当前行需要的列数大于表格当前列数，则扩展表格的列数
            int currentColCount = node_path.size() + 1; // 因为第一列是demandId
            if (currentColCount > ui->tableWidget_path->columnCount())
            {
                int oldColCount = ui->tableWidget_path->columnCount();
                ui->tableWidget_path->setColumnCount(currentColCount);
                // 设置新的列头
                for (int col = oldColCount; col < currentColCount; ++col)
                {
                    ui->tableWidget_path->setHorizontalHeaderItem(col, new QTableWidgetItem(QString("Node%1").arg(col)));
                }
            }

            int newRow = ui->tableWidget_path->rowCount();
            ui->tableWidget_path->insertRow(newRow);    // 末尾增加一行
            // 填充当前行的数据
            ui->tableWidget_path->setItem(newRow, 0, new QTableWidgetItem(QString::number(demandId)));
            int col = 1;  // 索引变量
            for (auto it = node_path.begin(); it != node_path.end(); ++it, ++col)
            {
                ui->tableWidget_path->setItem(newRow, col, new QTableWidgetItem(QString::number(*it)));
            }
        }
    }
}

void QKDSim::on_bt_start_clicked()
{
    readNetTable();
    readDemTable();

//    net->MainProcess();
    net->InitRelayPath();

    // 输出表格
    showOutput();
}

void QKDSim::on_bt_next_clicked()
{
    if (!net->AllDemandsDelivered())
    {
        TIME executeTime = net->OneTimeRelay();
        net->MoveSimTime(executeTime);

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




