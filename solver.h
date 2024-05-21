//
// Created by Daria on 5/7/2024.
//

#ifndef UNTITLED2_EMPTY_H
#define UNTITLED2_EMPTY_H

#include <algorithm>
#include <deque>
#include <vector>

using namespace std;

struct Interval {
    int start, end;
    std::vector<int> users;

    Interval();
    Interval(int start_, int end_);
    Interval(int start_, int end_, vector<int>& users_);
};
struct UserInfo {
    int rbNeed;
    int beam;
    int id;
};

vector<Interval> getCurrent(const vector<Interval> &reservedRBs, int m, vector<UserInfo> &userInfos);
vector<Interval> Solver(int n, int m, int k, int j, int l,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos);


#endif //UNTITLED2_EMPTY_H
