#pragma once

extern "C" void init_pixel_match_switcher();
extern "C" void free_pixel_match_switcher();

#include <QObject>
#include <QMutex>
#include <QImage>
#include <QPointer>

#include <string>
#include <vector>

#include <obs.hpp>

#include "pm-filter-ref.hpp"
#include "pm-structs.hpp"

struct obs_scene_item;
struct obs_scene;
struct pm_filter_data;
class PmDialog;

class PmCore : public QObject
{
    Q_OBJECT

    // interactions with the C components
    friend void init_pixel_match_switcher();
    friend void free_pixel_match_switcher();
    friend void on_frame_processed(struct pm_filter_data* filter);

public:
    PmCore();
    ~PmCore();

    std::vector<PmFilterRef> filters() const;
    PmFilterRef activeFilterRef() const;
    std::string scenesInfo() const;

    const QString &matchImgFilename() { return m_matchImgFilename; }
    PmMatchResults results() const;
    PmMatchConfig matchConfig() const;
    PmScenes scenes() const;
    PmSwitchConfig switchConfig() const;
    QSize videoBaseSize() const;
    gs_effect_t *drawMatchImageEffect() const { return m_drawMatchImageEffect; }

signals:
    void sigFrameProcessed();
    void sigImgSuccess(QString filename);
    void sigImgFailed(QString filename);
    void sigNewMatchResults(PmMatchResults);
    void sigScenesChanged(PmScenes);

public slots:
    void onOpenImage(QString filename);
    void onNewMatchConfig(PmMatchConfig);
    void onNewSceneConfig(PmSwitchConfig);

private slots:
    void onMenuAction();
    void onPeriodicUpdate();
    void onFrameProcessed();

protected:
    static void switchScene(OBSWeakSource& scene, OBSWeakSource& transition);
    void scanScenes();
    void updateActiveFilter();
    void supplyImageToFilter();
    void supplyConfigToFilter();

    static PmCore *m_instance;
    QPointer<PmDialog> m_dialog;

    mutable QMutex m_filtersMutex;
    std::vector<PmFilterRef> m_filters;
    PmFilterRef m_activeFilter;

    mutable QMutex m_matchConfigMutex;
    PmMatchConfig m_matchConfig;

    mutable QMutex m_resultsMutex;
    PmMatchResults m_results;

    mutable QMutex m_scenesMutex;
    PmScenes m_scenes;

    mutable QMutex m_switchConfigMutex;
    PmSwitchConfig m_switchConfig;

    QString m_matchImgFilename;
    QImage m_matchImg;
    gs_effect *m_drawMatchImageEffect = nullptr;
};