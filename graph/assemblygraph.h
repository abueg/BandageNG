// Copyright 2017 Ryan Wick
// Copyright 2022 Anton Korobeynikov
// Copyright 2017 Andrey Zakharov

// This file is part of Bandage

// Bandage is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Bandage is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Bandage.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include "gfa.h"
#include "path.h"
#include "annotation.hpp"

#include "ui/mygraphicsscene.h"

#include "seq/sequence.hpp"
#include "parallel_hashmap/phmap.h"
#include "tsl/htrie_map.h"

#include <QString>
#include <QPair>
#include <QObject>
#include <vector>

class DeBruijnNode;
class DeBruijnEdge;
class MyProgressDialog;

class AssemblyGraphError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

class AssemblyGraphBuilder;


class AssemblyGraph : public QObject
{
    Q_OBJECT

    friend class AssemblyGraphBuilder;
    
public:
    AssemblyGraph();
    ~AssemblyGraph() override;

    //Nodes are stored in a map with a key of the node's name.
    tsl::htrie_map<char, DeBruijnNode*> m_deBruijnGraphNodes;

    using DeBruijnLink = QPair<DeBruijnNode*, DeBruijnNode*>;

    // Edges are stored in a map with a key of the starting and ending node
    // pointers.
    phmap::parallel_flat_hash_map<DeBruijnLink, DeBruijnEdge*> m_deBruijnGraphEdges;

    // Custom colors
    phmap::parallel_flat_hash_map<const DeBruijnNode*, QColor> m_nodeColors;
    // Custom labels
    phmap::parallel_flat_hash_map<const DeBruijnNode*, QString> m_nodeLabels;
    // Edge styles
    phmap::parallel_flat_hash_map<const DeBruijnEdge*, Qt::PenStyle> m_edgeStyles;
    // Edge colors
    phmap::parallel_flat_hash_map<const DeBruijnEdge*, QColor> m_edgeColors;

    // CSV data
    phmap::parallel_flat_hash_map<const DeBruijnNode*, QStringList> m_nodeCSVData;
    QStringList m_csvHeaders;
    // Tags
    phmap::parallel_flat_hash_map<const DeBruijnNode*, std::vector<gfa::tag>> m_nodeTags;
    phmap::parallel_flat_hash_map<const DeBruijnEdge*, std::vector<gfa::tag>> m_edgeTags;

    tsl::htrie_map<char, Path*> m_deBruijnGraphPaths;

    int m_kmer;
    int m_nodeCount;
    int m_edgeCount;
    int m_pathCount;
    long long m_totalLength;
    long long m_shortestContig;
    long long m_longestContig;
    double m_meanDepth;
    double m_firstQuartileDepth;
    double m_medianDepth;
    double m_thirdQuartileDepth;
    GraphFileType m_graphFileType;
    bool m_contiguitySearchDone;
    QString m_filename;
    QString m_depthTag;
    SequencesLoadedFromFasta m_sequencesLoadedFromFasta;

    void cleanUp();
    void createDeBruijnEdge(const QString& node1Name, const QString& node2Name,
                            int overlap = 0,
                            EdgeOverlapType overlapType = UNKNOWN_OVERLAP);
    void resetNodes();
    static QByteArray getReverseComplement(const QByteArray& forwardSequence);
    void resetEdges();
    double getMeanDepth(bool drawnNodesOnly = false);
    static double getMeanDepth(const std::vector<DeBruijnNode *> &nodes);
    static double getMeanDepth(const QList<DeBruijnNode *>& nodes);
    void resetNodeContiguityStatus();
    void determineGraphInfo();
    void clearGraphInfo();
    void recalculateAllDepthsRelativeToDrawnMean();
    void recalculateAllNodeWidths();

    bool loadGraphFromFile(const QString& filename);
    void markNodesToDraw(const std::vector<DeBruijnNode *>& startingNodes,
                         int nodeDistance);

    bool loadCSV(const QString& filename, QStringList * columns, QString * errormsg, bool * coloursLoaded);
    std::vector<DeBruijnNode *> getStartingNodes(QString * errorTitle,
                                                 QString * errorMessage,
                                                 bool doubleMode,
                                                 const QString& nodesList,
                                                 const QString& blastQueryName,
                                                 const QString& pathName) const;

    static bool checkIfStringHasNodes(QString nodesString);
    static QString generateNodesNotFoundErrorMessage(std::vector<QString> nodesNotInGraph,
                                              bool exact);
    std::vector<DeBruijnNode *> getNodesFromString(QString nodeNamesString,
                                                   bool exactMatch,
                                                   std::vector<QString> * nodesNotInGraph = nullptr) const;

    void setAllEdgesExactOverlap(int overlap);
    void autoDetermineAllEdgesExactOverlap();

    int getDrawnNodeCount() const;
    void deleteNodes(const std::vector<DeBruijnNode *> &nodes);
    void deleteEdges(const std::vector<DeBruijnEdge *> &edges);
    void duplicateNodePair(DeBruijnNode * node, MyGraphicsScene * scene);
    bool mergeNodes(QList<DeBruijnNode *> nodes, MyGraphicsScene * scene,
                    bool recalulateDepth);

    int mergeAllPossible(MyGraphicsScene * scene = 0,
                         MyProgressDialog * progressDialog = 0);

    void saveEntireGraphToFasta(const QString& filename);
    void saveEntireGraphToFastaOnlyPositiveNodes(const QString& filename);
    bool saveEntireGraphToGfa(const QString& filename);
    bool saveVisibleGraphToGfa(const QString& filename);
    void changeNodeName(const QString& oldName, const QString& newName);
    NodeNameStatus checkNodeNameValidity(const QString& nodeName) const;
    void changeNodeDepth(const std::vector<DeBruijnNode *> &nodes,
                         double newDepth);

    unsigned int getDeadEndCount() const;
    void getNodeStats(int * n50, int * shortestNode, int * firstQuartile, int * median, int * thirdQuartile, int * longestNode) const;
    void getGraphComponentCountAndLargestComponentSize(int * componentCount, int * largestComponentLength) const;
    double getMedianDepthByBase() const;

    long long getEstimatedSequenceLength(double medianDepthByBase) const;
    long long getTotalLengthMinusEdgeOverlaps() const;
    QPair<int, int> getOverlapRange() const;
    long long getTotalLengthOrphanedNodes() const;

    bool hasCustomColour(const DeBruijnNode* node) const;
    bool hasCustomColour(const DeBruijnEdge* edge) const;
    QColor getCustomColour(const DeBruijnNode* node) const;
    QColor getCustomColour(const DeBruijnEdge* edge) const;
    void setCustomColour(const DeBruijnNode* node, QColor color);
    void setCustomColour(const DeBruijnEdge* edge, QColor color);

    Qt::PenStyle getCustomStyle(const DeBruijnEdge* edge) const;
    void setCustomStyle(const DeBruijnEdge* edge, Qt::PenStyle style);
    bool hasCustomStyle(const DeBruijnEdge* edge) const;

    QString getCustomLabel(const DeBruijnNode* node) const;
    void setCustomLabel(const DeBruijnNode* node, QString newLabel);

    bool hasCsvData(const DeBruijnNode* node) const;
    QStringList getAllCsvData(const DeBruijnNode *node) const;
    std::optional<QString> getCsvLine(const DeBruijnNode *node, int i) const;
    void setCsvData(const DeBruijnNode* node, QStringList csvData);
    void clearCsvData(const DeBruijnNode* node);

    QColor getCustomColourForDisplay(const DeBruijnNode *node) const;
    QStringList getCustomLabelForDisplay(const DeBruijnNode *node) const;

    QString getGfaSegmentLine(const DeBruijnNode *node, const QString& depthTag) const;

    QString getUniqueNodeName(QString baseName) const;
    QString getNodeNameFromString(QString string) const;
private:
    std::vector<DeBruijnNode *> getNodesFromListExact(const QStringList& nodesList, std::vector<QString> * nodesNotInGraph) const;
    std::vector<DeBruijnNode *> getNodesFromListPartial(const QStringList& nodesList, std::vector<QString> * nodesNotInGraph) const;
    std::vector<DeBruijnNode *> getNodesInDepthRange(double min, double max) const;
    std::vector<int> makeOverlapCountVector();
    void clearAllCsvData();
    QString getNewNodeName(QString oldNodeName) const;

signals:
    void setMergeTotalCount(int totalCount);
    void setMergeCompletedCount(int completedCount);
};
