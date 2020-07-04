#include "pm-presets-widget.hpp"
#include "pm-core.hpp"

#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>

const char* PmPresetsWidget::k_unsavedPresetStr
    = obs_module_text("<unsaved preset>");

PmPresetsWidget::PmPresetsWidget(PmCore* core, QWidget* parent)
: QWidget(parent)
, m_core(core)
{
    // preset controls
    QHBoxLayout* presetLayout = new QHBoxLayout;

    QLabel* presetsLabel = new QLabel(obs_module_text("Preset: "), this);
    presetLayout->addWidget(presetsLabel);

    m_presetCombo = new QComboBox(this);
    m_presetCombo->setInsertPolicy(QComboBox::InsertAlphabetically);
    m_presetCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(m_presetCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onPresetSelected()));
    presetLayout->addWidget(m_presetCombo);

    m_presetSaveButton = new QPushButton(obs_module_text("Save"), this);
    connect(m_presetSaveButton, &QPushButton::released,
        this, &PmPresetsWidget::onPresetSave, Qt::QueuedConnection);
    presetLayout->addWidget(m_presetSaveButton);

    m_presetSaveAsButton = new QPushButton(obs_module_text("Save As"), this);
    connect(m_presetSaveAsButton, &QPushButton::released,
        this, &PmPresetsWidget::onPresetSaveAs, Qt::QueuedConnection);
    presetLayout->addWidget(m_presetSaveAsButton);

    m_presetResetButton = new QPushButton(obs_module_text("Reset"), this);
    connect(m_presetResetButton, &QPushButton::released,
        this, &PmPresetsWidget::onConfigReset, Qt::QueuedConnection);
    presetLayout->addWidget(m_presetResetButton);

    m_presetRemoveButton = new  QPushButton(obs_module_text("Remove"), this);
    connect(m_presetRemoveButton, &QPushButton::released,
        this, &PmPresetsWidget::onPresetRemove, Qt::QueuedConnection);
    presetLayout->addWidget(m_presetRemoveButton);
    
    setLayout(presetLayout);

    // core event handlers
    connect(m_core, &PmCore::sigAvailablePresetsChanged,
        this, &PmPresetsWidget::onAvailablePresetsChanged, Qt::QueuedConnection);
    connect(m_core, &PmCore::sigActivePresetChanged,
        this, &PmPresetsWidget::onActivePresetChanged, Qt::QueuedConnection);
    //connect(m_core, &PmCore::sigNewMatchResults,
    //    this, &PmPresetsWidget::onNewMatchResults, Qt::QueuedConnection);

    // local signals -> core slots
    //connect(this, &PmPresetsWidget::sigNewUiConfig,
    //    m_core, &PmCore::onChangedMatchConfig, Qt::QueuedConnection);
    connect(this, &PmPresetsWidget::sigSaveMatchPreset,
        m_core, &PmCore::onSaveMatchPreset, Qt::QueuedConnection);
    connect(this, &PmPresetsWidget::sigSelectActiveMatchPreset,
        m_core, &PmCore::onSelectActiveMatchPreset, Qt::QueuedConnection);
    connect(this, &PmPresetsWidget::sigRemoveMatchPreset,
        m_core, &PmCore::onRemoveMatchPreset, Qt::QueuedConnection);

    // finish init
    onAvailablePresetsChanged();
    onActivePresetChanged();
}

void PmPresetsWidget::onAvailablePresetsChanged()
{
    PmMatchPresets presets = m_core->matchPresets();
    m_presetCombo->blockSignals(true);
    m_presetCombo->clear();
    for (const auto &name : presets.keys()) {
        m_presetCombo->addItem(name.data());
    }
    m_presetCombo->blockSignals(false);
}

void PmPresetsWidget::onActivePresetChanged()
{
    std::string activePreset = m_core->activeMatchPresetName();
    bool dirty = m_core->matchConfigDirty();

    m_presetCombo->blockSignals(true);
    int findPlaceholder = m_presetCombo->findText(k_unsavedPresetStr);
    if (activePreset.empty()) {
        if (findPlaceholder == -1) {
            m_presetCombo->addItem(k_unsavedPresetStr);
        }
        m_presetCombo->setCurrentText(k_unsavedPresetStr);
    } else {
        if (findPlaceholder != -1) {
            m_presetCombo->removeItem(findPlaceholder);
        }
        m_presetCombo->setCurrentText(activePreset.data());
    }
    m_presetCombo->blockSignals(false);

    m_presetRemoveButton->setEnabled(!activePreset.empty());
    m_presetSaveButton->setEnabled(dirty);
}

void PmPresetsWidget::onPresetSelected()
{
    std::string selPreset = m_presetCombo->currentText().toUtf8().data();
    //PmMatchConfig presetConfig = m_core->matchPresetByName(selPreset);
    emit sigSelectActiveMatchPreset(selPreset);
    //configToUi(presetConfig);
    //updateFilterDisplaySize(presetConfig, m_prevResults);
}

void PmPresetsWidget::onPresetSave()
{
    std::string presetName = m_core->activeMatchPresetName();
    if (presetName.empty()) {
        onPresetSaveAs();
    } else {
        emit sigSaveMatchPreset(presetName);
    }
}

void PmPresetsWidget::onPresetSaveAs()
{
    bool ok;
    QString presetNameQstr = QInputDialog::getText(
        this, obs_module_text("Save Preset"), obs_module_text("Enter Name: "),
        QLineEdit::Normal, QString(), &ok);

    std::string presetName = presetNameQstr.toUtf8().data();

    if (!ok || presetName.empty()) return;

    if (m_core->activeMatchPresetName() != presetName
        && m_core->matchPresetExists(presetName)) {
        int ret = QMessageBox::warning(this, "Preset Exists",
            QString("Overwrite preset \"%1\"?").arg(presetNameQstr),
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

        if (ret != QMessageBox::Yes) return;
    }

    emit sigSaveMatchPreset(presetName);
}

void PmPresetsWidget::onConfigReset()
{
    //PmMatchConfig config; // defaults
    //configToUi(config);
    //updateFilterDisplaySize(config, m_prevResults);
    //emit sigNewUiConfig(config);
    emit sigSelectActiveMatchPreset("");
}

void PmPresetsWidget::onPresetRemove()
{
    auto presets = m_core->matchPresets();
    std::string oldPreset = m_core->activeMatchPresetName();
    emit sigRemoveMatchPreset(oldPreset);
    //for (auto p : presets) {
    //    if (p.first != oldPreset) {
            //configToUi(p.second);
    //        emit sigSelectActiveMatchPreset(p.first);
    //        break;
    //    }
    //}
}
