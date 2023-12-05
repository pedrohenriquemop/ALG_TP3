#include <math.h>

#include <algorithm>
#include <iostream>
#include <vector>

#define DEBUG false
#define MINUS_INFINITE -2147483647
#define INFINITE 2147483647

using namespace std;

class Section {
   public:
    int multiplier, length;

    Section(int m, int l) : multiplier(m), length(l){};
};

class Trick {
   public:
    int points, exec_time;

    Trick(int p, int e) : points(p), exec_time(e){};
};

class TrickCombination {
   public:
    vector<int> tricks;
    int totalTime;

    TrickCombination(vector<int> _tricks, int _totalTime) : tricks(_tricks), totalTime(_totalTime){};
};

void trickCombination(int aux[], int start, int end, int index, int size, vector<TrickCombination> &result, vector<Trick> &tricksReference);

vector<TrickCombination> generateAllTrickCombinations(vector<Trick> &tricks, int size) {
    vector<TrickCombination> result;
    for (int i = 1; i <= size; i++) {
        int *aux = new int[i];
        trickCombination(aux, 0, size - 1, 0, i, result, tricks);
    }
    return result;
}

void trickCombination(int aux[], int start, int end, int index, int size, vector<TrickCombination> &result, vector<Trick> &tricksReference) {
    if (index == size) {
        vector<int> tricks;
        int totalTime = 0;
        for (int i = 0; i < size; i++) {
            tricks.push_back(aux[i]);
            totalTime += tricksReference[aux[i]].exec_time;
        }
        result.push_back(TrickCombination(tricks, totalTime));
        return;
    }

    for (int i = start; i <= end && end - i + 1 >= size - index; i++) {
        aux[index] = i;
        trickCombination(aux, i + 1, end, index + 1, size, result, tricksReference);
    }
}

int calcPoints(vector<Trick> &tricksReference, vector<TrickCombination> &trickCombinations, int currentIndex, int previousIndex, vector<vector<int>> &pointsCache) {
    if (previousIndex == -1) {
        vector<int> currentTricks = trickCombinations[currentIndex].tricks;
        int sum = 0;

        for (int currentTrick : currentTricks) {
            sum += tricksReference[currentTrick].points;
        }

        sum *= currentTricks.size();
        return sum;
    }

    if (pointsCache[currentIndex][previousIndex] == MINUS_INFINITE) {
        vector<int> currentTricks = trickCombinations[currentIndex].tricks;
        vector<int> previousTricks = trickCombinations[previousIndex].tricks;
        int sum = 0;

        for (int currentTrick : currentTricks) {
            if (find(previousTricks.begin(), previousTricks.end(), currentTrick) != previousTricks.end()) {
                sum += tricksReference[currentTrick].points / 2;
                continue;
            }
            sum += tricksReference[currentTrick].points;
        }

        sum *= currentTricks.size();

        pointsCache[currentIndex][previousIndex] = sum;
    }

    return pointsCache[currentIndex][previousIndex];
}

vector<int> getPossibleTrickCombinations(vector<Section> &sections, vector<TrickCombination> &trickCombinations, int sectionIndex, vector<vector<int>> &sectionPossibleTricksCache) {
    if (sectionPossibleTricksCache[sectionIndex][0] == -1) {
        vector<int> result;

        for (int i = 0; i < trickCombinations.size(); i++) {
            if (trickCombinations[i].totalTime <= sections[sectionIndex].length) result.push_back(i);
        }

        sectionPossibleTricksCache[sectionIndex] = result;
    }

    return sectionPossibleTricksCache[sectionIndex];
}

int f(int sectionIndex, int trickCombinationIndex, vector<Section> &sections, vector<Trick> &tricksReference, vector<TrickCombination> &trickCombinations, vector<vector<int>> &pointsCache, vector<vector<int>> &sectionPossibleTricksCache, vector<int> &resultArray, vector<vector<int>> &fCache) {
    if (sectionIndex >= sections.size()) return 0;

    if (fCache[sectionIndex][trickCombinationIndex + 1] != MINUS_INFINITE) return fCache[sectionIndex][trickCombinationIndex + 1];

    int max = f(sectionIndex + 1, -1, sections, tricksReference, trickCombinations, pointsCache, sectionPossibleTricksCache, resultArray, fCache);
    int maxTrickCombinationIndex = -1;
    vector<int> possibleTrickCombinations = getPossibleTrickCombinations(sections, trickCombinations, sectionIndex, sectionPossibleTricksCache);

    for (int currTrickCombinationIndex : possibleTrickCombinations) {
        int result = f(sectionIndex + 1, currTrickCombinationIndex, sections, tricksReference, trickCombinations, pointsCache, sectionPossibleTricksCache, resultArray, fCache) + calcPoints(tricksReference, trickCombinations, currTrickCombinationIndex, trickCombinationIndex, pointsCache) * sections[sectionIndex].multiplier;

        if (result > max) {
            max = result;
            maxTrickCombinationIndex = currTrickCombinationIndex;
        }
    }

    resultArray[sectionIndex] = maxTrickCombinationIndex;
    fCache[sectionIndex][trickCombinationIndex + 1] = max;
    return max;
}

int callF(vector<Section> &sections, vector<Trick> &tricksReference, vector<TrickCombination> &trickCombinations, vector<vector<int>> &pointsCache, vector<vector<int>> &sectionPossibleTricksCache, vector<int> &resultArray, vector<vector<int>> &fCache) {
    return f(0, -1, sections, tricksReference, trickCombinations, pointsCache, sectionPossibleTricksCache, resultArray, fCache);
}

int main() {
    int n, k;

    cin >> n >> k;

    vector<Section> sections;
    vector<Trick> tricks;

    for (int i = 0; i < n; i++) {
        int multiplier, length;
        cin >> multiplier >> length;
        sections.push_back(Section(multiplier, length));
    }

    for (int i = 0; i < k; i++) {
        int points, exec_time;
        cin >> points >> exec_time;
        tricks.push_back(Trick(points, exec_time));
    }

    if (DEBUG) {
        cout << "Sections:" << endl;
        for (int i = 0; i < n; i++) {
            cout << i << " - mult: " << sections[i].multiplier << ", length: " << sections[i].length << endl;
        }

        cout << "Tricks:" << endl;
        for (int i = 0; i < k; i++) {
            cout << i << " - points: " << tricks[i].points << ", time: " << tricks[i].exec_time << endl;
        }
    }

    vector<vector<int>> pointsCache((int)pow(2, k));

    for (int i = 0; i < (int)pow(2, k); i++) {
        for (int j = 0; j < (int)pow(2, k); j++) {
            pointsCache[i].push_back(MINUS_INFINITE);
        }
    }

    vector<vector<int>> sectionPossibleTricksCache(n);
    for (int i = 0; i < n; i++) {
        sectionPossibleTricksCache[i] = {-1};
    }

    vector<TrickCombination> combinations = generateAllTrickCombinations(tricks, k);

    vector<vector<int>> fCache(n);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= combinations.size(); j++) {
            fCache[i].push_back(MINUS_INFINITE);
        }
    }

    if (DEBUG) {
        cout << "Combinations:" << endl;
        for (int i = 0; i < combinations.size(); i++) {
            cout << i << ": ";
            for (int j = 0; j < combinations[i].tricks.size(); j++) {
                cout << combinations[i].tricks[j] << " ";
            }
            cout << "(" << combinations[i].totalTime << ")" << endl;
        }

        cout << "test points:" << calcPoints(tricks, combinations, 2, 1, pointsCache) << endl;
    }

    vector<int> resultArray(n);

    int finalResult = callF(sections, tricks, combinations, pointsCache, sectionPossibleTricksCache, resultArray, fCache);

    if (DEBUG) {
        cout << "fCache FINAL:" << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= combinations.size(); j++) {
                cout << "fCache[" << i << "][" << j << "] = " << fCache[i][j] << endl;
            }
        }
    }

    cout << finalResult << endl;
    for (int trickCombinationIndex : resultArray) {
        if (trickCombinationIndex == -1) {
            cout << 0 << endl;
            continue;
        }
        cout << combinations[trickCombinationIndex].tricks.size();
        for (int trickIndex : combinations[trickCombinationIndex].tricks) {
            cout << " " << trickIndex + 1;
        }
        cout << endl;
    }

    if (DEBUG) {
        cout << "pointsCache: " << endl;
        for (int i = 0; i < (int)pow(2, k); i++) {
            for (int j = 0; j < (int)pow(2, k); j++) {
                if (pointsCache[i][j] != MINUS_INFINITE) cout << "(" << i << ", " << j << ") = " << pointsCache[i][j] << endl;
            }
        }
    }

    return 0;
}

/*
IN:
3 2
10 20
1 60
100 60
50 10
1000 50

OUT:
210050
1 1
0
2 1 2

IN:
3 2
4 1
3 3
1 1
1000 1
50 2

OUT:
7800
1 1
2 1 2
1 1

IN:
10 4
3 84
3 100
5 16
5 82
7 4
2 48
9 11
10 37
9 73
4 14
4 18
20 19
13 12
6 13

OUT:
3486
4 4 3 2 1
4 4 3 2 1
0
4 4 3 2 1
0
3 4 3 2
0
2 4 2
4 4 3 2 1
1 3

IN:
10 4
10 67
8 50
10 87
10 74
10 59
9 38
4 4
8 39
5 20
8 70
1 5
8 7
12 11
17 19

OUT:
7150
4 4 3 2 1
0
4 4 3 2 1
0
4 4 3 2 1
3 4 3 2
0
3 4 3 2
0
4 4 3 2 1

*/