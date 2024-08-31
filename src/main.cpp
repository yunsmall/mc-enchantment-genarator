#include <iostream>
#include <QApplication>
#include "selectwindow.h"
#include "core/EnchGenMachine.h"

#include "core/JsonReader.h"

int main(int argc,char** argv) {
    // GenEnch::JsonReader reader;
    // auto enchs=reader.readEnchDatas("data/附魔.json");
    //
    // // auto ench_json=reader.toJson(*enchs);
    // // std::cout<<ench_json.dump(4)<<std::endl;
    //
    // auto items=reader.readItems("data/物品");
    // // std::cout<<reader.toJson(items->begin()->second).dump(4)<<std::endl;
    //
    //
    //
    // GenEnch::EnchGenMachine machine("1.19.2",enchs,items);
    // machine.changeCurrentItem("剑");
    // machine.setToSurvivalBestEnch();
    // // machine.tryAddEnch("锋利",100);
    // // machine.tryAddEnch("耐久",100);
    // // machine.tryAddEnch("火焰附加",100);
    // machine.trySettingInGameName("下界合金剑");
    // std::cout<<machine.genCommand()<<std::endl;
    QApplication a(argc,argv);
    SelectWindow window;
    window.show();

    return a.exec();
}