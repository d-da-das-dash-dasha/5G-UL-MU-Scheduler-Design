#include <algorithm>
#include <array>
#include <vector>
#include <iostream>
#include <bitset>
#include <map>
#include <unordered_map>

using namespace std;

#ifdef __LOCAL

#include "solver.h"

int rbsCounting(const vector<Interval> &answer, vector<UserInfo> userInfos);

#else
struct Interval {
    int start{}, end{};
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
#endif

Interval::Interval() = default;

Interval::Interval(int start_, int end_) : start(start_), end(end_) {}

Interval::Interval(int start_, int end_, vector<int> &users_) : start(start_), end(end_), users(users_) {}

const int BEAM_COUNT = 32;
const int USERS_COUNT = 129;
int L;

array<vector<pair<Interval *, int>>, USERS_COUNT> user_intervals;
array<vector<UserInfo>, BEAM_COUNT> users_data;
vector<Interval> current;
vector<UserInfo> best;

struct IntervalInfo {
    Interval* cur;
    int bound;
    int res;
    bitset<USERS_COUNT> mask;
};

unordered_map<Interval*, IntervalInfo> intervals;
int operations = 0;

vector<UserInfo> getBest() {
    best.clear();
    for (int beam = 0; beam < BEAM_COUNT; ++beam) {
        if (!users_data[beam].empty()) {
            best.emplace_back(users_data[beam].back());
        }
    }
    sort(best.begin(), best.end(), [](const UserInfo &lhs, const UserInfo &rhs) {
        return lhs.rbNeed > rhs.rbNeed;
    });
    return best;
}

void getAnswer(Interval* interval, vector<UserInfo> &userInfos) {
    if (interval->start == interval->end) {
        intervals[interval] = {interval, 0, -1, 0};
        return;
    }

    auto &cur = *interval;
    if (cur.users.empty()) {
        int res = 0;
        bitset<USERS_COUNT> best_mask;
        for (int i = 0; i < min(L, (int) best.size()); ++i) {
            auto &user = userInfos[best[i].id];
            int rb_score = min(cur.end - cur.start, user.rbNeed);
            res += rb_score;
            best_mask.set(best[i].id);
        }
        intervals[&cur] = {&cur, 0, res, best_mask};
        return;
    }

    int best_res = 0;
    int best_bound = -1;
    bitset<USERS_COUNT> best_mask;
    vector<int> wide_users;
    for (int i = 0; i < cur.users.size(); ++i) {
        const UserInfo &user = userInfos[cur.users[i]];
        if (&cur != user_intervals[user.id].back().first) {
            wide_users.emplace_back(user.id);
            best_mask.set(user.id);
        }
    }

    bitset<USERS_COUNT> temp_mask;
    vector<int> temp_users;

    vector<int> bounds;
    for (int cut = 0; cut < cur.users.size(); ++cut) {
        int bound = cur.start + user_intervals[cur.users[cut]].back().second;
        for (int i = bound; i < min(bound + 20, cur.end); ++i) {
            bounds.emplace_back(bound);
        }
    }
    sort(bounds.begin(), bounds.end());
    bounds.resize(unique(bounds.begin(), bounds.end()) - bounds.begin());

    for (auto bound : bounds) {
        vector<int> ids;
        for (int i = 0; i < cur.users.size(); ++i) {
            const UserInfo &user = userInfos[cur.users[i]];
            if (&cur == user_intervals[user.id].back().first) {
                if (cur.start + user_intervals[user.id].back().second - 1 >= bound) {
                    ids.emplace_back(user.id);
                }
            }
        }
        int choosen_mask = -1;
        for (int mask = 0; mask < (1 << ids.size()); (ids.size() <= 6 ? ++mask : mask += (1 << (ids.size() - 6)))) {
            ++operations;
            vector<bool> badBeams(BEAM_COUNT);
            int res = 0;
            int cnt = 0;
            for (int i = 0; i < ids.size(); ++i) {
                const UserInfo &user = userInfos[ids[i]];
                if ((1 << i) & mask) {
                    int diff = user_intervals[user.id].back().second - (bound - cur.start);
                    res -= diff;
                } else {
                    badBeams[user.beam] = true;
                    ++cnt;
                }
            }
            for (auto id: wide_users) {
                badBeams[userInfos[id].beam] = true;
                ++cnt;
            }
            vector<int> users;
            for (int i = 0; cnt < L && i < best.size(); ++i) {
                if (badBeams[best[i].beam]) {
                    continue;
                }
                res += min(cur.end - bound, best[i].rbNeed);
                users.emplace_back(best[i].id);
                ++cnt;
            }
            if (res > best_res) {
                best_res = res;
                best_bound = bound;
                choosen_mask = mask;
                temp_users = users;
            }
        }

        if (choosen_mask != -1) {
            temp_mask.reset();
            for (int i = 0; i < ids.size(); ++i) {
                const UserInfo &user = userInfos[ids[i]];
                if (!((1 << i) & choosen_mask)) {
                    temp_mask.set(user.id);
                }
            }
        }
    }
    best_mask |= temp_mask;
    for (auto id : temp_users) {
        best_mask.set(id);
    }

    intervals[&cur] = {&cur, best_bound, best_res, best_mask};
}

int total = 0;

void modify_interval(IntervalInfo& interval_info, vector<UserInfo> &userInfos) {
    auto &cur = *interval_info.cur;
    Interval* choosen_interval = &cur;
    if (interval_info.bound != 0) {
        Interval newInterval = {interval_info.bound, cur.end};
        cur.end = interval_info.bound;
        current.emplace_back(newInterval);
        choosen_interval = &current.back();
    }
    auto &new_cur = *choosen_interval;
    auto mask = interval_info.mask;
    vector<int> check_users;
    for (int i = 0; i < cur.users.size(); ++i) {
        UserInfo &user = userInfos[cur.users[i]];
        if (interval_info.mask[user.id]) {
            interval_info.mask.flip(user.id);
            new_cur.users.emplace_back(user.id);

            if (&cur == user_intervals[user.id].back().first) {
                int rb_score = user_intervals[user.id].back().second - (cur.end - cur.start);
                user_intervals[user.id].back().second -= rb_score;
                user_intervals[user.id].emplace_back(&new_cur, rb_score);
            } else {
                user_intervals[user.id].emplace_back(&new_cur, new_cur.end - new_cur.start);
                sort(user_intervals[user.id].begin(), user_intervals[user.id].end(),
                     [](const pair<Interval *, int> &lhs, const pair<Interval *, int> &rhs) {
                         return lhs.first->start < rhs.first->start;
                     });
                for (auto &interval: user_intervals[user.id]) {
                    if (interval.first == &cur) {
                        interval.second = cur.end - cur.start;
                    }
                }
            }
        } else {
            if (cur.start + user_intervals[user.id].back().second - 1 >= interval_info.bound) {
                int diff = user_intervals[user.id].back().second - (cur.end - cur.start);
                user.rbNeed += diff;
                total += diff;
                user_intervals[user.id].back().second = cur.end - cur.start;
                check_users.emplace_back(user.id);
            }
        }
    }
    for (int i = 0; i < USERS_COUNT; ++i) {
        if (interval_info.mask[i]) {
            auto &user = userInfos[i];
            new_cur.users.emplace_back(user.id);
            int rb_score = min(new_cur.end - new_cur.start, user.rbNeed);
            user.rbNeed -= rb_score;
            user_intervals[user.id].emplace_back(&new_cur, rb_score);
            users_data[user.beam].pop_back();
        }
    }
    for (auto id : check_users) {
        auto &user = userInfos[id];
        int cnt = 0;
        for (auto interval : user_intervals[id]) {
            cnt += interval.second;
        }
        if (!users_data[user.beam].empty() && users_data[user.beam].back().rbNeed >= cnt) {
            int new_id = users_data[user.beam].back().id;
            for (auto interval : user_intervals[id]) {
                userInfos[new_id].rbNeed -= cnt;
                user_intervals[new_id].emplace_back(interval);
                auto it = find(interval.first->users.begin(), interval.first->users.end(), id);
                *it = new_id;
            }
            users_data[user.beam].pop_back();
            user.rbNeed += cnt;
            user_intervals[id].clear();
            users_data[user.beam].emplace_back(user);
        }
    }
    getBest();
    getAnswer(&cur, userInfos);
    if (&cur != &new_cur) {
        getAnswer(&new_cur, userInfos);
    }
    for (auto& temp_cur : current) {
        IntervalInfo& info = intervals[&temp_cur];
        bool flag = false;
        for (int i = 0; i < USERS_COUNT; ++i) {
            if (info.mask[i] && mask[i] && &temp_cur != user_intervals[i].back().first && &new_cur == user_intervals[i].back().first) {
                flag = true;
                break;
            }
        }
        if (flag) {
            getAnswer(&temp_cur, userInfos);
        }
    }
}


vector<Interval> getCurrent(const vector<Interval> &reservedRBs, int m, vector<UserInfo> &userInfos) {
    current.clear();
    int prev = 0;
    for (const auto &res: reservedRBs) {
        if (res.start > prev) {
            current.emplace_back(prev, res.start);
        }
        prev = res.end;
    }
    if (prev != m) {
        current.emplace_back(prev, m);
    }
    current.reserve(current.size() + 26);
    getBest();
    for (auto & i : current) {
        getAnswer(&i, userInfos);
    }
    return current;
}


Interval* getInterval() {
    int best_res = 0;
    Interval* choosen_interval = nullptr;
    for (auto& cur : current) {
        if (best_res > 0 && intervals[&cur].res >= best_res || intervals[&cur].res > best_res) {
            best_res = intervals[&cur].res;
            choosen_interval = &cur;
        }
    }

    return choosen_interval;
}

int rbs(const vector<Interval>& answer, vector<UserInfo> userInfos) {
    int rbs = 0;
    for (const auto& interval : answer) {
        for (const auto id : interval.users) {
            int current_res = min(interval.end - interval.start, userInfos[id].rbNeed);
            userInfos[id].rbNeed -= current_res;
            rbs += current_res;
        }
    }
    return rbs;
}

void check(Interval& interval, vector<UserInfo>& userInfos, const vector<UserInfo>& userInfos_temp) {
    for (auto id: interval.users) {
        for (auto interval_it = user_intervals[id].begin(); interval_it != user_intervals[id].end(); ++interval_it) {
            auto &temp = *interval_it;
            if (temp.first == &interval) {
                int cur = min(temp.second + userInfos[id].rbNeed, interval.end - interval.start);
                userInfos[id].rbNeed += temp.second - cur;
                temp.second -= temp.second - cur;
                if (temp.second == 0) {
                    user_intervals[id].erase(interval_it);
                }
                if (userInfos[id].rbNeed == userInfos_temp[id].rbNeed) {
                    users_data[userInfos[id].beam].emplace_back(userInfos[id]);
                    sort(users_data[userInfos[id].beam].begin(), users_data[userInfos[id].beam].end(),
                         [](const UserInfo &lhs, const UserInfo &rhs) {
                             return lhs.rbNeed < rhs.rbNeed;
                         });
                }
                break;
            }
        }
    }

    if (interval.end - interval.start == 0) {
        interval.start = 1e9;
        interval.end = 1e9;
        interval.users.clear();
    }
}

vector<Interval> Solver(int n, int m, int k, int j, int l,
                        vector<Interval> reservedRBs,
                        vector<UserInfo> userInfos) {
    vector<UserInfo> userInfos_temp = userInfos;
    L = l;
    for (int i = 0; i <= n; ++i) {
        user_intervals[i].clear();
    }
    for (int i = 0; i < BEAM_COUNT; ++i) {
        users_data[i].clear();
    }
    for (int i = 0; i < userInfos.size(); ++i) {
        userInfos[i].id = i;
    }
    for (const auto &user: userInfos) {
        users_data[user.beam].emplace_back(user);
    }
    for (int beam = 0; beam < BEAM_COUNT; ++beam) {
        sort(users_data[beam].begin(), users_data[beam].end(),
             [](const UserInfo &lhs, const UserInfo &rhs) {
                 return lhs.rbNeed < rhs.rbNeed;
             });
    }
    getCurrent(reservedRBs, m, userInfos);

    int sum = 0;
    while (j--) {
        Interval* choosen_interval = getInterval();
        if (choosen_interval) {
            sum += intervals[choosen_interval].res;
            modify_interval(intervals[choosen_interval], userInfos);
        }
        if (!choosen_interval || j == 0) {
            bool was = false;
            for (auto &interval: current) {
                auto it = find_if(current.begin(), current.end(), [&interval](const Interval &el) {
                    return el.start == interval.end;
                });
                if (interval.start == 1e9 || it != current.end() && it->start == 1e9) {
                    continue;
                }
                if (it != current.end()) {
                    Interval &next = *it;
                    bool flag = false;
                    while (next.end - next.start >= 1) {
                        interval.end++;
                        next.start++;
                        int cur = rbs(current, userInfos_temp);
                        if (cur <= sum) {
                            interval.end--;
                            next.start--;
                            break;
                        }
                        flag = true;
                        sum = cur;
                    }
                    while (interval.end - interval.start >= 1) {
                        interval.end--;
                        next.start--;
                        int cur = rbs(current, userInfos_temp);
                        if (cur <= sum) {
                            interval.end++;
                            next.start++;
                            break;
                        }
                        flag = true;
                        sum = cur;
                    }
                    if (flag) {
                        check(next, userInfos, userInfos_temp);
                        check(interval, userInfos, userInfos_temp);
                        if (next.start == 1e9 || interval.start == 1e9) {
                            ++j;
                            was = true;
                        }
                        getAnswer(&next, userInfos);
                        getAnswer(&interval, userInfos);
                    }
                }
            }
            if (!was) {
                break;
            }
        }

    }

    vector<Interval> best_answer;
    for (const auto &interval : current) {
        if (interval.end - interval.start != 0 && !interval.users.empty()) {
            best_answer.emplace_back(interval);
        }
    }

#ifdef __LOCAL
    if (sum != rbsCounting(best_answer, userInfos_temp)) {
        cerr << sum << ' ' << rbs(best_answer, userInfos_temp) << endl;
    }
#endif

    return best_answer;
}