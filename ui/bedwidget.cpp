#include "bedwidget.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>

#include "program/memory.h"
#include "graph/bedloader.hpp"
#include "graph/annotationsmanager.hpp"
#include "graph/assemblygraph.h"

BedWidget::BedWidget(QWidget *parent) : QWidget(parent) {
    static const QString label = "Load BED file";
    auto *button = new QPushButton(label, this);

    auto *layout = new QVBoxLayout();
    layout->addWidget(button);

    connect(button, &QPushButton::clicked, [this]() {
        g_annotationsManager->removeGroupByName(g_settings->bedAnnotationGroupName);
        QString bedFileName = QFileDialog::getOpenFileName(this, label, g_memory->rememberedPath);
        if (bedFileName.isNull()) return;
        auto bedLines = bed::load(bedFileName.toStdString());
        auto &annotationGroup = g_annotationsManager->createAnnotationGroup(g_settings->bedAnnotationGroupName);
        for (const auto &bedLine : bedLines) {
            auto nodeName = bedLine.chrom + (bedLine.strand == bed::Strand::REVERSE_COMPLEMENT ? "-" : "+");
            auto it = g_assemblyGraph->m_deBruijnGraphNodes.find(nodeName);
            if (it != g_assemblyGraph->m_deBruijnGraphNodes.end()) {
                auto &annotation = annotationGroup.annotationMap[it.value()].emplace_back(
                        std::make_unique<Annotation>(bedLine.chromStart, bedLine.chromEnd, bedLine.name));
                annotation->addView(std::make_unique<SolidView>(1, bedLine.itemRgb.toQColor()));
                annotation->addView(std::make_unique<BedThickView>(1.2, bedLine.itemRgb.toQColor(), bedLine.thickStart, bedLine.thickEnd));
                annotation->addView(std::make_unique<BedBlockView>(1.4, bedLine.itemRgb.toQColor(), bedLine.blocks));
            }
        }
    });

    setLayout(layout);
}