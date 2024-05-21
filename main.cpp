//
// Created by Antos on 5/3/2024.
//

#include <iostream>
#include "solver.h"

using namespace std;

int rbsCounting(const vector<Interval>& answer, vector<UserInfo> userInfos);

int aCounting(const vector<UserInfo>& userInfos);

vector<vector<string>> imageOfAnswer(vector<Interval> answer, vector<UserInfo> userInfos, vector<Interval>& reservedRBs, int m, int l);

void userStatistics(vector<Interval>& answer, vector<UserInfo>& userInfos, int n);

int main() {
    freopen("./../open.txt", "r", stdin);
    freopen("./../output.txt", "w", stdout);
    int t;
    cin >> t;
    int totalRbs = 0;
    int totalA = 0;

    while (t--) {
        int n, m, k, j, l;
        cin >> n >> m >> k >> j >> l;
        vector<Interval> reservedRBs(k);
        int s = m * l;
        for (int i = 0; i < k; ++i) {
            cin >> reservedRBs[i].start >> reservedRBs[i].end;
            s -= (reservedRBs[i].end - reservedRBs[i].start) * l;
        }
        vector<UserInfo> userInfos(n);
        for (int i = 0; i < n; ++i) {
            cin >> userInfos[i].rbNeed >> userInfos[i].beam;
            userInfos[i].id = i;
        }
        vector<Interval> answer = Solver(n, m, k, j, l, reservedRBs, userInfos);

        int rbs = rbsCounting(answer, userInfos);
        // cerr << "RBs: " << rbs << '\n';
        totalRbs += rbs;
        int sumA = aCounting(userInfos);
        // cerr << "Sum of RB need: " << sumA << '\n';
        totalA += sumA;
        // cerr << t << endl;
        if (min(sumA, s) - rbs >= 300 && l <= 11 && answer.size() < j) {
            cout << "n m k j l\n";
            cout << n << ' ' << m << ' ' << k << ' ' << j << ' ' << l << endl << "Reserved\n";
            for (int i = 0; i < k; ++i) {
                cout << reservedRBs[i].start << ' ' << reservedRBs[i].end << endl;
            }
            cout << "Users\n";
            for (int i = 0; i < n; ++i) {
                cout << userInfos[i].rbNeed << ' ' << userInfos[i].beam << ' ' << i << endl;
            }
            cout << "sumA S RBs\n";
            cout << sumA << ' ' << s << ' ' << rbs << endl;
            cout << "Answer\n";
            for (const auto &interval: answer) {
                cout << interval.start << ' ' << interval.end << '\n';
                for (auto id: interval.users) {
                    cout << id << ' ';
                }
                cout << '\n';
            }
            cout << "\n\n";

            vector<vector<string>> image = imageOfAnswer(answer, userInfos, reservedRBs, m, l);
            for (auto &row: image) {
                for (auto &col: row) {
                    cout << col << '\t';
                }
                cout << '\n';
            }
            cout << "\n\n";

            userStatistics(answer, userInfos, n);
        }

    }
    cout << "Total RBs: " << totalRbs << '\n';
    cout << "Total RB need: " << totalA << '\n';
}