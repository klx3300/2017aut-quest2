#include "include/gtest/gtest.h"
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

extern "C" {
#include "lab_vector.h"
}

class VectorTest : public testing::Test {
protected:
    virtual void SetUp() {
        c_vec = vector.constructor();
    }
    virtual void TearDown() {
        vector.destructor(&c_vec);
    }
    VectorDescriptor c_vec;

};

TEST(randomPush, timing) {
    // initializations
    const unsigned elem_cnt = 1000000;
    std::default_random_engine rand_eng(810);
    std::uniform_real_distribution<double> distribution(0, 100);

    // generate the elements to be pushed
    std::array<double, elem_cnt> elems;
    for (unsigned i = 0; i != elem_cnt; ++i) {
        elems[i] = distribution(rand_eng);
    }

    // loads of pushing & timing
    std::vector<std::pair<unsigned, double>> data_points;
    for (unsigned count = 100; count <= elem_cnt; count *= 10) {
        auto c_vec = vector.constructor();
        auto start = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i != count; ++i) {
            vector.push_back(&c_vec, &elems[i], sizeof(double));
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        data_points.push_back(std::make_pair(count, diff.count()));
        vector.destructor(&c_vec);
    }
    // for (unsigned count = 100; count <= elem_cnt; count *= 10) {
    //     std::vector<double> vec;
    //     auto start = std::chrono::high_resolution_clock::now();
    //     for (unsigned i = 0; i != count; ++i) {
    //         vec.push_back(elems[i]);
    //     }
    //     auto end = std::chrono::high_resolution_clock::now();
    //     std::chrono::duration<double> diff = end - start;
    //     data_points.push_back(std::make_pair(count, diff.count()));
    // }

    // print the result
    std::cout << "N\tduration" << std::endl;
    for (auto data : data_points)
        std::cout << data.first << "\t" << data.second << std::endl;
}

void random_push(VectorDescriptor &c_vec, std::vector<double> &std_vec,
                 size_t count);

#define TRAVERSE_CHECK() do {    /* validation*/\
    /* size */\
    ASSERT_EQ(vector.size(&c_vec), std_vec.size()) << "wrong size";\
    /* get the iterators */\
    auto beg = vector.begin(&c_vec);\
    auto end = vector.end(&c_vec);\
    for (auto elem : std_vec) {\
        auto c_elem = *reinterpret_cast<double*>(vector.iter_dereference(beg));\
        ASSERT_DOUBLE_EQ(c_elem, elem) << "found unmatched element";\
        beg = vector.iter_move(beg, 1);\
    }\
    ASSERT_EQ(beg, end) << "end iterator not functioning";\
} while(0);

TEST_F(VectorTest, iterator) {
    // constructions
    std::vector<double> std_vec;

    // random push
    random_push(c_vec, std_vec, 2333);

    TRAVERSE_CHECK();
}

TEST_F(VectorTest, randomPop) {
    // constructions
    std::vector<double> std_vec;

    // random push
    random_push(c_vec, std_vec, 233);
    vector.pop_back(&c_vec);
    std_vec.pop_back();

    random_push(c_vec, std_vec, 132);
    for (unsigned i = 0; i != 10; ++i) {
        vector.pop_back(&c_vec);
        std_vec.pop_back();
    }

    random_push(c_vec, std_vec, 112);
    for (unsigned i = 0; i != 20; ++i) {
        vector.pop_back(&c_vec);
        std_vec.pop_back();
    }

    TRAVERSE_CHECK();
}

TEST_F(VectorTest, randomErase) {
    // constructions
    std::vector<double> std_vec;

    // random push
    random_push(c_vec, std_vec, 233);
    auto iter = vector.begin(&c_vec);
    vector.erase(&c_vec, iter);
    std_vec.erase(std_vec.begin());

    random_push(c_vec, std_vec, 233);
    iter = vector.begin(&c_vec);
    iter = vector.iter_move(iter, 10);
    vector.erase(&c_vec, iter);
    std_vec.erase(std_vec.begin()+10);


    random_push(c_vec, std_vec, 233);
    for (unsigned i = 0; i != 23; ++i) {
        iter = vector.begin(&c_vec);
        iter = vector.iter_move(iter, 5);
        vector.erase(&c_vec, iter);
        std_vec.erase(std_vec.begin()+5);
    }

    TRAVERSE_CHECK();
}

TEST_F(VectorTest, clear) {
    std::default_random_engine rand_eng(810);
    std::uniform_real_distribution<double> distribution(0, 100);
    for (unsigned i = 0; i != 1000; ++i) {
        for (unsigned j = 0; j != 20000; ++j) {
            double real = distribution(rand_eng);
            vector.push_back(&c_vec, &real, sizeof(double));
        }
        vector.clear(&c_vec);
    }

    std::vector<double> std_vec;
    random_push(c_vec, std_vec, 1000);
    
    TRAVERSE_CHECK();
}

TEST_F(VectorTest, shrink) {
    std::default_random_engine rand_eng(810);
    std::uniform_real_distribution<double> distribution(0, 100);
    for (unsigned j = 0; j != 20000; ++j) {
        double real = distribution(rand_eng);
        vector.push_back(&c_vec, &real, sizeof(double));
    }
    for (unsigned i = 0; i != 1000; ++i) {
        for (unsigned j = 0; j != 10000; ++j) {
            double real = distribution(rand_eng);
            vector.push_back(&c_vec, &real, sizeof(double));
        }
        for (unsigned j = 0; j != 10000; ++j) {
            vector.pop_back(&c_vec);
        }
        vector.shrink_to_fit(&c_vec);
    }
    std::vector<double> std_vec;
    random_push(c_vec, std_vec, 1000);
    
    TRAVERSE_CHECK();
}
TEST_F(VectorTest, reserve) {
    std::default_random_engine rand_eng(810);
    std::uniform_real_distribution<double> distribution(0, 100);
    for (unsigned j = 0; j != 20000; ++j) {
        double real = distribution(rand_eng);
        vector.push_back(&c_vec, &real, sizeof(double));
    }
    for (unsigned i = 0; i != 1000; ++i) {
        vector.reserve(&c_vec, 30000);
        vector.shrink_to_fit(&c_vec);
    }
    std::vector<double> std_vec;
    random_push(c_vec, std_vec, 1000);
    
    TRAVERSE_CHECK();
}
void random_push(VectorDescriptor &c_vec, std::vector<double> &std_vec,
                 size_t count) {
    std::default_random_engine rand_eng(810);
    std::uniform_real_distribution<double> distribution(0, 100);
    for (unsigned i = 0; i != count; ++i) {
        double real = distribution(rand_eng);
        vector.push_back(&c_vec, &real, sizeof(double));
        std_vec.push_back(real);
    }
}
