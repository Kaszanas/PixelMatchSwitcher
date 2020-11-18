#include "pm-presets-widget.hpp"
#include "pm-preset-exists-dialog.hpp"
#include "pm-core.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QFileDialog>

const char* PmPresetsWidget::k_unsavedPresetStr
    = obs_module_text("<unsaved preset>");

PmPresetsWidget::PmPresetsWidget(PmCore* core, QWidget* parent)
: QGroupBox(obs_module_text("Presets"), parent)
, m_core(core)

{
    const Qt::ConnectionType qc = Qt::QueuedConnection;

    // top level layout
    QHBoxLayout* presetLayout = new QHBoxLayout;
    setLayout(presetLayout);

    // preset controls
    m_presetCombo = new QComboBox(this);
    m_presetCombo->setInsertPolicy(QComboBox::InsertAlphabetically);
    m_presetCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    connect(m_presetCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onPresetSelected()));
    presetLayout->addWidget(m_presetCombo);

    m_presetSaveButton = prepareButton(obs_module_text("Save Preset"),
        ":/res/images/icons8-save-32.png");
    connect(m_presetSaveButton, &QPushButton::released,
            this, &PmPresetsWidget::onPresetSave, qc);
    presetLayout->addWidget(m_presetSaveButton);

    m_presetSaveAsButton = prepareButton(obs_module_text("Save Preset As"),
        ":/res/images/icons8-save-as-32.png");
    connect(m_presetSaveAsButton, &QPushButton::released,
        this, &PmPresetsWidget::onPresetSaveAs, qc);
    presetLayout->addWidget(m_presetSaveAsButton);

    m_presetRevertButton = prepareButton(obs_module_text("Revert Changes"),
        ":/res/images/revert.svg");
    m_presetRevertButton->setProperty("themeID", QVariant("revertIcon"));
    connect(m_presetRevertButton, &QPushButton::released,
            this, &PmPresetsWidget::onPresetRevert, qc);
    presetLayout->addWidget(m_presetRevertButton);

    m_newConfigButton = prepareButton(obs_module_text("New Configuration"),
        ":/res/images/icons8-file.svg");
    connect(m_newConfigButton, &QPushButton::released,
            this, &PmPresetsWidget::onNewConfig, qc);
    presetLayout->addWidget(m_newConfigButton);

    m_presetRemoveButton = prepareButton(obs_module_text("Remove Preset"),
        ":/res/images/icons8-trash.svg");
    connect(m_presetRemoveButton, &QPushButton::released,
        this, &PmPresetsWidget::onPresetRemove, qc);
    presetLayout->addWidget(m_presetRemoveButton);

    // divider
    QFrame *divider = new QFrame(this);
    divider->setFrameShape(QFrame::VLine);
    divider->setFrameShadow(QFrame::Plain);
    divider->setFixedWidth(1);
    presetLayout->addWidget(divider);

    m_presetImportButton = prepareButton(obs_module_text("Import Preset(s)"),
        ":/res/images/icons8-import-32.png");
    connect(m_presetImportButton, &QPushButton::released,
            this, &PmPresetsWidget::onPresetImport, qc);
    presetLayout->addWidget(m_presetImportButton);

    m_presetExportButton = prepareButton(obs_module_text("Export Preset(s)"),
        ":/res/images/icons8-export-32.png");
    connect(m_presetExportButton, &QPushButton::released,
            this, &PmPresetsWidget::onPresetExport, qc);
    presetLayout->addWidget(m_presetExportButton);

    // core event handlers
    connect(m_core, &PmCore::sigAvailablePresetsChanged,
            this, &PmPresetsWidget::onAvailablePresetsChanged, qc);
    connect(m_core, &PmCore::sigActivePresetChanged,
            this, &PmPresetsWidget::onActivePresetChanged, qc);
    connect(m_core, &PmCore::sigActivePresetDirtyChanged,
            this, &PmPresetsWidget::onActivePresetDirtyStateChanged, qc);
    connect(m_core, &PmCore::sigPresetsImportAvailable,
            this, &PmPresetsWidget::onPresetsImportAvailable, qc);

    // local signals -> core slots
    connect(this, &PmPresetsWidget::sigMatchPresetSave,
            m_core, &PmCore::onMatchPresetSave, qc);
    connect(this, &PmPresetsWidget::sigMatchPresetSelect,
            m_core, &PmCore::onMatchPresetSelect, qc);
    connect(this, &PmPresetsWidget::sigMatchPresetRemove,
            m_core, &PmCore::onMatchPresetRemove, qc);
    connect(this, &PmPresetsWidget::sigMultiMatchConfigReset,
            m_core, &PmCore::onMultiMatchConfigReset, qc);
    connect(this, &PmPresetsWidget::sigMatchPresetExport,
            m_core, &PmCore::onMatchPresetExport, qc);
    connect(this, &PmPresetsWidget::sigMatchPresetsImport,
            m_core, &PmCore::onMatchPresetsImport, qc);
    connect(this, &PmPresetsWidget::sigMatchPresetsAppend,
            m_core, &PmCore::onMatchPresetsAppend, qc);

    // finish init state
    onAvailablePresetsChanged();
    onActivePresetChanged();
    onActivePresetDirtyStateChanged();
}

void PmPresetsWidget::onAvailablePresetsChanged()
{
    auto presetNames = m_core->matchPresetNames();
    m_presetCombo->blockSignals(true);
    m_presetCombo->clear();
    for (const auto &name : presetNames) {
        m_presetCombo->addItem(name.data());
    }
    m_presetCombo->blockSignals(false);
}

void PmPresetsWidget::onActivePresetChanged()
{
    std::string activePreset = m_core->activeMatchPresetName();

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
    m_presetExportButton->setEnabled(!activePreset.empty());
}

void PmPresetsWidget::onActivePresetDirtyStateChanged()
{
    bool dirty = m_core->matchConfigDirty();
    std::string activePreset = m_core->activeMatchPresetName();
    m_presetRevertButton->setEnabled(dirty && !activePreset.empty());
    m_presetSaveButton->setEnabled(dirty);
    setTitle(dirty ? obs_module_text("Preset (*)") : obs_module_text("Preset"));
}

void PmPresetsWidget::onPresetsImportAvailable(PmMatchPresets availablePresets)
{
    // TODO: allow selection
    QList<std::string> selectedPresets = availablePresets.keys();

    PmMatchPresets newPresets;
    std::string firstImported;
    PmDuplicateNameReaction defaultReaction
        = PmDuplicateNameReaction::Undefined;
    size_t importCountRemaining = availablePresets.count(); 

    for (std::string presetName : availablePresets.keys()) {
        --importCountRemaining;

        while (m_core->matchPresetExists(presetName)) {
            // preset with this name exists
            PmMultiMatchConfig checkValue = availablePresets[presetName];
            if (checkValue == m_core->matchPresetByName(presetName)) {
                // it's the same as in existing configuration. don't worry about it
                break;
            }
            // new preset, same name, different configuration.
            PmDuplicateNameReaction reaction;
            if (defaultReaction != PmDuplicateNameReaction::Undefined) {
                // user previously requested to handle all duplicates the same way
                reaction = defaultReaction;
            } else {
                // user needs to make a choice to react
                PmPresetExistsDialog* choiceDialog = new PmPresetExistsDialog(
                    presetName, importCountRemaining > 0, this);
                reaction = choiceDialog->choice();
                if (choiceDialog->applyToAll()) {
                    // user requests a default reaction for subsequent duplicates
                    defaultReaction = reaction;
                }
            }
            switch (reaction) {
            case PmDuplicateNameReaction::Abort:
                // abort everything
                return;
            case PmDuplicateNameReaction::Skip:
                // break out of the name check loop and skip preset
                break;  break; continue;
            case PmDuplicateNameReaction::Replace:
                // break out of the name check loop and allow overwrite
                break; break; 
            case PmDuplicateNameReaction::Rename: {
                // ask for a new name
                bool ok;
                QString presetNameQstr = QInputDialog::getText(
                     this, obs_module_text("Rename Imported Preset"),
                    obs_module_text("Enter Name: "), QLineEdit::Normal,
                     QString(presetName.data()) + " (new)", &ok);
                if (ok)
                    presetName = presetNameQstr.toUtf8().data();
                }
                break;
            }
        }

        newPresets[presetName] = availablePresets[presetName];

        if (firstImported.empty()) {
            firstImported = presetName;
        }
    }
    if (firstImported.size()) {
        emit sigMatchPresetsAppend(newPresets);
        emit sigMatchPresetSelect(firstImported);
    }
}

QPushButton* PmPresetsWidget::prepareButton(
    const char* tooltip, const char* icoPath)
{
    QIcon icon;
    icon.addFile(icoPath, QSize(), QIcon::Normal, QIcon::Off);

    QPushButton* ret = new QPushButton(icon, "", this);
    ret->setToolTip(tooltip);
    ret->setIcon(icon);
    ret->setIconSize(QSize(16, 16));
    ret->setMaximumSize(22, 22);
    ret->setFlat(true);
    //ret->setProperty("themeID", QVariant(themeId));
    ret->setFocusPolicy(Qt::NoFocus);

    return ret;
}

void PmPresetsWidget::onPresetSelected()
{
    std::string selPreset = m_presetCombo->currentText().toUtf8().data();
    std::string activePreset = m_core->activeMatchPresetName();

    if (selPreset != activePreset && m_core->matchConfigDirty()) {
        auto role = promptUnsavedProceed();
        if (role == QMessageBox::RejectRole) {
            m_presetCombo->blockSignals(true);
            m_presetCombo->setCurrentText(
                activePreset.size() ? activePreset.data() : k_unsavedPresetStr);
            m_presetCombo->blockSignals(false);
            return;
        } else {
            if (role == QMessageBox::YesRole) {
                if (activePreset.empty()) {
                    onPresetSaveAs();
                } else {
                    emit sigMatchPresetSave(activePreset);
                }
            }
        }
    }
    emit sigMatchPresetSelect(selPreset);
}

void PmPresetsWidget::onPresetRevert()
{
    // TODO: make less hacky
    std::string presetName = m_core->activeMatchPresetName();
    if (presetName.size() && m_core->matchConfigDirty()) {
        emit sigMatchPresetSelect("");
        emit sigMatchPresetSelect(presetName);
    }
}

void PmPresetsWidget::onPresetSave()
{
    std::string presetName = m_core->activeMatchPresetName();
    if (presetName.empty()) {
        onPresetSaveAs();
    } else {
        emit sigMatchPresetSave(presetName);
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
        int ret = QMessageBox::warning(this, 
            obs_module_text("Preset Exists"),
            QString(obs_module_text("Overwrite preset \"%1\"?"))
               .arg(presetNameQstr),
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

        if (ret != QMessageBox::Yes) return;
    }

    emit sigMatchPresetSave(presetName);
}

void PmPresetsWidget::onNewConfig()
{
    std::string activePreset = m_core->activeMatchPresetName();

    if (m_core->matchConfigDirty()) {
        auto role = promptUnsavedProceed();
        if (role == QMessageBox::RejectRole) {
            return;
        } else {
            if (role == QMessageBox::YesRole) {
                if (activePreset.empty()) {
                    onPresetSaveAs();
                } else {
                    emit sigMatchPresetSave(activePreset);
                }
            }
        }
    }

    if (activePreset.empty()) {
        emit sigMultiMatchConfigReset();
    } else {
        emit sigMatchPresetSelect("");
    }
}

void PmPresetsWidget::onPresetRemove()
{
    std::string oldPreset = m_core->activeMatchPresetName();
    if (oldPreset.size() && m_core->matchPresetSize(oldPreset)) {
        int ret = QMessageBox::warning(this,
            obs_module_text("Remove preset?"),
            QString(obs_module_text("Really remove preset \"%1\"?"))
                .arg(oldPreset.data()),
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
        if (ret != QMessageBox::Yes)
            return;
    }
    emit sigMatchPresetRemove(oldPreset);
}

void PmPresetsWidget::onPresetExport()
{
    std::string activePresetName = m_core->activeMatchPresetName();

    QFileDialog saveDialog(
        this, obs_module_text("Export Preset(s) XML"), QString(),
        PmConstants::k_xmlFilenameFilter);
    saveDialog.selectFile(activePresetName.data());
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.exec();

    QStringList selectedFiles = saveDialog.selectedFiles();
    if (selectedFiles.empty()) return;
    QString qstrFilename = selectedFiles.first();
    std::string filename(qstrFilename.toUtf8().data());

    // TODO: allow exporting multiple presets

    QList<std::string> selectedPresets { activePresetName };
    emit sigMatchPresetExport(filename, selectedPresets);
}

void PmPresetsWidget::onPresetImport()
{
    QString qstrFilename = QFileDialog::getOpenFileName(
        this, obs_module_text("Import Presets(s) XML"), QString(),
        PmConstants::k_xmlFilenameFilter);
    std::string filename = qstrFilename.toUtf8().data();
    if (filename.size()) {
        emit sigMatchPresetsImport(filename);
    }
}


QMessageBox::ButtonRole PmPresetsWidget::promptUnsavedProceed()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(obs_module_text("Unsaved changes"));
    msgBox.setText(obs_module_text(
        "What would you like to do with unsaved changes?"));
    msgBox.addButton(obs_module_text("Save"), QMessageBox::YesRole);
    msgBox.addButton(obs_module_text("Discard"), QMessageBox::NoRole);
    msgBox.addButton(obs_module_text("Cancel"), QMessageBox::RejectRole);

    msgBox.exec();

    return msgBox.buttonRole(msgBox.clickedButton());
}

bool PmPresetsWidget::proceedWithExit()
{
    auto activePreset = m_core->activeMatchPresetName();

    if (m_core->matchConfigDirty()) {
        auto role = promptUnsavedProceed();
        if (role == QMessageBox::RejectRole) {
            return false;
        } else {
            if (role == QMessageBox::YesRole) {
                if (activePreset.empty()) {
                    onPresetSaveAs();
                } else {
                    emit sigMatchPresetSave(activePreset);
                }
            } else {
                onPresetRevert();
            }
        }
    }
    return true;
}
