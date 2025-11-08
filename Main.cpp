#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

vector<string> loadRecords(const string& filename) {
    vector<string> records;
    ifstream file(filename);

    if (!file) {
        cout << "Error: file not found!\n";
        return records;
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty())
            records.push_back(line);
    }

    return records;
}

void saveRecordsToFile(const string& filename, const vector<string>& records) {
    ofstream file(filename, ios::trunc);

    if (!file) {
        cout << "Error writing to " << filename << "!\n";
        return;
    }

    for (const string& rec : records) {
        file << rec << "\n";   // write each record as a full line
    }

    file.close();
}

struct IndexEntry {
    string id;
    int vecIndex;
};

class PrimaryIndex {
private:
    string indexfile;
    vector<string>* records;     // pointer to the vector of records

    int binarySearch(const string& key) {
        int low = 0, high = indexList.size() - 1;

        while (low <= high) {
            int mid = (low + high) / 2;

            if (indexList[mid].id == key)
                return mid;

            if (indexList[mid].id < key)
                low = mid + 1;
            else
                high = mid - 1;
        }
        return -1;
    }

public:
    vector<IndexEntry> indexList;

    PrimaryIndex(string idxFile, vector<string>& recs)
        : indexfile(idxFile), records(&recs) {}

    void buildIndex() {
        indexList.clear();

        for (int i = 0; i < records->size(); i++) {
            string line = (*records)[i];
            stringstream ss(line);
            string code, id, name, addr;

            getline(ss, code, '|');
            getline(ss, id, '|');

            indexList.push_back({ id, i });  // store vector index
        }

        // Sort by ID
        sort(indexList.begin(), indexList.end(),
            [](const IndexEntry& a, const IndexEntry& b) {
                return a.id < b.id;
            });

        saveIndex();
    }

    void loadIndex() {
        ifstream idx(indexfile);
        if (!idx) {
            cout << "Index file missing!\n";
            return;
        }

        indexList.clear();
        string line, id, idxStr;

        while (getline(idx, line)) {
            stringstream ss(line);
            getline(ss, id, '|');
            getline(ss, idxStr);
            indexList.push_back({ id, stoi(idxStr) });
        }
        idx.close();
    }

    void saveIndex() {
        ofstream idx(indexfile, ios::trunc);
        if (!idx) {
            cout << "Error writing " << indexfile << "\n";
            return;
        }

        for (const auto& e : indexList) {
            idx << e.id << "|" << e.vecIndex << "\n";
        }
        idx.close();
    }

    // Returns vector index, NOT file offset
    int positionInVec(const string& keyID) {
        int pos = binarySearch(keyID);
        if (pos == -1) return -1;
        return indexList[pos].vecIndex;
    }

    // Return the actual record string
    string extractRecord(const string& keyID) {
        int vecIndex = positionInVec(keyID);
        if (vecIndex < 0 || vecIndex >= records->size())
            return "Record not found";

        return (*records)[vecIndex];
    }
};

int main() {
    vector<string> recs = loadRecords("appointments.txt");

    // modify something (example)
    recs.push_back("014 |09|13 sep|09");

    // save back
    saveRecordsToFile("appointments.txt", recs);

    cout << "File updated successfully!\n";
}
