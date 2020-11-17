#pragma once

#include <QGroupBox>
#include <QMessageBox>
#include <QList>

class QComboBox;
class QPushButton;

class PmCore;

class PmPresetsWidget : public QGroupBox
{
    Q_OBJECT

public:
    PmPresetsWidget(PmCore* core, QWidget* parent);
    bool proceedWithExit();

signals:
    void sigMatchPresetSelect(std::string name);
    void sigMatchPresetSave(std::string name);
    void sigMatchPresetRemove(std::string name);
    void sigMultiMatchConfigReset();
    void sigMatchPresetExport(std::string filename, QList<std::string> presets);

protected slots:
    // local UI handlers
    void onPresetSelected();
    void onPresetRevert();
    void onPresetSave();
    void onPresetSaveAs();
    void onNewConfig();
    void onPresetRemove();
    void onPresetExport();

    // core events handlers
    void onAvailablePresetsChanged();
    void onActivePresetChanged();
    void onActivePresetDirtyStateChanged();

protected:
    QMessageBox::ButtonRole promptUnsavedProceed();

    static const char* k_unsavedPresetStr;

    QPushButton* prepareButton(const char* tooltip, const char* icoPath);

    PmCore* m_core;

    QComboBox* m_presetCombo;
    QPushButton* m_presetRevertButton;
    QPushButton* m_presetSaveButton;
    QPushButton* m_presetSaveAsButton;
    QPushButton* m_newConfigButton;
    QPushButton* m_presetRemoveButton;
    QPushButton *m_exportButton;
    QPushButton *m_importButton;
};
