
#include "qkdsim.h"
#include "ui_qkdsim.h"
#include <unordered_set>

// 加载csv文件至数据结构
void QKDSim::loadCSV(const QString &fileName, Kind kind)
{
    //    QFile file(fileName);
    //    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    //    {
    //        return;
    //    }

    //    QTextStream in(&file);

    //    QTableWidget *tableWidget;

    //    QStringList headers;

    //    switch (kind)
    //    {
    //    case Network:
    //        tableWidget = ui->tableWidget_in;
    //        /********* 读取node个数 ************/
    //        int nodeNum;
    //        in >> nodeNum;
    //        ui->edit_node_num->setText(QString::number(nodeNum));   // 将显示的nodeNum也更改
    //        in.readLine();  // 略过第一行的换行符

    //        headers = {"链路序号", "源节点序号", "目的节点序号", "密钥生成速率(keys/s)", "时延", "带宽(MB/s)", "链路权重", "故障时间(s)"};
    //        break;

    //    case Demand:
    //        tableWidget = ui->tableWidget_dem;

    //        headers = {"需求序号", "源节点序号", "目的节点序号", "需传输数据量(MB)", "开始传输时间(s)"};
    //        break;

    //    default:
    //        break;
    //    }

    //    // 清空表格
    //    tableWidget->clear();
    //    tableWidget->setRowCount(0);

    //    tableWidget->setColumnCount(headers.size());
    //    tableWidget->setHorizontalHeaderLabels(headers);

    //    int row = 0;
    //    while (!in.atEnd())
    //    {
    //        QString line = in.readLine();
    //        QStringList fields = line.split(",");

    //        tableWidget->insertRow(row);
    //        for (int column = 0; column < fields.size(); ++column)
    //        {
    //            tableWidget->setItem(row, column, new QTableWidgetItem(fields[column]));
    //        }
    //        row++;
    //    }

    //    file.close();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;  // 文件打开失败

    QTextStream in(&file);
    if (kind == Network)
    {
        int nodeNum;
        in >> nodeNum;
        ui->edit_node_num->setText(QString::number(nodeNum));   // 将显示的nodeNum也更改
        in.readLine();  // 略过第一行的换行符
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = line.split(',');

            //            if (fields.size() < 8)
            //                continue;  // 确保数据完整
            int field1 = fields[0].toInt();
            int field2 = fields[1].toInt();
            int field3 = fields[2].toInt();
            double field4 = fields[3].toDouble();
            double field5 = fields[4].toDouble();
            double field6 = fields[5].toDouble();
            double field7 = fields[6].toDouble();
            double field8 = fields[7].toDouble();

            network.push_back(make_tuple(field1, field2, field3, field4, field5, field6, field7, field8));
        }
    }
    else if (kind == Demand)
    {
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList fields = line.split(',');

            int field1 = fields[0].toInt();
            int field2 = fields[1].toInt();
            int field3 = fields[2].toInt();
            double field4 = fields[3].toDouble();
            double field5 = fields[4].toDouble();

            demand.push_back(make_tuple(field1, field2, field3, field4, field5));
        }
    }
    file.close();
}

// 将数据结构存储的数据显示到表格上
void QKDSim::showCSV(Kind kind)
{
    if (kind == Network)
    {
        QStringList headers = {"链路序号", "源节点序号", "目的节点序号", "密钥生成速率(kbps)", "时延", "带宽(Mbps)", "链路权重", "故障时间(s)"};
        ui->tableWidget_in->setColumnCount(headers.size());
        ui->tableWidget_in->setHorizontalHeaderLabels(headers);
        ui->tableWidget_in->setRowCount(static_cast<int>(network.size()));
        ui->tableWidget_in->insertRow(static_cast<int>(network.size()));
        int row = 0;
        for(auto &net : network)
        {
            ui->tableWidget_in->setItem(row, 0, new QTableWidgetItem(QString::number(std::get<0>(net))));
            ui->tableWidget_in->setItem(row, 1, new QTableWidgetItem(QString::number(std::get<1>(net))));
            ui->tableWidget_in->setItem(row, 2, new QTableWidgetItem(QString::number(std::get<2>(net))));
            ui->tableWidget_in->setItem(row, 3, new QTableWidgetItem(QString::number(std::get<3>(net), 'f', 2)));
            ui->tableWidget_in->setItem(row, 4, new QTableWidgetItem(QString::number(std::get<4>(net), 'f', 2)));
            ui->tableWidget_in->setItem(row, 5, new QTableWidgetItem(QString::number(std::get<5>(net), 'f', 2)));
            ui->tableWidget_in->setItem(row, 6, new QTableWidgetItem(QString::number(std::get<6>(net), 'f', 2)));
            ui->tableWidget_in->setItem(row, 7, new QTableWidgetItem(QString::number(std::get<7>(net), 'f', 2)));
            row++;
        }
    }
    else if(kind == Demand)
    {
        QStringList headers = {"需求序号", "源节点序号", "目的节点序号", "需传输数据量(MB)", "开始传输时间(s)"};
        ui->tableWidget_in->setColumnCount(headers.size());
        ui->tableWidget_in->setHorizontalHeaderLabels(headers);
        ui->tableWidget_in->setRowCount(static_cast<int>(demand.size()));
        ui->tableWidget_in->insertRow(static_cast<int>(demand.size()));
        int row = 0;
        for(auto &dem : demand)
        {
            ui->tableWidget_in->setItem(row, 0, new QTableWidgetItem(QString::number(std::get<0>(dem))));
            ui->tableWidget_in->setItem(row, 1, new QTableWidgetItem(QString::number(std::get<1>(dem))));
            ui->tableWidget_in->setItem(row, 2, new QTableWidgetItem(QString::number(std::get<2>(dem))));
            ui->tableWidget_in->setItem(row, 3, new QTableWidgetItem(QString::number(std::get<3>(dem), 'f', 2)));
            ui->tableWidget_in->setItem(row, 4, new QTableWidgetItem(QString::number(std::get<4>(dem), 'f', 2)));
            row++;
        }
    }
}

// 将存储的数据结构转化为初始化CNetwork
void QKDSim::readCSV(Kind kind)
{
    if (kind == Network)
    {
        // 读取节点信息
        QString nodeNumString = ui->edit_node_num->text();
        if (nodeNumString.isEmpty())
        {
            ui->statusbar->showMessage("Missing data in node number");
            return;
        }
        int nodeNum = nodeNumString.toInt();
        net->SetNodeNum(nodeNum);
        net->InitNodes(nodeNum);

        // 逐行读取链路信息
        for (auto &netw : network)
        {
            LINKID linkId = std::get<0>(netw);
            NODEID sourceId = std::get<1>(netw);
            NODEID sinkId = std::get<2>(netw);
            RATE keyRate = std::get<3>(netw);
            TIME proDelay = std::get<4>(netw);
            RATE bandWidth = std::get<5>(netw);
            WEIGHT weight = std::get<6>(netw);
            TIME faultTime = std::get<7>(netw);

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
            net->InitKeyManagerOverLink(linkId);

            net->m_vAllNodes[sourceId].m_lAdjNodes.push_back(sinkId);
            net->m_vAllNodes[sinkId].m_lAdjNodes.push_back(sourceId);
            net->InitKeyManagerOverLink(linkId);

            /**********如何赋值**************/
            if (faultTime > 0)
                net->m_mDemandArriveTime.insert(make_pair(faultTime, 1000000 + linkId));   // 增加m_mDemandArriveTime视为增加故障点  m_mDemandArriveTime插入的value为1000000 + linkId     ？？可能有bug
        }
        net->SetLinkNum(static_cast<int>(network.size()));
    }
    else if(kind == Demand)
    {
        for (auto &dem : demand)
        {
            DEMANDID demandId = std::get<0>(dem);
            NODEID sourceId = std::get<1>(dem);
            NODEID sinkId = std::get<2>(dem);
            VOLUME demandVolume = std::get<3>(dem);
            TIME arriveTime = std::get<4>(dem);

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
        net->SetDemandNum(static_cast<int>(demand.size()));
    }
}

// 读取表格并存储为数据结构
void QKDSim::readTable(Kind kind)
{
    if (kind == Network)
    {
        network.clear();
        // 逐行读取链路信息
        for (int row = 0; row < ui->tableWidget_in->rowCount() - 1; ++row)     // 最后一行为空行
        {
            LINKID linkId;
            NODEID sourceId, sinkId;
            RATE keyRate, bandWidth;
            TIME proDelay, faultTime;
            WEIGHT weight;

            // 从表格的每一列读取数据
            if (ui->tableWidget_in->item(row, 0) && ui->tableWidget_in->item(row, 1) && ui->tableWidget_in->item(row, 2) && ui->tableWidget_in->item(row, 3)
                    && ui->tableWidget_in->item(row, 4) && ui->tableWidget_in->item(row, 5) && ui->tableWidget_in->item(row, 6))
            {
                // 转换为相应的数据类型
                linkId = ui->tableWidget_in->item(row, 0)->text().toUInt();
                sourceId = ui->tableWidget_in->item(row, 1)->text().toUInt();
                sinkId = ui->tableWidget_in->item(row, 2)->text().toUInt();
                keyRate = ui->tableWidget_in->item(row, 3)->text().toDouble();  // 假设 keyRate 是一个双精度浮点数
                proDelay = ui->tableWidget_in->item(row, 4)->text().toDouble(); // 假设 proDelay 是一个双精度浮点数
                bandWidth = ui->tableWidget_in->item(row, 5)->text().toDouble();
                weight = ui->tableWidget_in->item(row, 6)->text().toDouble();
                faultTime = ui->tableWidget_in->item(row, 7)->text() == "" ? -1 : ui->tableWidget_in->item(row, 7)->text().toDouble();    // 没有故障，则为-1

                network.push_back(make_tuple(linkId, sourceId, sinkId, keyRate, proDelay, bandWidth, weight, faultTime));
            }
            else
            {
                ui->statusbar->showMessage("Missing data in network table");
            }
        }
    }
    else if(kind == Demand)
    {
        demand.clear();
        // 逐行读取需求信息
        for (int row = 0; row < ui->tableWidget_in->rowCount() - 1; ++row)     // 最后一行为空行
        {
            DEMANDID demandId;
            NODEID sourceId, sinkId;
            VOLUME demandVolume;
            TIME arriveTime;

            // 从表格的每一列读取数据
            if (ui->tableWidget_in->item(row, 0) && ui->tableWidget_in->item(row, 1) && ui->tableWidget_in->item(row, 2)
                    && ui->tableWidget_in->item(row, 3) && ui->tableWidget_in->item(row, 4))
            {
                // 转换为相应的数据类型
                demandId = ui->tableWidget_in->item(row, 0)->text().toUInt();
                sourceId = ui->tableWidget_in->item(row, 1)->text().toUInt();
                sinkId = ui->tableWidget_in->item(row, 2)->text().toUInt();
                demandVolume = ui->tableWidget_in->item(row, 3)->text().toDouble();  // 假设 demandVolume 是一个双精度浮点数
                arriveTime = ui->tableWidget_in->item(row, 4)->text().toDouble();  // 假设 arriveTime 是一个双精度浮点数

                demand.push_back(make_tuple(demandId, sourceId, sinkId, demandVolume, arriveTime));
            }
            else
            {
                ui->statusbar->showMessage("Missing data in demand table");
            }
        }
    }
}

// 显示输出的表格数据
void QKDSim::showOutput()
{
    ui->edit_time->setText(QString("%1s").arg(QString::number(net->CurrentTime(), 'f', 2)));

    ui->tableWidget_out->clear();
    ui->tableWidget_out->setRowCount(0);    // 清空表格
    QStringList headers = {"需求序号", "当前节点序号", "下一节点序号", "下一跳链路序号", "可用密钥(keys)", "待传输数据量(Mb)", "完成时间(s)", "是否故障", "等待或传输"};     // 尚未确定
    ui->tableWidget_out->setColumnCount(headers.size());
    ui->tableWidget_out->setHorizontalHeaderLabels(headers);

    // 遍历每个节点上正在传输的数据量
    for (NODEID nodeId = 0; nodeId < net->GetNodeNum(); nodeId++)
    {
        for (auto demandIter = net->m_vAllNodes[nodeId].m_mRelayVolume.begin(); demandIter != net->m_vAllNodes[nodeId].m_mRelayVolume.end();)
        {
            if (net->m_vAllDemands[demandIter->first].GetRoutedFailed())
            {
                demandIter = net->m_vAllNodes[nodeId].m_mRelayVolume.erase(demandIter);
                continue;
            }
            if (net->m_vAllDemands[demandIter->first].GetArriveTime() > net->CurrentTime()) // 不显示未到达的需求
            {
                demandIter++;
                continue;
            }

            DEMANDID demandId = demandIter->first;
            VOLUME relayVolume = demandIter->second;
            bool isDelivered = net->m_vAllDemands[demandId].GetAllDelivered();
            // 对于每个需求，从其路径中找到下一个要中继到的节点 nextNode
            NODEID nextNode = net->m_vAllDemands[demandId].m_Path.m_mNextNode[nodeId];
            // 找到当前节点和下一个节点之间的链路 minLink
            LINKID minLink = net->m_mNodePairToLink[make_pair(nodeId, nextNode)];
            VOLUME avaiableKeys = net->m_vAllLinks[minLink].GetAvaialbeKeys();
            TIME completeTime = net->m_vAllDemands[demandId].GetCompleteTime();
            bool isRouteFailed = net->m_vAllDemands[demandId].GetRoutedFailed();
            bool isWait = net->m_vAllLinks[minLink].wait_or_not;

            int newRow = ui->tableWidget_out->rowCount();
            ui->tableWidget_out->insertRow(newRow);    // 末尾增加一行

            ui->tableWidget_out->setItem(newRow, 0, new QTableWidgetItem(QString::number(demandId)));
            ui->tableWidget_out->setItem(newRow, 1, new QTableWidgetItem(QString::number(nodeId)));
            ui->tableWidget_out->setItem(newRow, 2, new QTableWidgetItem(QString::number(nextNode)));
            ui->tableWidget_out->setItem(newRow, 3, new QTableWidgetItem(QString::number(minLink)));
            ui->tableWidget_out->setItem(newRow, 4, new QTableWidgetItem(QString::number(avaiableKeys, 'f', 2)));
            ui->tableWidget_out->setItem(newRow, 5, new QTableWidgetItem(QString::number(relayVolume, 'f', 2)));
            ui->tableWidget_out->setItem(newRow, 6, new QTableWidgetItem(isDelivered ? QString::number(completeTime, 'f', 2) : "No"));
            ui->tableWidget_out->setItem(newRow, 7, new QTableWidgetItem(isRouteFailed ? "Yes" : "No"));
            ui->tableWidget_out->setItem(newRow, 8, new QTableWidgetItem(isWait ? "Wait" : "Transmit"));

            demandIter++;
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
            bool isRouteFailed = net->m_vAllDemands[demandId].GetRoutedFailed();
            TIME completeTime = net->m_vAllDemands[demandId].GetCompleteTime();

            int newRow = ui->tableWidget_out->rowCount();
            ui->tableWidget_out->insertRow(newRow);    // 末尾增加一行

            ui->tableWidget_out->setItem(newRow, 0, new QTableWidgetItem(QString::number(demandId)));
            ui->tableWidget_out->setItem(newRow, 1, new QTableWidgetItem(QString::number(nodeId)));
            ui->tableWidget_out->setItem(newRow, 2, new QTableWidgetItem());
            ui->tableWidget_out->setItem(newRow, 3, new QTableWidgetItem());
            ui->tableWidget_out->setItem(newRow, 4, new QTableWidgetItem());
            ui->tableWidget_out->setItem(newRow, 5, new QTableWidgetItem(QString::number(relayVolume, 'f', 2)));
            ui->tableWidget_out->setItem(newRow, 6, new QTableWidgetItem(isDelivered ? QString::number(completeTime, 'f', 2) : "No"));
            ui->tableWidget_out->setItem(newRow, 7, new QTableWidgetItem(isRouteFailed ? "Yes" : "No"));
        }
    }

    ui->tableWidget_path->clear();
    ui->tableWidget_path->setRowCount(0);    // 清空表格
    QStringList headers_path = {"需求序号", "节点1序号", "节点2序号"};     // 至少首尾两个节点
    ui->tableWidget_path->setColumnCount(headers_path.size());
    ui->tableWidget_path->setHorizontalHeaderLabels(headers_path);

    // 显示每个需求的路由的最短路径
    for (auto demandIter = net->m_vAllDemands.begin(); demandIter != net->m_vAllDemands.end(); demandIter++)
    {
        if (demandIter->GetRoutedFailed())
        {
            DEMANDID demandId = demandIter->GetDemandId();
            int newRow = ui->tableWidget_path->rowCount();
            ui->tableWidget_path->insertRow(newRow);    // 末尾增加一行
            ui->tableWidget_path->setItem(newRow, 0, new QTableWidgetItem(QString::number(demandId)));
            // 都显示-1
            ui->tableWidget_path->setItem(newRow, 1, new QTableWidgetItem("-1"));
            ui->tableWidget_path->setItem(newRow, 2, new QTableWidgetItem("-1"));
        }
        else
        {
            DEMANDID demandId = demandIter->GetDemandId();
            list<NODEID> node_path = net->m_vAllDemands[demandId].m_Path.m_lTraversedNodes;

            // 如果当前行需要的列数大于表格当前列数，则扩展表格的列数
            int currentColCount = static_cast<int>(node_path.size() + 1); // 因为第一列是demandId
            if (currentColCount > ui->tableWidget_path->columnCount())
            {
                int oldColCount = ui->tableWidget_path->columnCount();
                ui->tableWidget_path->setColumnCount(currentColCount);
                // 设置新的列头
                for (int col = oldColCount; col < currentColCount; ++col)
                {
                    ui->tableWidget_path->setHorizontalHeaderItem(col, new QTableWidgetItem(QString("节点%1序号").arg(col)));
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

// 显示局部节点拓扑图
void QKDSim::showNodeGraph()
{
    NODEID centerNodeId = ui->edit_show_node->text().toInt(); // 中心节点
    if (centerNodeId >= net->GetNodeNum())
    {
        QMessageBox::critical(nullptr, "错误", "没有这个节点！");
        return;
    }

    scene->clear();
    int WIDTH = ui->graph_node->size().width() - 10;
    int HEIGHT = ui->graph_node->size().height() - 10;
    int NODE_SIZE = min(WIDTH, HEIGHT) / 10;
    qreal RADIUS = min(WIDTH, HEIGHT) / 4.2;

    scene->setSceneRect(0, 0, WIDTH, HEIGHT);  // 场景大小

    unordered_set<NODEID> nodeShow;
    unordered_set<LINKID> linkShow;
    unordered_map<NODEID, pair<int, int>> loc;    // 节点位置

    // 中心节点
    scene->addEllipse(WIDTH / 2 - NODE_SIZE / 2, HEIGHT / 2 - NODE_SIZE / 2, NODE_SIZE, NODE_SIZE, QPen(), QBrush(Qt::cyan));
    QGraphicsTextItem* centerNodeText = scene->addText(QString::number(centerNodeId), QFont("Arial", 24));
    centerNodeText->setPos(WIDTH / 2 - centerNodeText->boundingRect().width() / 2, HEIGHT / 2 - centerNodeText->boundingRect().height() / 2);
    nodeShow.emplace(centerNodeId);
    loc[centerNodeId] = make_pair(WIDTH / 2, HEIGHT / 2);

    int node_layer = ui->box_layer->currentText().toInt();
    unordered_set<NODEID> per1Nodes;   // 第一层节点
    if (node_layer >= 1)    // 显示第一层（距离中心节点为1）的节点与链路
    {
        for (auto linkIter = net->m_vAllLinks.begin(); linkIter != net->m_vAllLinks.end(); linkIter++)
        {
            if (linkIter->GetSourceId() == centerNodeId)
                per1Nodes.emplace(linkIter->GetSinkId());
            if (linkIter->GetSinkId() == centerNodeId)
                per1Nodes.emplace(linkIter->GetSourceId());
        }

        int numPer1Node = static_cast<int>(per1Nodes.size());
        auto per1Node = per1Nodes.begin();
        // 以中心节点为圆心的一个圆环，分布第一层节点
        for (int i = 0; i < numPer1Node; i++, per1Node++)
        {
            qreal angle = 2 * M_PI * i / numPer1Node;  // 分割圆周
            qreal x = WIDTH / 2 + RADIUS * cos(angle);
            qreal y = HEIGHT / 2 + RADIUS * sin(angle);
            scene->addEllipse(x - NODE_SIZE / 2, y - NODE_SIZE / 2, NODE_SIZE, NODE_SIZE, QPen(), QBrush(Qt::lightGray));
            QGraphicsTextItem* peripheralNodeText = scene->addText(QString::number(*per1Node), QFont("Arial", 24)); // 给每个节点一个编号
            peripheralNodeText->setPos(x - peripheralNodeText->boundingRect().width() / 2, y - peripheralNodeText->boundingRect().height() / 2);
            // 位置
            nodeShow.emplace(*per1Node);
            loc[*per1Node] = make_pair(x, y);
        }
    }
    unordered_set<NODEID> per2Nodes;   // 第二层节点
    if (node_layer >= 2)    // 显示第二层（距离中心节点为2）的节点与链路
    {
        /************完成第二层节点的显示***************/
        qreal RADIUS_LAYER2 = RADIUS * 2; // 第二层的半径比第一层大200
        // 获取并显示第二层的节点
        for (auto linkIter = net->m_vAllLinks.begin(); linkIter != net->m_vAllLinks.end(); ++linkIter)
        {
            NODEID sourceId = linkIter->GetSourceId();
            NODEID sinkId = linkIter->GetSinkId();

            // 当此link的sourceId在第一层，且sinkId不在nodeShow（不为中心节点且不在第一层）
            if (auto conNode = per1Nodes.find(sourceId); conNode != per1Nodes.end() && nodeShow.find(sinkId) == nodeShow.end())
                per2Nodes.emplace(sinkId);
            // 当此link的sinkId在第一层，且sourceId不在nodeShow（不为中心节点且不在第一层）
            if (auto conNode = per1Nodes.find(sinkId); conNode != per1Nodes.end() && nodeShow.find(sourceId) == nodeShow.end())
                per2Nodes.emplace(sourceId);
        }

        int numPer2Node = static_cast<int>(per2Nodes.size());
        auto per2Node = per2Nodes.begin();
        // 以中心节点为圆心的一个圆环，分布第二层节点
        for (int i = 0; i < numPer2Node; i++, per2Node++)
        {
            qreal angle = 2 * M_PI * i / numPer2Node + M_PI / 6; // 分割圆周 为了岔开，第二层起始点加30度
            qreal x = WIDTH / 2 + RADIUS_LAYER2 * cos(angle);
            qreal y = HEIGHT / 2 + RADIUS_LAYER2 * sin(angle);
            scene->addEllipse(x - NODE_SIZE / 2, y - NODE_SIZE / 2, NODE_SIZE, NODE_SIZE, QPen(), QBrush(Qt::lightGray));
            QGraphicsTextItem* peripheralNodeText = scene->addText(QString::number(*per2Node), QFont("Arial", 24)); // 给每个节点一个编号
            peripheralNodeText->setPos(x - peripheralNodeText->boundingRect().width() / 2, y - peripheralNodeText->boundingRect().height() / 2);
            // 位置
            nodeShow.emplace(*per2Node);
            loc[*per2Node] = make_pair(x, y);
        }
    }
    // 所有显示节点的两两间的链路
    for (auto i = nodeShow.begin(); i != nodeShow.end(); i++)
    {
        auto next = i;
        for(auto j = ++next; j != nodeShow.end(); j++)
        {
            if(net->m_mNodePairToLink.find(make_pair(*i, *j)) != net->m_mNodePairToLink.end())
            {
                qreal x1 = loc[*i].first;
                qreal y1 = loc[*i].second;
                qreal x2 = loc[*j].first;
                qreal y2 = loc[*j].second;
                scene->addLine(x1, y1, x2, y2, QPen(Qt::gray));
                LINKID linkId = net->m_mNodePairToLink[make_pair(*i, *j)];
                if (ui->check_show_linkid->isChecked() == true)
                {
                    QGraphicsTextItem* lineText = scene->addText(QString::number(linkId), QFont("Arial", 16)); // 线上也显示编号
                    lineText->setPos((x1 + x2) / 2 - lineText->boundingRect().width() / 2, (y1 + y2) / 2 - lineText->boundingRect().height() / 2);
                }
                linkShow.emplace(linkId);
            }
        }
    }

    ui->graph_node->show();

    // 显示node
    ui->tableWidget_node->clear();
    ui->tableWidget_node->setRowCount(0);    // 清空表格
    QStringList headers_node = {"需求序号", "节点序号", "下一跳链路序号", "待传输数据量(Mb)"};     // 尚未确定
    ui->tableWidget_node->setColumnCount(headers_node.size());
    ui->tableWidget_node->setHorizontalHeaderLabels(headers_node);
    for (NODEID nodeId : nodeShow)
    {
        for (auto demandIter = net->m_vAllNodes[nodeId].m_mRelayVolume.begin(); demandIter != net->m_vAllNodes[nodeId].m_mRelayVolume.end(); demandIter++)
        {
            //            if (net->m_vAllDemands[demandIter->first].GetRoutedFailed()||net->m_vAllDemands[demandIter->first].GetAllDelivered())
            //            {
            //                continue;
            //            }
            DEMANDID demandId = demandIter->first;
            VOLUME relayVolume = demandIter->second;
            // 对于每个需求，从其路径中找到下一个要中继到的节点 nextNode
            NODEID nextNode = net->m_vAllDemands[demandId].m_Path.m_mNextNode[nodeId];
            // 找到当前节点和下一个节点之间的链路 minLink
            LINKID minLink = net->m_mNodePairToLink[make_pair(nodeId, nextNode)];

            int newRow = ui->tableWidget_node->rowCount();
            ui->tableWidget_node->insertRow(newRow);    // 末尾增加一行

            ui->tableWidget_node->setItem(newRow, 0, new QTableWidgetItem(QString::number(demandId)));
            ui->tableWidget_node->setItem(newRow, 1, new QTableWidgetItem(QString::number(nodeId)));
            ui->tableWidget_node->setItem(newRow, 2, new QTableWidgetItem(QString::number(minLink)));
            ui->tableWidget_node->setItem(newRow, 3, new QTableWidgetItem(QString::number(relayVolume, 'f', 2)));
        }
    }
    // 显示link
    ui->tableWidget_link->clear();
    ui->tableWidget_link->setRowCount(0);    // 清空表格
    QStringList headers_link = {"链路序号", "节点1序号", "节点2序号", "可用密钥量(key)"};     // 尚未确定
    ui->tableWidget_link->setColumnCount(headers_link.size());
    ui->tableWidget_link->setHorizontalHeaderLabels(headers_link);
    for (LINKID linkId : linkShow)
    {
        NODEID nodeId = net->m_vAllLinks[linkId].GetSourceId();
        NODEID nextNode = net->m_vAllLinks[linkId].GetSinkId();
        VOLUME avaiableKeys = net->m_vAllLinks[linkId].GetAvaialbeKeys();

        int newRow = ui->tableWidget_link->rowCount();
        ui->tableWidget_link->insertRow(newRow);    // 末尾增加一行

        ui->tableWidget_link->setItem(newRow, 0, new QTableWidgetItem(QString::number(linkId)));
        ui->tableWidget_link->setItem(newRow, 1, new QTableWidgetItem(QString::number(nodeId)));
        ui->tableWidget_link->setItem(newRow, 2, new QTableWidgetItem(QString::number(nextNode)));
        ui->tableWidget_link->setItem(newRow, 3, new QTableWidgetItem(QString::number(avaiableKeys, 'f', 2)));
    }
}

//void QKDSim::readNetTable()
//{
//    // 读取节点信息
//    QString nodeNumString = ui->edit_node_num->text();
//    if (nodeNumString.isEmpty())
//    {
//        ui->statusbar->showMessage("Missing data in node number");
//        return;
//    }
//    int nodeNum = nodeNumString.toInt();
//    net->SetNodeNum(nodeNum);
//    net->InitNodes(nodeNum);

//    // 逐行读取链路信息
//    for (int row = 0; row < ui->tableWidget_in->rowCount() - 1; ++row)     // 最后一行为空行
//    {
//        LINKID linkId;
//        NODEID sourceId, sinkId;
//        RATE keyRate, bandWidth;
//        TIME proDelay, faultTime;
//        WEIGHT weight;

//        // 从表格的每一列读取数据
//        if (ui->tableWidget_in->item(row, 0) && ui->tableWidget_in->item(row, 1) && ui->tableWidget_in->item(row, 2) && ui->tableWidget_in->item(row, 3)
//                && ui->tableWidget_in->item(row, 4) && ui->tableWidget_in->item(row, 5) && ui->tableWidget_in->item(row, 6))
//        {
//            // 转换为相应的数据类型
//            linkId = ui->tableWidget_in->item(row, 0)->text().toUInt();
//            sourceId = ui->tableWidget_in->item(row, 1)->text().toUInt();
//            sinkId = ui->tableWidget_in->item(row, 2)->text().toUInt();
//            keyRate = ui->tableWidget_in->item(row, 3)->text().toDouble();  // 假设 keyRate 是一个双精度浮点数
//            proDelay = ui->tableWidget_in->item(row, 4)->text().toDouble(); // 假设 proDelay 是一个双精度浮点数
//            bandWidth = ui->tableWidget_in->item(row, 5)->text().toDouble();
//            weight = ui->tableWidget_in->item(row, 6)->text().toDouble();
//            faultTime = ui->tableWidget_in->item(row, 7)->text() == "" ? -1 : ui->tableWidget_in->item(row, 7)->text().toDouble();    // 没有故障，则为-1

//            // 处理链路信息
//            CLink newLink;
//            newLink.SetLinkId(linkId);
//            newLink.SetSourceId(sourceId);
//            newLink.SetSinkId(sinkId);
//            newLink.SetQKDRate(keyRate);
//            newLink.SetLinkDelay(proDelay);
//            newLink.SetBandwidth(bandWidth);
//            newLink.SetWeight(weight);
//            newLink.SetFaultTime(faultTime);

//            net->m_vAllLinks.push_back(newLink);
//            net->m_mNodePairToLink[make_pair(sourceId, sinkId)] = linkId;
//            net->m_mNodePairToLink[make_pair(sinkId, sourceId)] = linkId;
//            net->InitKeyManagerOverLink(linkId);


//            net->m_vAllNodes[sourceId].m_lAdjNodes.push_back(sinkId);
//            net->m_vAllNodes[sinkId].m_lAdjNodes.push_back(sourceId);
//            net->InitKeyManagerOverLink(linkId);

//            /**********如何赋值**************/
//            int id_faultTime = 1000000 + linkId;    // m_mDemandArriveTime插入的value为1000000 + linkId     ？？可能有bug
//            if (faultTime > 0)
//                net->m_mDemandArriveTime.insert(make_pair(faultTime, id_faultTime));   // 增加m_mDemandArriveTime视为增加故障点
//        }
//        else
//        {
//            ui->statusbar->showMessage("Missing data in network table");
//        }
//    }
//    net->SetLinkNum(ui->tableWidget_in->rowCount() - 1); //第一行是link数量，需要rowCount()-1
//}

//void QKDSim::readDemTable()
//{
//    // 逐行读取需求信息
//    for (int row = 0; row < ui->tableWidget_dem->rowCount() - 1; ++row)     // 最后一行为空行
//    {
//        DEMANDID demandId;
//        NODEID sourceId, sinkId;
//        VOLUME demandVolume;
//        TIME arriveTime;

//        // 从表格的每一列读取数据
//        if (ui->tableWidget_dem->item(row, 0) && ui->tableWidget_dem->item(row, 1) && ui->tableWidget_dem->item(row, 2)
//                && ui->tableWidget_dem->item(row, 3) && ui->tableWidget_dem->item(row, 4))
//        {
//            // 转换为相应的数据类型
//            demandId = ui->tableWidget_dem->item(row, 0)->text().toUInt();
//            sourceId = ui->tableWidget_dem->item(row, 1)->text().toUInt();
//            sinkId = ui->tableWidget_dem->item(row, 2)->text().toUInt();
//            demandVolume = ui->tableWidget_dem->item(row, 3)->text().toDouble();  // 假设 demandVolume 是一个双精度浮点数
//            arriveTime = ui->tableWidget_dem->item(row, 4)->text().toDouble();  // 假设 arriveTime 是一个双精度浮点数

//            // 处理需求信息
//            CDemand newDemand;
//            newDemand.SetDemandId(demandId);
//            newDemand.SetSourceId(sourceId);
//            newDemand.SetSinkId(sinkId);
//            newDemand.SetDemandVolume(demandVolume);
//            newDemand.SetRemainingVolume(demandVolume);
//            newDemand.SetArriveTime(arriveTime);
//            newDemand.SetCompleteTime(INF); // 假设 INF 是一个定义好的常量，表示无限大
//            net->m_vAllDemands.push_back(newDemand);
//            net->m_vAllNodes[sourceId].m_mRelayVolume[demandId] = demandVolume;   //对m_mRelayVolume做初始化
//            net->m_mDemandArriveTime.insert(make_pair(arriveTime, demandId));   // 增加m_mDemandArriveTime
//        }
//        else
//        {
//            ui->statusbar->showMessage("Missing data in demand table");
//        }
//    }
//    net->SetDemandNum(ui->tableWidget_dem->rowCount() - 1); //第一行是demand数量，需要rowCount()-1
//    //    ui->statusbar->showMessage("Demand data processed successfully", 5000);
//}
