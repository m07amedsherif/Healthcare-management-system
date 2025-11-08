#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

// ================== File Helpers ==================
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
    for (const string& rec : records)
        file << rec << "\n";
    file.close();
}

// ================== Index Structures ==================
struct IndexEntry {
    string id;
    int vecIndex;
};

class PrimaryIndex {
private:
    string indexfile;
    vector<string>* records;

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
            string id;
            getline(ss, id, '|');
            id.erase(remove_if(id.begin(), id.end(), ::isspace), id.end());
            indexList.push_back({ id, i });
        }
        sort(indexList.begin(), indexList.end(),
             [](const IndexEntry& a, const IndexEntry& b) {
                 return a.id < b.id;
             });
        saveIndex();
    }

    void saveIndex() {
        ofstream idx(indexfile, ios::trunc);
        if (!idx) {
            cout << "Error writing " << indexfile << "\n";
            return;
        }
        for (const auto& e : indexList)
            idx << e.id << "|" << e.vecIndex << "\n";
        idx.close();
    }

    int positionInVec(const string& keyID) {
        int pos = binarySearch(keyID);
        if (pos == -1) return -1;
        return indexList[pos].vecIndex;
    }

    void updateIndex() {
        buildIndex();
    }
};

// ================== Doctor Class ==================
class Doctor {
private:
    string filename;
    string indexfile;
    vector<string> records;
    PrimaryIndex* index;

public:
    Doctor(string file, string idx) : filename(file), indexfile(idx) {
        records = loadRecords(filename);
        index = new PrimaryIndex(indexfile, records);
        index->buildIndex();
    }

    void insertDoctor(const string& id, const string& name, const string& specialty) {}

    void updateDoctor(const string& id, const string& newName, const string& newSpec) {}

    void deleteDoctor(const string& id) {}

    void viewDoctor(const string& id) {}
};

// ================== Appointment Class ==================
class Appointment {
private:
    string filename;
    string indexfile;
    vector<string> records;
    PrimaryIndex* index;

public:
    Appointment(string file, string idx) : filename(file), indexfile(idx) {
        records = loadRecords(filename);
        index = new PrimaryIndex(indexfile, records);
        index->buildIndex();
    }

    void insertAppointment(const string& id, const string& docID,
                           const string& date, const string& patientID) {}

    void updateAppointment(const string& id, const string& newDoc,
                           const string& newDate, const string& newPatient) {}

    void deleteAppointment(const string& id) {}

    void viewAppointment(const string& id) {}
};

// ================== Main ==================
int main() {
    Doctor d("doctors.txt", "DocIndexfile.txt");
    Appointment a("appointments.txt", "AppointmentsIndexfile.txt");

    // Examples
    d.insertDoctor("Dr. Ali", "Cardiology");
    d.updateDoctor("05", "Dr. Ali Hassan", "Cardiology");
    d.deleteDoctor("01");
    d.viewDoctor("01");

    a.insertAppointment("10 Nov", "P007");
    a.updateAppointment("07" ,"12 Nov", "P010");
    a.deleteAppointment("01");
    a.viewAppointment("01");

    cout << "Done.\n";
    return 0;
}
