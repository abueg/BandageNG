//Copyright 2017 Ryan Wick

//This file is part of Bandage

//Bandage is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//Bandage is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with Bandage.  If not, see <http://www.gnu.org/licenses/>.


#ifndef GLOBALS_H
#define GLOBALS_H

#include <colormap/tinycolormap_fwd.hpp>

#include <QString>
#include <QProcess>
#include <QSharedPointer>
#include <QColor>

#include <vector>

class Settings;
class Memory;
class MyGraphicsView;
class BlastSearch;
class AssemblyGraph;
class AnnotationsManager;

enum GraphScope {WHOLE_GRAPH, AROUND_NODE, AROUND_PATHS, AROUND_BLAST_HITS, DEPTH_RANGE};
enum ContiguityStatus {STARTING, CONTIGUOUS_STRAND_SPECIFIC,
                       CONTIGUOUS_EITHER_STRAND, MAYBE_CONTIGUOUS,
                       NOT_CONTIGUOUS};
enum NodeDragging {ONE_PIECE, NEARBY_PIECES, ALL_PIECES, NO_DRAGGING};
enum ZoomSource {MOUSE_WHEEL, SPIN_BOX, KEYBOARD, GESTURE};
enum UiState {NO_GRAPH_LOADED, GRAPH_LOADED, GRAPH_DRAWN};
enum NodeLengthMode {AUTO_NODE_LENGTH, MANUAL_NODE_LENGTH};
enum GraphFileType {LAST_GRAPH, FASTG, GFA, TRINITY, ASQG, PLAIN_FASTA, ANY_FILE_TYPE,
                    UNKNOWN_FILE_TYPE};
enum SequenceType {NUCLEOTIDE, PROTEIN, EITHER_NUCLEOTIDE_OR_PROTEIN};
enum BlastUiState {BLAST_DB_NOT_YET_BUILT, BLAST_DB_BUILD_IN_PROGRESS,
                   BLAST_DB_BUILT_BUT_NO_QUERIES,
                   READY_FOR_BLAST_SEARCH, BLAST_SEARCH_IN_PROGRESS,
                   BLAST_SEARCH_COMPLETE};
enum CommandLineCommand {NO_COMMAND, BANDAGE_LOAD, BANDAGE_INFO, BANDAGE_IMAGE,
                         BANDAGE_DISTANCE, BANDAGE_QUERY_PATHS, BANDAGE_REDUCE};
enum EdgeOverlapType {UNKNOWN_OVERLAP, EXACT_OVERLAP,
                      AUTO_DETERMINED_EXACT_OVERLAP, JUMP};
enum NodeNameStatus {NODE_NAME_OKAY, NODE_NAME_TAKEN, NODE_NAME_CONTAINS_TAB,
                     NODE_NAME_CONTAINS_NEWLINE, NODE_NAME_CONTAINS_COMMA,
                     NODE_NAME_CONTAINS_SPACE};
enum SequencesLoadedFromFasta {NOT_READY, NOT_TRIED, TRIED};

// FIXME: factor out
enum ColorMap : int {
    Viridis = 0,
    Parula, Heat, Jet, Turbo, Hot, Gray, Magma, Inferno, Plasma, Cividis, Github, Cubehelix
};

using AnnotationGroupId = int;
using ViewId = int;


//Some of the program's common components are made global, so they don't have
//to be passed around as parameters.
extern QSharedPointer<Settings> g_settings;
extern QSharedPointer<Memory> g_memory;
extern MyGraphicsView * g_graphicsView;
extern double g_absoluteZoom;
extern QSharedPointer<BlastSearch> g_blastSearch;
extern QSharedPointer<AssemblyGraph> g_assemblyGraph;
extern std::shared_ptr<AnnotationsManager> g_annotationsManager;


//Functions for formatting numbers are used in many places, and are made global.
QString formatIntForDisplay(int num);
QString formatIntForDisplay(long long num);
QString formatDoubleForDisplay(double num, int decimalPlacesToDisplay);
QString formatDepthForDisplay(double depth);

std::vector<QColor> getPresetColours();
QString getColourName(QColor colour);
ColorMap colorMapFromName(const QString& name);
tinycolormap::ColormapType colorMap(ColorMap colorMap);
QString getColorMapName(ColorMap colorMap);

// Often used function for access const reference to value in map even if it doesn't exist
template <typename K, typename V, template<typename, typename, typename...> typename MapLike>
const V &getFromMapOrDefaultConstructed(const MapLike<K, V> &map, const K &key) {
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    } else {
        static const V defaultConstructed{};
        return defaultConstructed;
    }
}


#endif // GLOBALS_H
