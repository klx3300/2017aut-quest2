#include "include/gtest/gtest.h"

#include <chrono>
#include <random>
#include <set>

extern "C" {
#include "lab_set.h"
}

int less(void *left, int lsize, void *right, int rsize) {
    int lhs = *reinterpret_cast<int *>(left);
    int rhs = *reinterpret_cast<int *>(right);
    return lhs < rhs;
}

class SequenceSet : public testing::Test {
  protected:
    virtual void SetUp() {
        c_set = set.constructor(less);
        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i != count; ++i) {
            unsigned elem = count - i;
            set.insert(&c_set, &elem, sizeof(unsigned));
            std_set.insert(elem);
        }
    }
    virtual void TearDown() {
        set.destructor(&c_set);
    }
    SetDescriptor c_set;
    std::set<unsigned> std_set;
    const unsigned count = 1000;
};

TEST(timing, randomPush) {
    // initializations
    const unsigned elem_cnt = 1000000;
    std::default_random_engine rand_eng(810);
    std::uniform_int_distribution<unsigned> distribution(0, 300);

    // generate the elements to be pushed
    std::array<unsigned, elem_cnt> elems;
    for (unsigned i = 0; i != elem_cnt; ++i) {
        elems[i] = distribution(rand_eng);
    }

    // loads of pushing and timing
    std::vector<std::pair<unsigned, double>> data_points;
    for (unsigned count = 100; count <= elem_cnt; count *= 10) {
        auto c_set = set.constructor(less);
        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i != count; ++i) {
            set.insert(&c_set, &elems[i], sizeof(unsigned));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        data_points.push_back(std::make_pair(count, diff.count()));
        set.destructor(&c_set);
    }

    // print the result
    std::cout << "N\tduration" << std::endl;
    for (auto data : data_points)
        std::cout << data.first << "\t" << data.second << std::endl;
}

TEST(timing, sequencePushFromZero) {
    // initializations
    const unsigned elem_cnt = 1000000;
    // loads of pushing and timing
    std::vector<std::pair<unsigned, double>> data_points;
    for (unsigned count = 100; count <= elem_cnt; count *= 10) {
        auto c_set = set.constructor(less);
        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i != count; ++i) {
            set.insert(&c_set, &i, sizeof(unsigned));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        data_points.push_back(std::make_pair(count, diff.count()));
        set.destructor(&c_set);
    }

    // print the result
    std::cout << "N\tduration" << std::endl;
    for (auto data : data_points)
        std::cout << data.first << "\t" << data.second << std::endl;
}

TEST(timing, sequencePushFromMax) {
    // initializations
    const unsigned elem_cnt = 1000000;
    // loads of pushing and timing
    std::vector<std::pair<unsigned, double>> data_points;
    for (unsigned count = 100; count <= elem_cnt; count *= 10) {
        auto c_set = set.constructor(less);
        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i != count; ++i) {
            unsigned elem = count - i;
            set.insert(&c_set, &elem, sizeof(unsigned));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        data_points.push_back(std::make_pair(count, diff.count()));
        set.destructor(&c_set);
    }

    // print the result
    std::cout << "N\tduration" << std::endl;
    for (auto data : data_points)
        std::cout << data.first << "\t" << data.second << std::endl;
}

TEST(timing, pushAndPopLeft) {
    std::vector<std::pair<unsigned, double>> data_points;
    for (unsigned count = 0; count != 1000; ++count) {
        auto c_set = set.constructor(less);

        for (unsigned i = 0; i != 10000; ++i) {
            unsigned elem = count - i;
            set.insert(&c_set, &elem, sizeof(unsigned));
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i != 1000; ++i) {
            unsigned elem = 0;
            set.insert(&c_set, &elem, sizeof(unsigned));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        data_points.push_back(std::make_pair(count, diff.count()));
        set.destructor(&c_set);
    }
    // print the result
    std::cout << "N\tduration" << std::endl;
    for (auto data : data_points)
        std::cout << data.first << "\t" << data.second << std::endl;
}

TEST(timing, pushAndPopMid) {
    std::vector<std::pair<unsigned, double>> data_points;
    for (unsigned count = 0; count != 1000; ++count) {
        auto c_set = set.constructor(less);

        for (unsigned i = 0; i != 10000; ++i) {
            unsigned elem = count - i;
            set.insert(&c_set, &elem, sizeof(unsigned));
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i != 1000; ++i) {
            unsigned elem = 5000;
            set.insert(&c_set, &elem, sizeof(unsigned));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        data_points.push_back(std::make_pair(count, diff.count()));
        set.destructor(&c_set);
    }
    // print the result
    std::cout << "N\tduration" << std::endl;
    for (auto data : data_points)
        std::cout << data.first << "\t" << data.second << std::endl;
}
TEST(timing, pushAndPopRight) {
    std::vector<std::pair<unsigned, double>> data_points;
    for (unsigned count = 0; count != 1000; ++count) {
        auto c_set = set.constructor(less);

        for (unsigned i = 0; i != 10000; ++i) {
            unsigned elem = count - i;
            set.insert(&c_set, &elem, sizeof(unsigned));
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i != 1000; ++i) {
            unsigned elem = 10001;
            set.insert(&c_set, &elem, sizeof(unsigned));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        data_points.push_back(std::make_pair(count, diff.count()));
        set.destructor(&c_set);
    }
    // print the result
    std::cout << "N\tduration" << std::endl;
    for (auto data : data_points)
        std::cout << data.first << "\t" << data.second << std::endl;
}

TEST_F(SequenceSet, size) {
    ASSERT_EQ(set.size(&c_set), std_set.size());
}

TEST_F(SequenceSet, traverse) {
    auto iter = set.begin(&c_set);
    for (unsigned i = 0; i != count; ++i) {
        unsigned elem = *reinterpret_cast<unsigned*>(set.iter_dereference(iter));
        ASSERT_EQ(elem, i+1);
        iter = set.iter_next(iter);
    }
}
