//
// Created by 91246 on 24-8-27.
//

#ifndef SELECTWINDOW_H
#define SELECTWINDOW_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QValidator>

#include "core/EnchGenMachine.h"


QT_BEGIN_NAMESPACE
namespace Ui { class SelectWindow; }
QT_END_NAMESPACE

class SelectWindow : public QMainWindow {
Q_OBJECT

public:
    explicit SelectWindow(QWidget *parent = nullptr);
    ~SelectWindow() override;
private slots:

private:
    void loadDatas();
    void clearAllDatas();

    /**
     * @brief UI应该怎么变化
     */
    void onChangeItem();

    void setItemNamesCombo();
    void setSubItemNamesCombo();
    void showPreinstallCombo();

    void clearAllShownEnchs();

    void showSuitableEnchs();
    void showAllEnchs();

    void setValidator();

    void updateMachineInGameName();
    void updateMachineSelectedEnchs();

    void checkThroughList(const QStringList& enchs,bool check=true);
    void checkAll(bool check=true);

    void setCheckedToMaxSurvivalLvl();
    void setCheckedToLvl(uint32_t lvl);

    bool getAtRow(int row,QLabel** name,QLineEdit** lvl,QCheckBox** enable) const;


private:
    Ui::SelectWindow *ui;

    std::unique_ptr<GenEnch::EnchGenMachine> gen_machine;

    QIntValidator int255Validator{0,255};
    QIntValidator int32767Validator{0,32767};

    static constexpr auto ench_types_path="data/附魔.json";
    static constexpr auto ench_items_dir_path="data/物品";
};


#endif //SELECTWINDOW_H
