#include "pm-match-results-widget.hpp"
#include "pm-core.hpp"

#include <QLabel>
#include <QHBoxLayout>

PmMatchResultsWidget::PmMatchResultsWidget(PmCore* core, QWidget* parent)
: QGroupBox(parent)
, m_core(core)
{
    m_matchResultDisplay = new QLabel(this);
    m_matchResultDisplay->setTextFormat(Qt::RichText);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_matchResultDisplay);
    setLayout(mainLayout);

    // core event handlers
    connect(m_core, &PmCore::sigSelectMatchIndex,
        this, &PmMatchResultsWidget::onSelectMatchIndex, Qt::QueuedConnection);
    connect(m_core, &PmCore::sigNewMatchResults,
        this, &PmMatchResultsWidget::onNewMatchResults, Qt::QueuedConnection);

    // finish state init
    size_t selIdx = m_core->selectedConfigIndex();
    onSelectMatchIndex(selIdx, m_core->matchConfig(selIdx));
    onNewMatchResults(selIdx, m_core->matchResults(selIdx));
}

void PmMatchResultsWidget::onNewMatchResults(
    size_t matchIdx, PmMatchResults results)
{
    if (matchIdx != m_matchIndex) return;

    QString matchLabel = results.isMatched
        ? obs_module_text("<font color=\"Green\">[MATCHED]</font>")
        : obs_module_text("<font color=\"Red\">[NO MATCH]</font>");

    QString resultStr = QString(
        obs_module_text("%1 %2 out of %3 pixels matched (%4 %)"))
        .arg(matchLabel)
        .arg(results.numMatched)
        .arg(results.numCompared)
        .arg(double(results.percentageMatched), 0, 'f', 1);
    m_matchResultDisplay->setText(resultStr);
}

void PmMatchResultsWidget::onSelectMatchIndex(
    size_t matchIndex, PmMatchConfig cfg)
{
    m_matchIndex = matchIndex;
    setTitle(
        QString(obs_module_text("Match Result #%1")).arg(matchIndex + 1));
    m_matchResultDisplay->setText(obs_module_text("N/A"));
}