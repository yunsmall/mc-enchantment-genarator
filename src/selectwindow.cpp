//
// Created by 91246 on 24-8-27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SelectWindow.h" resolved

#include "selectwindow.h"

#include "ui_SelectWindow.h"

#include <QLineEdit>
#include <QClipboard>

#include "core/JsonReader.h"


SelectWindow::SelectWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::SelectWindow) {
    ui->setupUi(this);

    loadDatas();
    setValidator();

    //工具栏
    connect(ui->load_data_action,&QAction::triggered,[this]() {
        loadDatas();
    });
    connect(ui->exit_action,&QAction::triggered,this,&SelectWindow::close);


    connect(ui->actionaboutqt,&QAction::triggered,qApp,&QApplication::aboutQt);
    //版本输入框
    connect(ui->version_combo_box,&QComboBox::currentIndexChanged,[this]() {
        loadDatas();
    });
    //物品类型下拉列表
    connect(ui->item_name_combo,&QComboBox::currentIndexChanged,[this]() {
        gen_machine->changeCurrentItem(ui->item_name_combo->currentText().toStdString());
        onChangeItem();

    });
    connect(ui->custom_ench_check_box,&QCheckBox::clicked,[this](auto checked) {
        if(checked) {
            showAllEnchs();
            ui->preinstall_combo->clear();
        }
        else {
            showSuitableEnchs();
            showPreinstallCombo();
        }
    });
    //附魔种类选择
    //全选框
    connect(ui->check_all,&QCheckBox::clicked,[this](auto checked) {
        for(auto row=0;row<ui->scroll_layout->rowCount();row++) {
            auto* enable_item=ui->scroll_layout->itemAtPosition(row,2);
            auto* enable_check=enable_item?qobject_cast<QCheckBox*>(enable_item->widget()):nullptr;
            if(enable_check) {
                enable_check->setChecked(checked);
            }
        }
    });
    //统一输入框
    connect(ui->unified_edit,&QLineEdit::editingFinished,[this]() {
        if(ui->unified_check->isChecked()) {
            for(auto row=0;row<ui->scroll_layout->rowCount();row++) {
                auto* lvl_item=ui->scroll_layout->itemAtPosition(row,1);
                auto* lvl_edit=lvl_item?qobject_cast<QLineEdit*>(lvl_item->widget()):nullptr;

                auto* enable_item=ui->scroll_layout->itemAtPosition(row,2);
                auto* enable_check=enable_item?qobject_cast<QCheckBox*>(enable_item->widget()):nullptr;

                if(lvl_item&&enable_check) {
                    if(enable_check->isChecked()) {
                        lvl_edit->setText(ui->unified_edit->text());
                    }
                }

            }
        }

    });
    //最高最低选项
    connect(ui->all_255,&QRadioButton::clicked,[this]() {
        setValidator();
    });
    connect(ui->all_32767,&QRadioButton::clicked,[this]() {
        setValidator();
    });
    //一键满级
    connect(ui->max_lvl_button,&QPushButton::clicked,[this]() {
        if(ui->all_255->isChecked()) {
            setCheckedToLvl(255);
        }
        else if(ui->all_32767->isChecked()) {
            setCheckedToLvl(32767);
        }
    });
    //预设类型
    connect(ui->preinstall_combo,&QComboBox::currentIndexChanged,[this]() {
        auto found_enchs=gen_machine->get_best_enchantments().find(ui->preinstall_combo->currentText().toStdString());
        if(found_enchs!=gen_machine->get_best_enchantments().end()) {
            checkThroughList(found_enchs->second|std::views::transform([](auto& str){return QString::fromStdString(str);})|std::ranges::to<QStringList>());
            setCheckedToMaxSurvivalLvl();
        }
        else {
            checkAll(false);
        }
    });

    //生成按钮
    connect(ui->gen_cmd_button,&QPushButton::clicked,[this]() {
        updateMachineInGameName();
        updateMachineSelectedEnchs();
        ui->command_edit->setText(QString::fromStdString(gen_machine->genCommand(ui->with_slash_check->isChecked(),ui->nbt_postposition_check->isChecked())));
    });
    //复制按钮
    connect(ui->copy_to_clipboard_button,&QPushButton::clicked,[this]() {
        auto clip_board=QApplication::clipboard();
        clip_board->setText(ui->command_edit->toPlainText());
    });
}

SelectWindow::~SelectWindow() {
    delete ui;
}

void SelectWindow::loadDatas() {
    GenEnch::JsonReader json_reader;
    auto ench_types=json_reader.readEnchDatas(ench_types_path);
    auto ench_items=json_reader.readItems(ench_items_dir_path);

    gen_machine=std::make_unique<decltype(gen_machine)::element_type>(ui->version_combo_box->currentText().toStdString(),ench_types,ench_items);

    setItemNamesCombo();
    gen_machine->changeCurrentItem(ui->item_name_combo->currentText().toStdString());
    onChangeItem();
    // ui->ench_config_list->clear();
    // auto suitable_names=gen_machine->get_suitable_enchantments();
    // for(auto& name:suitable_names) {
    //     EnchSelectWidget ench_select_widget(ui->ench_config_list);
    //     // ench_select_widget.ui
    // }
}

void SelectWindow::clearAllDatas() {
    gen_machine.reset();
    ui->item_name_combo->clear();
    ui->item_subname_combo->clear();
    ui->command_edit->clear();
}

void SelectWindow::onChangeItem() {
    setSubItemNamesCombo();
    if(!ui->custom_ench_check_box->isChecked()) {
        showSuitableEnchs();
        showPreinstallCombo();
    }
}

void SelectWindow::setItemNamesCombo() {
    ui->item_name_combo->clear();
    auto items=gen_machine->get_enchant_items();
    QStringList string_list;
    std::ranges::for_each((*items)|std::views::keys,[&](auto& str){string_list<<QString::fromStdString(str);});
    ui->item_name_combo->addItems(string_list);
}

void SelectWindow::setSubItemNamesCombo() {
    ui->item_subname_combo->clear();
    auto current_item=gen_machine->get_current_item();
    if(current_item) {
        QStringList name_list;
        std::ranges::for_each(current_item->game_names|std::views::keys,[&](auto& str){name_list<<QString::fromStdString(str);});
        ui->item_subname_combo->addItems(name_list);
    }
}

void SelectWindow::showPreinstallCombo() {
    ui->preinstall_combo->clear();
    auto& best_enchantments=gen_machine->get_best_enchantments();

    QStringList name_list;
    name_list<<"无";
    std::ranges::for_each(best_enchantments|std::views::keys,[&](const auto& str){name_list<<QString::fromStdString(str);});
    ui->preinstall_combo->addItems(name_list);
}

void SelectWindow::clearAllShownEnchs() {
    QLayoutItem* child;
    while((child=ui->scroll_layout->takeAt(0))!=nullptr) {
        if(child->widget())delete child->widget();
        delete child;
    }
}

void SelectWindow::showSuitableEnchs() {
    clearAllShownEnchs();

    for(const auto& [i,name]:std::views::zip(std::ranges::iota_view(0),gen_machine->get_suitable_enchantments())) {
        auto* name_label=new QLabel(QString::fromStdString(name),this);
        ui->scroll_layout->addWidget(name_label,i,0);

        auto* lvl_edit=new QLineEdit(this);
        lvl_edit->setValidator(new QIntValidator(0,ui->all_255->isChecked()?127:32767));
        lvl_edit->setText("0");
        ui->scroll_layout->addWidget(lvl_edit,i,1);

        auto* enable_check=new QCheckBox(this);
        ui->scroll_layout->addWidget(enable_check,i,2);
    }
}

void SelectWindow::showAllEnchs() {
    clearAllShownEnchs();

    for(const auto& [i,name]:std::views::zip(std::ranges::iota_view(0),(*gen_machine->get_enchant_types())|std::views::transform([](auto& ench){return ench.name;}))) {
        auto* name_label=new QLabel(QString::fromStdString(name),this);
        ui->scroll_layout->addWidget(name_label,i,0);

        auto* lvl_edit=new QLineEdit(this);
        lvl_edit->setValidator(new QIntValidator(0,ui->all_255->isChecked()?127:32767));
        lvl_edit->setText("0");
        ui->scroll_layout->addWidget(lvl_edit,i,1);

        auto* enable_check=new QCheckBox(this);
        ui->scroll_layout->addWidget(enable_check,i,2);
    }
}

void SelectWindow::setValidator() {
    ui->unified_edit->setValidator(ui->all_255->isChecked()?&int255Validator:&int32767Validator);
    for(auto row=0;row<ui->scroll_layout->rowCount();row++) {

        QLabel* name_label;
        QLineEdit* lvl_edit;
        QCheckBox* enable_check;

        if(getAtRow(row,&name_label,&lvl_edit,&enable_check)) {
            lvl_edit->setValidator(ui->all_255->isChecked()?&int255Validator:&int32767Validator);
        }
    }
}

void SelectWindow::updateMachineInGameName() {
    gen_machine->get_target_in_game_name().clear();
    if(ui->custom_check_box->isChecked()) {
        gen_machine->get_target_in_game_name()=ui->custom_name_combo->currentText().toStdString();
    }
    else {
        gen_machine->trySettingInGameName(ui->item_subname_combo->currentText().toStdString());
    }
}

void SelectWindow::updateMachineSelectedEnchs() {
    gen_machine->selected_enchantment_storage.clear();
    for(auto row=0;row<ui->scroll_layout->rowCount();row++) {

        QLabel* name_label;
        QLineEdit* lvl_edit;
        QCheckBox* enable_check;

        if(getAtRow(row,&name_label,&lvl_edit,&enable_check)) {
            if(enable_check->isChecked()) {
                if(ui->custom_ench_check_box->isChecked()) {
                    gen_machine->forceAddEnch(name_label->text().toStdString(),lvl_edit->text().toInt());
                }
                else {
                    gen_machine->tryAddEnch(name_label->text().toStdString(),lvl_edit->text().toInt());
                }
            }
        }
    }
}

void SelectWindow::checkThroughList(const QStringList &enchs,bool check) {
    for(auto row=0;row<ui->scroll_layout->rowCount();row++) {

        QLabel* name_label;
        QLineEdit* lvl_edit;
        QCheckBox* enable_check;

        if(getAtRow(row,&name_label,&lvl_edit,&enable_check)) {
            if(enchs.contains(name_label->text())) {
                enable_check->setChecked(check);
            }
            else {
                enable_check->setChecked(!check);
            }
        }
    }
}

void SelectWindow::checkAll(bool check) {
    for(auto row=0;row<ui->scroll_layout->rowCount();row++) {

        QLabel* name_label;
        QLineEdit* lvl_edit;
        QCheckBox* enable_check;

        if(getAtRow(row,&name_label,&lvl_edit,&enable_check)) {
            enable_check->setChecked(check);
        }
    }
}

void SelectWindow::setCheckedToMaxSurvivalLvl() {
    for(auto row=0;row<ui->scroll_layout->rowCount();row++) {

        QLabel* name_label;
        QLineEdit* lvl_edit;
        QCheckBox* enable_check;

        if(getAtRow(row,&name_label,&lvl_edit,&enable_check)) {
            if(enable_check->isChecked()) {
                auto ench_type=gen_machine->tryGetEnchType(name_label->text().toStdString());
                if(ench_type!=nullptr) {
                    lvl_edit->setText(QString::number(ench_type->max_level));
                }
            }
        }
    }
}

void SelectWindow::setCheckedToLvl(uint32_t lvl) {
    auto lvl_str=QString::number(lvl);
    for(auto row=0;row<ui->scroll_layout->rowCount();row++) {

        QLabel* name_label;
        QLineEdit* lvl_edit;
        QCheckBox* enable_check;

        if(getAtRow(row,&name_label,&lvl_edit,&enable_check)) {
            if(enable_check->isChecked()) {
                lvl_edit->setText(lvl_str);
            }
        }
    }
}

bool SelectWindow::getAtRow(const int row,QLabel **name, QLineEdit **lvl, QCheckBox **enable) const {
    auto* label_item=ui->scroll_layout->itemAtPosition(row,0);
    auto* name_label=label_item?qobject_cast<QLabel*>(label_item->widget()):nullptr;

    auto* lvl_item=ui->scroll_layout->itemAtPosition(row,1);
    auto* lvl_edit=lvl_item?qobject_cast<QLineEdit*>(lvl_item->widget()):nullptr;

    auto* enable_item=ui->scroll_layout->itemAtPosition(row,2);
    auto* enable_check=enable_item?qobject_cast<QCheckBox*>(enable_item->widget()):nullptr;

    if(name_label&&lvl_edit&&enable_check) {
        *name=name_label;
        *lvl=lvl_edit;
        *enable=enable_check;
        return true;
    }
    *name=nullptr;
    *lvl=nullptr;
    *enable=nullptr;
    return false;
}
