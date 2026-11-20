#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>
#include <iomanip>

using namespace std;

const int ROWS = 6;
const int COLS = 6;

// 1. WEIGHTED GRID (-1 = Wall, 1 to 9 = Movement Cost/Weight)
int grid[ROWS][COLS] = {
    {1,  1,  5,  1,  1,  1},
    {1, -1,  5, -1,  2,  1},
    {1, -1,  1,  1,  9,  1},
    {1,  1,  1, -1,  1,  1},
    {5,  5, -1,  1,  1,  1},
    {1,  1,  1,  1,  5,  1}
};

pair<int, int> startNode = {0, 0};
pair<int, int> endNode   = {5, 5};

// Direction arrays: Up, Down, Left, Right
int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};

// 2. PROJECT RECORDS (Mandatory: At least 15 records for CRUD, Search & Sort)
struct TerrainRecord {
    int cellID;
    int row, col;
    string terrainType; // e.g., "Road", "Mud", "Water", "Wall"
    int weight;         // -1 for Wall, 1-9 for walkable
};

vector<TerrainRecord> records;

// Load 15+ sample records directly from the grid
void loadSampleRecords() {
    records.clear();
    int id = 101;
    for (int r = 0; r < 3; r++) { // Loads first 18 cells (3x6) as records
        for (int c = 0; c < COLS; c++) {
            string type = "Road";
            if (grid[r][c] == -1) type = "Wall";
            else if (grid[r][c] >= 5) type = "Mud/Swamp";
            else if (grid[r][c] > 1) type = "Grass";
            records.push_back({id++, r, c, type, grid[r][c]});
        }
    }
    cout << "[OK] Loaded " << records.size() << " sample grid cell records.\n";
}

// 3. DISPLAY GRID (Highlights path with '*' if provided)
void printGrid(vector<vector<bool>> isPath) {
    cout << "\n--- WEIGHTED GRID (S=Start, E=End, #=Wall, *=Path, 1-9=Cost) ---\n";
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (make_pair(r, c) == startNode) cout << " S ";
            else if (make_pair(r, c) == endNode) cout << " E ";
            else if (isPath[r][c]) cout << " * ";
            else if (grid[r][c] == -1) cout << " # ";
            else cout << " " << grid[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "---------------------------------------------------------------\n";
}

// 4. PATH RECONSTRUCTION (Backtracks from End to Start using parent matrix)
void traceAndPrintPath(vector<vector<pair<int, int>>>& parent, string algoName, int nodesExplored) {
    vector<vector<bool>> isPath(ROWS, vector<bool>(COLS, false));
    pair<int, int> curr = endNode;

    if (parent[endNode.first][endNode.second] == make_pair(-1, -1)) {
        cout << "\n[" << algoName << "] No path found! Nodes Explored: " << nodesExplored << "\n";
        return;
    }

    int totalCost = 0;
    int steps = 0;
    while (curr != startNode) {
        isPath[curr.first][curr.second] = true;
        totalCost += grid[curr.first][curr.second];
        steps++;
        curr = parent[curr.first][curr.second];
    }

    cout << "\n=== " << algoName << " RESULTS ===";
    printGrid(isPath);
    cout << "Steps Taken    : " << steps << "\n";
    cout << "Total Path Cost: " << totalCost << "\n";
    cout << "Nodes Explored : " << nodesExplored << "\n";
}



// 5. BFS (Breadth-First Search - Uses Queue)

void runBFS(bool showSteps = false) {
    vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));
    vector<vector<pair<int, int>>> parent(ROWS, vector<pair<int, int>>(COLS, {-1, -1}));
    queue<pair<int, int>> q;

    q.push(startNode);
    visited[startNode.first][startNode.second] = true;
    int nodesExplored = 0;

    if (showSteps) cout << "\n[BFS Traversal Order]: ";

    while (!q.empty()) {
        pair<int, int> curr = q.front();
        q.pop();
        nodesExplored++;

        if (showSteps) cout << "(" << curr.first << "," << curr.second << ") -> ";

        if (curr == endNode) break; // Reached destination!

        for (int i = 0; i < 4; i++) {
            int nr = curr.first + dr[i];
            int nc = curr.second + dc[i];

            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS &&
                !visited[nr][nc] && grid[nr][nc] != -1) {
                visited[nr][nc] = true;
                parent[nr][nc] = curr; // Save breadcrumb
                q.push({nr, nc});
            }
        }
    }
    if (showSteps) cout << "DONE\n";
    traceAndPrintPath(parent, "BFS (Unweighted Shortest Steps)", nodesExplored);
}

// 6. DFS (Depth-First Search - Uses Recursion)

bool dfsHelper(int r, int c, vector<vector<bool>>& visited,
               vector<vector<pair<int, int>>>& parent, int& nodesExplored, bool showSteps) {
    visited[r][c] = true;
    nodesExplored++;
    if (showSteps) cout << "(" << r << "," << c << ") -> ";

    if (make_pair(r, c) == endNode) return true; // Found target!

    for (int i = 0; i < 4; i++) {
        int nr = r + dr[i];
        int nc = c + dc[i];

        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS &&
            !visited[nr][nc] && grid[nr][nc] != -1) {
            parent[nr][nc] = {r, c};
            if (dfsHelper(nr, nc, visited, parent, nodesExplored, showSteps))
                return true; // Stop immediately once target is found
        }
    }
    return false;
}

void runDFS(bool showSteps = false) {
    vector<vector<bool>> visited(ROWS, vector<bool>(COLS, false));
    vector<vector<pair<int, int>>> parent(ROWS, vector<pair<int, int>>(COLS, {-1, -1}));
    int nodesExplored = 0;

    if (showSteps) cout << "\n[DFS Traversal Order]: ";
    dfsHelper(startNode.first, startNode.second, visited, parent, nodesExplored, showSteps);
    if (showSteps) cout << "DONE\n";

    traceAndPrintPath(parent, "DFS (Deep Exploration)", nodesExplored);
}


// 7. DIJKSTRA (Uses Min-Heap Priority Queue)

void runDijkstra(bool showSteps = false) {
    vector<vector<int>> dist(ROWS, vector<int>(COLS, INT_MAX));
    vector<vector<pair<int, int>>> parent(ROWS, vector<pair<int, int>>(COLS, {-1, -1}));

    // Min-Heap storing {accumulated_cost, {row, col}}
    typedef pair<int, pair<int, int>> PQNode;
    priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pq;

    dist[startNode.first][startNode.second] = 0;
    pq.push({0, startNode});
    int nodesExplored = 0;

    if (showSteps) cout << "\n[Dijkstra Pop Order (Cost, Cell)]: ";

    while (!pq.empty()) {
        PQNode top = pq.top();
        pq.pop();

        int currCost = top.first;
        int r = top.second.first;
        int c = top.second.second;

        // Skip outdated entries in the priority queue
        if (currCost > dist[r][c]) continue;
        nodesExplored++;

        if (showSteps) cout << "[Cost " << currCost << " @ (" << r << "," << c << ")] -> ";

        if (make_pair(r, c) == endNode) break; // Optimal cheapest path found!

        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && grid[nr][nc] != -1) {
                int newCost = currCost + grid[nr][nc]; // Add neighbor's terrain weight!
                if (newCost < dist[nr][nc]) {
                    dist[nr][nc] = newCost;
                    parent[nr][nc] = {r, c};
                    pq.push({newCost, {nr, nc}});
                }
            }
        }
    }
    if (showSteps) cout << "DONE\n";
    traceAndPrintPath(parent, "Dijkstra (Weighted Lowest Cost)", nodesExplored);
}


// 8. OPERATION HISTORY & RESET

vector<string> operationHistory;

void logOp(string action) {
    operationHistory.push_back(action);
}

void showHistory() {
    cout << "\n=== OPERATION HISTORY ===\n";
    if (operationHistory.empty()) {
        cout << "No operations performed yet.\n";
        return;
    }
    for (int i = 0; i < (int)operationHistory.size(); i++) {
        cout << i + 1 << ". " << operationHistory[i] << "\n";
    }
}

void resetAll() {
    int defaultGrid[ROWS][COLS] = {
        {1,  1,  5,  1,  1,  1},
        {1, -1,  5, -1,  2,  1},
        {1, -1,  1,  1,  9,  1},
        {1,  1,  1, -1,  1,  1},
        {5,  5, -1,  1,  1,  1},
        {1,  1,  1,  1,  5,  1}
    };
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            grid[r][c] = defaultGrid[r][c];

    startNode = {0, 0};
    endNode   = {5, 5};
    loadSampleRecords();
    logOp("Reset grid and reloaded 18 sample records.");
    cout << "[OK] System reset to default state.\n";
}


// 9. CRUD OPERATIONS + INPUT VALIDATION

void viewRecords() {
    cout << "\n=== TERRAIN CELL RECORDS (" << records.size() << " total) ===\n";
    cout << left << setw(8) << "ID" << setw(10) << "(Row,Col)"
         << setw(14) << "Terrain" << "Weight\n";
    cout << "------------------------------------------\n";
    for (const auto& rec : records) {
        cout << left << setw(8) << rec.cellID
             << "(" << rec.row << "," << rec.col << ")     "
             << setw(14) << rec.terrainType
             << (rec.weight == -1 ? "WALL (-1)" : to_string(rec.weight)) << "\n";
    }
}

void createOrUpdateRecord() {
    int id, r, c, w;
    string type;
    cout << "Enter Cell ID (e.g., 119): "; cin >> id;
    cout << "Enter Row (0-" << ROWS - 1 << ") and Col (0-" << COLS - 1 << "): ";
    cin >> r >> c;

    // Input Validation
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) {
        cout << "[ERROR] Invalid grid coordinates!\n";
        return;
    }
    if (make_pair(r, c) == startNode || make_pair(r, c) == endNode) {
        cout << "[ERROR] Cannot turn Start or End node into a wall/custom obstacle!\n";
        return;
    }
    cout << "Enter Weight (-1 for Wall, 1 to 9 for walkable): "; cin >> w;
    if (w < -1 || w == 0 || w > 9) {
        cout << "[ERROR] Weight must be -1 or between 1 and 9!\n";
        return;
    }
    cout << "Enter Terrain Name (Road/Grass/Mud/Wall): "; cin >> type;

    // Sync directly with the 2D Grid!
    grid[r][c] = w;

    // Update if ID exists, otherwise Create new record
    for (auto& rec : records) {
        if (rec.cellID == id) {
            rec = {id, r, c, type, w};
            logOp("Updated Record ID " + to_string(id) + " at (" + to_string(r) + "," + to_string(c) + ")");
            cout << "[OK] Record updated & synced with grid!\n";
            return;
        }
    }
    records.push_back({id, r, c, type, w});
    logOp("Created Record ID " + to_string(id) + " at (" + to_string(r) + "," + to_string(c) + ")");
    cout << "[OK] New record created & synced with grid!\n";
}

void deleteRecord() {
    int id;
    cout << "Enter Cell ID to delete: "; cin >> id;
    for (int i = 0; i < (int)records.size(); i++) {
        if (records[i].cellID == id) {
            int r = records[i].row, c = records[i].col;
            grid[r][c] = 1; // Reset cell on grid back to default Road (weight 1)
            records.erase(records.begin() + i);
            logOp("Deleted Record ID " + to_string(id) + " (grid cell reset to 1)");
            cout << "[OK] Record deleted.\n";
            return;
        }
    }
    cout << "[ERROR] Record ID not found.\n";
}


// 10. SEARCH & SORT RECORDS

void searchRecords() {
    int choice;
    cout << "Search by: 1. Cell ID  2. Terrain Type: ";
    cin >> choice;
    bool found = false;

    if (choice == 1) {
        int targetID;
        cout << "Enter Cell ID: "; cin >> targetID;
        for (const auto& rec : records) {
            if (rec.cellID == targetID) {
                cout << "[FOUND] ID: " << rec.cellID << " | (" << rec.row << "," << rec.col
                     << ") | " << rec.terrainType << " | Weight: " << rec.weight << "\n";
                found = true;
            }
        }
    } else {
        string targetType;
        cout << "Enter Terrain Type (Road/Grass/Mud/Wall): "; cin >> targetType;
        for (const auto& rec : records) {
            if (rec.terrainType.find(targetType) != string::npos) {
                cout << "[MATCH] ID: " << rec.cellID << " | (" << rec.row << "," << rec.col
                     << ") | " << rec.terrainType << " | Weight: " << rec.weight << "\n";
                found = true;
            }
        }
    }
    if (!found) cout << "[INFO] No matching records found.\n";
    logOp("Executed record search.");
}

void sortRecordsByWeight() {
    // Selection Sort by weight (ascending)
    int n = records.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (records[j].weight < records[minIdx].weight)
                minIdx = j;
        }
        swap(records[i], records[minIdx]);
    }
    cout << "[OK] Records sorted by Weight (Ascending).\n";
    logOp("Sorted records by weight.");
    viewRecords();
}


// 11. COMPLEXITY SUMMARY & ALGORITHM COMPARE

void showComplexitySummary() {
    cout << "\n================ DSA COMPLEXITY SUMMARY (V = R*C) ================\n";
    cout << left << setw(15) << "Algorithm" << setw(18) << "Data Structure"
         << setw(16) << "Time" << setw(12) << "Space" << "Weighted Optimal?\n";
    cout << "------------------------------------------------------------------\n";
    cout << left << setw(15) << "BFS" << setw(18) << "Queue (FIFO)"
         << setw(16) << "O(R * C)" << setw(12) << "O(R * C)" << "No (Steps only)\n";
    cout << left << setw(15) << "DFS" << setw(18) << "Recursion Stack"
         << setw(16) << "O(R * C)" << setw(12) << "O(R * C)" << "No\n";
    cout << left << setw(15) << "Dijkstra" << setw(18) << "Min-Heap (PQ)"
         << setw(16) << "O(V log V)" << setw(12) << "O(R * C)" << "Yes (Lowest Cost)\n";
    cout << "==================================================================\n";
}


// 12. MAIN MENU CONTROLLER

int main() {
    loadSampleRecords();
    logOp("Initialized system with 18 sample grid records.");

    int choice;
    vector<vector<bool>> emptyPath(ROWS, vector<bool>(COLS, false));

    while (true) {
        cout << "\n========= GRID PATHFINDING VISUALIZER (DSA2-P030) =========\n";
        cout << "1. View Current Weighted Grid\n";
        cout << "2. Run BFS (Shortest Steps)\n";
        cout << "3. Run DFS (Deep Exploration)\n";
        cout << "4. Run Dijkstra (Lowest Weighted Cost - Priority Queue)\n";
        cout << "5. Compare ALL 3 Algorithms Side-by-Side\n";
        cout << "6. View All Terrain Records (15+ Records)\n";
        cout << "7. Create / Update Cell Record (Modifies Grid)\n";
        cout << "8. Delete Cell Record\n";
        cout << "9. Search Records (Filter by ID / Terrain)\n";
        cout << "10. Sort Records by Weight\n";
        cout << "11. View Operation History & Complexity Summary\n";
        cout << "12. Reset Grid & Reload Sample Data\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[ERROR] Please enter a valid number.\n";
            continue;
        }

        switch (choice) {
            case 1: printGrid(emptyPath); break;
            case 2: runBFS(true); logOp("Executed BFS with step trace."); break;
            case 3: runDFS(true); logOp("Executed DFS with step trace."); break;
            case 4: runDijkstra(true); logOp("Executed Dijkstra with PQ trace."); break;
            case 5:
                runBFS(false);
                runDFS(false);
                runDijkstra(false);
                logOp("Compared BFS, DFS, and Dijkstra.");
                break;
            case 6: viewRecords(); break;
            case 7: createOrUpdateRecord(); break;
            case 8: deleteRecord(); break;
            case 9: searchRecords(); break;
            case 10: sortRecordsByWeight(); break;
            case 11: showHistory(); showComplexitySummary(); break;
            case 12: resetAll(); break;
            case 0: cout << "Exiting...\n"; return 0;
            default: cout << "[ERROR] Invalid choice.\n";
        }
    }
}