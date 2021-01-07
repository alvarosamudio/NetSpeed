#include "WinDdeDockSetting.h"
#include "ui_WinDdeDockSetting.h"
//#include <QFile>
//#include <QByteArray>
//#include <QJsonDocument>
//#include <QJsonParseError>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <QColorDialog>
#include <QMouseEvent>
#include <QDebug>
using namespace std;

#define DATA_JSON_PATH "/home/xmuli/project/github/lfxNet/config.json"

WinDdeDockSetting::WinDdeDockSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WinDdeDockSetting)
{
    ui->setupUi(this);
    init();
}

WinDdeDockSetting::~WinDdeDockSetting()
{
    delete ui;
}

void WinDdeDockSetting::init()
{
    // 读入 json 文件到流中
    ifstream jfile(DATA_JSON_PATH);
    jfile >> m_js;

    // 控件的基本设置，其读写留其它函数完成
    ui->labTextColor->setAutoFillBackground(true);
    ui->labBackgroundColor->setAutoFillBackground(true);

    ui->spinBoxFractionalAccuracy->setRange(0, 100);
    ui->spinBoxFractionalAccuracy->setSingleStep(1);
    ui->spinBoxFractionalAccuracy->setSuffix(tr("位"));
    ui->spinBoxRefreshInterval->setRange(1000, 2147483647);
    ui->spinBoxRefreshInterval->setSingleStep(1000);
    ui->spinBoxRefreshInterval->setSuffix(tr("ms"));

    ui->labTextColor->installEventFilter(this);
    ui->labBackgroundColor->installEventFilter(this);

    ui->comboBoxUnitModel->setEditable(true);
    ui->fontComboBox->setEditable(true);
    ui->comboBoxDoubleClick->setEditable(true);

    connect(ui->btnSave, &QPushButton::clicked, this, &WinDdeDockSetting::onBtnSave);
    connect(ui->btnQuit, &QPushButton::clicked, this, &WinDdeDockSetting::onBtnQuit);
}

/*!
 * \brief WinDdeDockSetting::readConfig 从 config.json 读取 config.json 写入到 UI 控件显示
 */
void WinDdeDockSetting::readConfig()
{
    json jsColorAndFont = m_js["WinDdeDock"]["ColorAndFont"];
//    ui->fontComboBox, const int fontSize = jsColorAndFont["FontSize"];// TODO: 2021-01-07 字体赋值
    ui->spinBoxFontSize->setValue(jsColorAndFont["FontSize"]);
    QPalette palette;
    palette.setColor(QPalette::Background, QString::fromStdString(jsColorAndFont["TextColor"]));
    ui->labTextColor->setPalette(palette);
    palette.setColor(QPalette::Background, QString::fromStdString(jsColorAndFont["BackgroundColor"]));
    ui->labBackgroundColor->setPalette(palette);

    json jsDisplayText = m_js["WinDdeDock"]["DisplayText"];
    ui->lineLabUpload->setText(QString::fromStdString(jsDisplayText[0]["LabUpload"]));
    ui->lineLabDown->setText(QString::fromStdString(jsDisplayText[0]["LabDown"]));
    ui->lineLabCpu->setText(QString::fromStdString(jsDisplayText[0]["LabCpu"]));
    ui->lineLabMemory->setText(QString::fromStdString(jsDisplayText[0]["LabMemory"]));
    ui->lineLabDiskRead->setText(QString::fromStdString(jsDisplayText[0]["LabDiskRead"]));
    ui->lineLabDiskWrite->setText(QString::fromStdString(jsDisplayText[0]["LabDiskWrite"]));
    ui->checkBoxDisolayNet->setChecked(jsDisplayText[1]["DisolayNet"]);
    ui->checkBoxDisolayCPUAndMemory->setChecked(jsDisplayText[1]["DisolayCPUAndMemory"]);
    ui->checkBoxDisolayDisk->setChecked(jsDisplayText[1]["DisolayDisk"]);
    ui->checkBoxLocationExchangeNet->setChecked(jsDisplayText[2]["LocationExchangeNet"]);
    ui->checkBoxLocationExchangeCPUAndMenory->setChecked(jsDisplayText[2]["LocationExchangeCPUAndMenory"]);
    ui->checkBoxLocationExchangeDisk->setChecked(jsDisplayText[2]["LocationExchangeDisk"]);
    ui->spinBoxFractionalAccuracy->setValue(jsDisplayText[3]["FractionalAccuracy"]);
    ui->spinBoxRefreshInterval->setValue(jsDisplayText[3]["RefreshInterval"]);

    json jsUnitSetting = m_js["WinDdeDock"]["UnitSetting"];
    ui->comboBoxUnitModel->setCurrentText(QString::fromStdString(jsUnitSetting["UnitModel"]));
    if (jsUnitSetting["NetUnitIsByte"]) {
        ui->radioBtnByte->setChecked(true);
        ui->radioBtnBit->setChecked(false);
    } else {
        ui->radioBtnByte->setChecked(false);
        ui->radioBtnBit->setChecked(true);
    }

    // TODO: 2021-01-07 占用图模式未写
}

/*!
 * \brief WinDdeDockSetting::saveConfig 将 UI 控件的数值全部覆盖保存到 config.json 文件
 * \note 功能函数
 */
void WinDdeDockSetting::saveConfig()
{
    json &jsColorAndFont = m_js["WinDdeDock"]["ColorAndFont"];
    jsColorAndFont["FontSize"] = ui->spinBoxFontSize->value();
    jsColorAndFont["FontType"] = ui->fontComboBox->currentText().toStdString().c_str();
    jsColorAndFont["TextColor"] = ui->labTextColor->palette().color(QPalette::Background).name().toStdString().c_str();
    jsColorAndFont["BackgroundColor"] = ui->labBackgroundColor->palette().color(QPalette::Background).name().toStdString().c_str();

    json &jsDisplayText = m_js["WinDdeDock"]["DisplayText"];
    jsDisplayText[0]["LabUpload"] = ui->lineLabUpload->text().toStdString().c_str();
    jsDisplayText[0]["LabDown"] = ui->lineLabDown->text().toStdString().c_str();
    jsDisplayText[0]["LabCpu"] = ui->lineLabCpu->text().toStdString().c_str();
    jsDisplayText[0]["LabMemory"] = ui->lineLabMemory->text().toStdString().c_str();
    jsDisplayText[0]["LabDiskRead"] = ui->lineLabDiskRead->text().toStdString().c_str();
    jsDisplayText[0]["LabDiskWrite"] = ui->lineLabDiskWrite->text().toStdString().c_str();
    jsDisplayText[1]["DisolayNet"] = ui->checkBoxDisolayNet->checkState() == Qt::Checked;
    jsDisplayText[1]["DisolayCPUAndMemory"] = ui->checkBoxDisolayCPUAndMemory->checkState() == Qt::Checked;
    jsDisplayText[1]["DisolayDisk"] = ui->checkBoxDisolayDisk->checkState() == Qt::Checked;
    jsDisplayText[2]["LocationExchangeNet"] = ui->checkBoxLocationExchangeNet->checkState() == Qt::Checked;
    jsDisplayText[2]["LocationExchangeCPUAndMenory"] = ui->checkBoxLocationExchangeCPUAndMenory->checkState() == Qt::Checked;
    jsDisplayText[2]["LocationExchangeDisk"] = ui->checkBoxLocationExchangeDisk->checkState() == Qt::Checked;
    jsDisplayText[3]["FractionalAccuracy"] = ui->spinBoxFractionalAccuracy->value();
    jsDisplayText[3]["RefreshInterval"] = ui->spinBoxRefreshInterval->value();

    json &jsUnitSetting = m_js["WinDdeDock"]["UnitSetting"];
    jsUnitSetting["UnitModel"] = ui->comboBoxUnitModel->currentText().toStdString().c_str();
    bool isBety = ui->radioBtnByte->isChecked();
    if (isBety)
        jsUnitSetting["NetUnitIsByte"] = true;
    else
        jsUnitSetting["NetUnitIsByte"] = false;

    ofstream outFile(DATA_JSON_PATH);
    outFile << setw(2) << m_js << endl;
}

/*!
 * \brief WinDdeDockSetting::eventFilter 事件监视器
 * \param watched 被监视的控件
 * \param event 发生的事件
 * \return 是否成功
 * \note 用法 https://blog.csdn.net/xiezhongyuan07/article/details/79992099
 */
bool WinDdeDockSetting::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->labTextColor) {
        if (event->type() == QEvent::MouseButtonRelease) {
            QColor labTextColor = QColorDialog::getColor(ui->labTextColor->palette().color(QPalette::Background), this, tr("选择文本颜色"));
            QPalette palette;
            palette.setColor(QPalette::Background, labTextColor);
            ui->labTextColor->setPalette(palette);
            return true;
        }
    } else if (watched == ui->labBackgroundColor) {
        if (event->type() == QEvent::MouseButtonRelease) {
            QColor labBackgroundColor = QColorDialog::getColor(ui->labTextColor->palette().color(QPalette::Background), this, tr("选择背景颜色"));

            QPalette palette;
            palette.setColor(QPalette::Background, labBackgroundColor);
            ui->labTextColor->setPalette(palette);
            return true;
        }
    } else {

    }

    QWidget::eventFilter(watched, event);
}

/*!
 * \brief WinDdeDockSetting::onBtnSave
 * \param check
 */
void WinDdeDockSetting::onBtnSave(bool check)
{
    Q_UNUSED(check)
    saveConfig();
}

/*!
 * \brief WinDdeDockSetting::onBtnQuit
 * \param check
 */
void WinDdeDockSetting::onBtnQuit(bool check)
{
    Q_UNUSED(check)
    close();
}