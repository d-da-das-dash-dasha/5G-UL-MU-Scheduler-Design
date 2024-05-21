//
// Created by Daria on 5/7/2024.
//

#include "solver.h"
#include <string>
#include <cassert>
#include <iostream>

using namespace std;

int rbsCounting(const vector<Interval>& answer, vector<UserInfo> userInfos) {
    int rbs = 0;
    for (const auto& interval : answer) {
        for (const auto id : interval.users) {
            int current = min(interval.end - interval.start, userInfos[id].rbNeed);
            userInfos[id].rbNeed -= current;
            rbs += current;
        }
    }
    return rbs;
}

int aCounting(const vector<UserInfo>& userInfos) {
    int sumA = 0;
    for (const auto& user : userInfos) {
        sumA += user.rbNeed;
    }
    return sumA;
}

vector<vector<string>> imageOfAnswer(vector<Interval> answer, vector<UserInfo> userInfos, vector<Interval>& reservedRBs, int m, int l) {
    sort(answer.begin(), answer.end(), [](const Interval &lhs, const Interval &rhs) {
        return lhs.start < rhs.start;
    });
    vector<vector<string>> image(l, vector<string>(m, "."));
    for (auto& interval : reservedRBs) {
        for (auto& row : image) {
            for (int j = interval.start; j < interval.end; ++j) {
                row[j] = "*";
            }
        }
    }
    for (auto& interval : answer) {
        int i = l - 1;
        for (int id : interval.users) {
            if (userInfos[id].rbNeed == 0) {
                cerr << id << ' ' << interval.start << ' ' << interval.end;
            }
            int cur = min(interval.end, interval.start + userInfos[id].rbNeed);
            userInfos[id].rbNeed -= (cur - interval.start);
            for (int j = interval.start; j < cur; ++j) {
                image[i][j] = to_string(userInfos[id].beam);
            }
            --i;
        }
    }
    return image;
}

struct UserStat {
    UserInfo info;
    vector<Interval> intervals;

    void print() {
        cout << info.rbNeed << ' ' << info.beam << ' ' << info.id << '\n';

        sort(intervals.begin(), intervals.end(), [](const Interval &lhs, const Interval &rhs) {
            return lhs.start < rhs.start;
        });
        for (auto& interval : intervals) {
            cout << interval.start << ' ' << interval.end << '\n';
        }
        cout << '\n';
    }

};

void userStatistics(vector<Interval>& answer, vector<UserInfo>& userInfos, int n) {
    vector<UserStat> stat(n);
    for (int i = 0; i < n; ++i) {
        stat[i].info = userInfos[i];
    }
    for (auto& interval : answer) {
        for (int id : interval.users) {
            stat[id].intervals.emplace_back(interval);
        }
    }
    for (auto& user : stat) {
        user.print();
    }
}
