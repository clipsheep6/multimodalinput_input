#pragma once
#include <utility>
#include <list>
#include <vector>
#include <deque>
#include <set>
#include <map>
#include <sstream>

namespace Input {

    template <typename T>
    std::ostream& operator<<(std::ostream& outStream, const std::shared_ptr<T>& item) {
        if (item) {
            return item->operator<<(outStream);
        }

        return outStream << "(null)";
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& outStream, const std::unique_ptr<T>& item) {
        if (item) {
            return item->operator<<(outStream);
        }
        return outStream << "(null)";
    }

    template <typename T1, typename T2>
    std::ostream& operator<<(std::ostream& outStream, const std::pair<T1, T2>& item) {
        return outStream << item.first << ':' << item.second;
    }

    template <typename T1, typename T2>
    std::ostream& operator<<(std::ostream& outStream, const std::map<T1, T2>& items) {
        outStream << '{';

        bool isFirst = true;
        for (const auto& item : items) {
            if (isFirst) {
                isFirst = false;
            } else {
                outStream << ',';
            }
            outStream << item;
        }

        outStream << '}';
        return outStream;
    }


    template <typename T>
    std::ostream& operator<<(std::ostream& outStream, const std::list<T>& items) {
        outStream << '[';

        bool isFirst = true;
        for (const auto& item : items) {
            if (isFirst) {
                isFirst = false;
            } else {
                outStream << ',';
            }
            outStream << item;
        }

        outStream << ']';

        return outStream;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& outStream, const std::vector<T>& items) {
        outStream << '[';

        bool isFirst = true;
        for (const auto& item : items) {
            if (isFirst) {
                isFirst = false;
            } else {
                outStream << ',';
            }
            outStream << item;
        }

        outStream << ']';

        return outStream;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& outStream, const std::deque<T>& items) {
        outStream << '[';

        bool isFirst = true;
        for (const auto& item : items) {
            if (isFirst) {
                isFirst = false;
            } else {
                outStream << ',';
            }
            outStream << item;
        }

        outStream << ']';

        return outStream;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& outStream, const std::set<T>& items) {
        outStream << '[';

        bool isFirst = true;
        for (const auto& item : items) {
            if (isFirst) {
                isFirst = false;
            } else {
                outStream << ',';
            }
            outStream << item;
        }

        outStream << ']';

        return outStream;
    }

    }
