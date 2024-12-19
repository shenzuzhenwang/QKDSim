#include "QKDSim.h"
#include "Network.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>  // 为了捕获异常

using namespace std;

// QKDSim Class Implementation

// 加载CSV文件中的数据
void QKDSim::loadCSV(const std::string& fileName, Kind kind) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }

    std::string line;
    if (kind == Network) {
        network.clear();

        // 读取链路数量（第一行）
        int linkNum = 0;
        if (std::getline(file, line)) {
            std::istringstream(line) >> linkNum;  // 读取链路数量
        }
        cout << "Link Number: " << linkNum << endl;

        // 逐行读取链路信息
        while (std::getline(file, line)) {
            std::istringstream stream(line);
            std::string field;
            std::vector<std::string> fields;
            while (std::getline(stream, field, ',')) {
                fields.push_back(field);
            }

            // 如果字段少于8个，则跳过此行
            if (fields.size() < 8) {
                continue; 
            }

            // 检查每个字段的有效性
            try {
                int field1 = std::stoi(fields[0]);  // 链路ID
                int field2 = std::stoi(fields[1]);  // 源节点ID
                int field3 = std::stoi(fields[2]);  // 目标节点ID
                double field4 = std::stod(fields[3]);  // QKD速率
                double field5 = std::stod(fields[4]);  // 延迟
                double field6 = std::stod(fields[5]);  // 带宽
                double field7 = std::stod(fields[6]);  // 权重
                double field8 = (fields[7].empty() || fields[7] == "-1") ? -1 : std::stod(fields[7]);  // 故障时间

                // 将链路数据存储到network中
                network.push_back(std::make_tuple(field1, field2, field3, field4, field5, field6, field7, field8));
            } catch (const std::invalid_argument& e) {
                // 如果有无效数据，跳过这一行并输出错误信息
                std::cerr << "Invalid data in line: " << line << std::endl;
                continue;
            } catch (const std::out_of_range& e) {
                // 如果数据超出范围，跳过这一行并输出错误信息
                std::cerr << "Data out of range in line: " << line << std::endl;
                continue;
            }
        }
    } else if (kind == Demand) {
        demand.clear();
        while (std::getline(file, line)) {
            std::istringstream stream(line);
            std::string field;
            std::vector<std::string> fields;
            while (std::getline(stream, field, ',')) {
                fields.push_back(field);
            }

            // 解析需求数据
            if (fields.size() < 5) {
                continue;  // 如果字段少于5个，跳过
            }

            try {
                int field1 = std::stoi(fields[0]);  // 需求ID
                int field2 = std::stoi(fields[1]);  // 源节点ID
                int field3 = std::stoi(fields[2]);  // 目标节点ID
                double field4 = std::stod(fields[3]);  // 流量
                double field5 = std::stod(fields[4]);  // 到达时间

                // 将需求数据存储到demand中
                demand.push_back(std::make_tuple(field1, field2, field3, field4, field5));
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid demand data in line: " << line << std::endl;
                continue;
            } catch (const std::out_of_range& e) {
                std::cerr << "Demand data out of range in line: " << line << std::endl;
                continue;
            }
        }
    }

    file.close();
}


// 将加载的数据用于初始化CNetwork
void QKDSim::readCSV(Kind kind) {
    if (kind == Network) {
        // 假设从文件或其他方式获取到的节点数（可以根据实际情况调整）
        int nodeNum = 10;  // 这里假设节点数是10
        net->SetNodeNum(nodeNum);
        net->InitNodes(nodeNum);

        // 逐行读取链路信息
        for (const auto& netw : network) {
            int linkId = std::get<0>(netw);
            int sourceId = std::get<1>(netw);
            int sinkId = std::get<2>(netw);
            double keyRate = std::get<3>(netw);
            double proDelay = std::get<4>(netw);
            double bandWidth = std::get<5>(netw);
            double weight = std::get<6>(netw);
            double faultTime = std::get<7>(netw);

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

            // 假设这部分用于处理故障时间
            if (faultTime > 0) {
                net->m_mDemandArriveTime.insert(make_pair(faultTime, 1000000 + linkId));
            }
        }

        // 设置链路数量
        net->SetLinkNum(static_cast<int>(network.size()));
    } else if (kind == Demand) {
        // 处理需求数据
        for (const auto& dem : demand) {
            int demandId = std::get<0>(dem);
            int sourceId = std::get<1>(dem);
            int sinkId = std::get<2>(dem);
            double flow = std::get<3>(dem);
            double arriveTime = std::get<4>(dem);

            // 处理需求信息（根据具体需求初始化）
            // 例如：添加需求到相应的节点、设置流量等
        }
    }
}
