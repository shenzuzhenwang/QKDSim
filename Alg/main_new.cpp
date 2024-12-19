#include "QKDSim.h"
#include "./Network.h"  // 包含 CNetwork 类的头文件

int main() {
    // 假设您已经有了 CNetwork 的实现
    CNetwork network;
    QKDSim sim(&network);

    // 加载网络数据
    sim.QKDSim::loadCSV("../Input/10规模/network.csv", Network);
    sim.readCSV(Network);

    // 加载需求数据
    sim.QKDSim::loadCSV("../Input/10规模/demand.csv", Demand);
    sim.readCSV(Demand);

    return 0;
}
