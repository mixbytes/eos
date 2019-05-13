#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include <simulator.hpp>
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;

using std::string;

TEST(eos_finality, one_node) {
    auto runner = TestRunner(1);
    runner.add_stop_task(2 * runner.get_slot_ms());
    runner.run<Node>();
    EXPECT_EQ(get_block_height(runner.get_db(0).get_master_block_id()), 2);
}

TEST(eos_finality, three_nodes) {
    auto runner = TestRunner(3);
    vector<pair<int, int> > v0{{1, 2}, {2, 10}};
    graph_type g{v0};
    runner.load_graph(g);
    runner.add_stop_task(7 * runner.get_slot_ms());
    runner.run<Node>();
    EXPECT_EQ(get_block_height(runner.get_db(0).last_irreversible_block_id()), 1);
    EXPECT_EQ(get_block_height(runner.get_db(1).last_irreversible_block_id()), 1);
    EXPECT_EQ(get_block_height(runner.get_db(2).last_irreversible_block_id()), 1);
}

TEST(eos_finality, master_chain_height) {
    auto runner = TestRunner(3);
    vector<pair<int, int> > v0{{1, 2}, {2, runner.get_slot_ms()}};
    graph_type g{v0};
    runner.load_graph(g);
    runner.add_stop_task(runner.get_slot_ms());
    runner.run<Node>();
    if (get_block_height(runner.get_db(0).get_master_block_id()) == 1) {
        EXPECT_EQ(get_block_height(runner.get_db(0).get_master_block_id()), 1);
        EXPECT_EQ(get_block_height(runner.get_db(1).get_master_block_id()), 1);
        EXPECT_EQ(get_block_height(runner.get_db(2).get_master_block_id()), 0);
    }
    else {
        EXPECT_EQ(get_block_height(runner.get_db(0).get_master_block_id()), 0);
        EXPECT_EQ(get_block_height(runner.get_db(1).get_master_block_id()), 0);
        EXPECT_EQ(get_block_height(runner.get_db(2).get_master_block_id()), 1);
    }
}

TEST(eos_finality, multiple_blocks) {
    auto runner = TestRunner(3, 2);
    vector<pair<int, int> > v0{{1, 2}, {2, 10}};
    graph_type g{v0};
    runner.load_graph(g);
    runner.add_stop_task(7 * runner.get_slot_ms());
    runner.run<Node>();
    EXPECT_EQ(get_block_height(runner.get_db(0).last_irreversible_block_id()), 2);
    EXPECT_EQ(get_block_height(runner.get_db(1).last_irreversible_block_id()), 2);
    EXPECT_EQ(get_block_height(runner.get_db(2).last_irreversible_block_id()), 2);

    auto master = runner.get_db(0).get_master_head();
    EXPECT_EQ(get_block_height(master->block_id), 14);
    runner.add_stop_task(13 * runner.get_slot_ms());
    runner.run_loop();
}

TEST(eos_finality, large_roundtrip) {
    auto runner = TestRunner(3);
    vector<pair<int, int> > v0{{1, 2}, {2, 3000}};
    graph_type g{v0};
    runner.load_graph(g);
    runner.add_update_delay_task(3 * runner.get_slot_ms(), 0, 2, 10);
    runner.add_stop_task(9 * runner.get_slot_ms());
    runner.run<Node>();
    EXPECT_GE(get_block_height(runner.get_db(0).last_irreversible_block_id()), 1);
    EXPECT_GE(get_block_height(runner.get_db(1).last_irreversible_block_id()), 1);
    EXPECT_GE(get_block_height(runner.get_db(2).last_irreversible_block_id()), 1);
}

TEST(eos_finality, many_nodes) {
    size_t nodes_amount = 21;
    auto runner = TestRunner(nodes_amount);
    vector<pair<int, int> > v0{{1, 20}, {2, 10}, {3, 50}, {4, 30}, {5, 100}};
    vector<pair<int, int> > v5{{6, 10}, {7, 30}, {8, 20}, {9, 10}, {10, 100}};
    vector<pair<int, int> > v10{{11, 10}, {12, 10}, {13, 10}, {14, 10}, {15, 100}};
    vector<pair<int, int> > v15{{16, 10}, {17, 10}, {18, 10}, {19, 10}, {20, 10}};
    graph_type g(nodes_amount);
    g[0] = v0;
    g[5] = v5;
    g[10] = v10;
    g[15] = v15;
    runner.load_graph(g);
    runner.add_stop_task(31 * runner.get_slot_ms());
    runner.run<Node>();
    EXPECT_EQ(get_block_height(runner.get_db(0).last_irreversible_block_id()), 1);
    EXPECT_EQ(get_block_height(runner.get_db(5).last_irreversible_block_id()), 1);
    EXPECT_EQ(get_block_height(runner.get_db(10).last_irreversible_block_id()), 1);
    EXPECT_EQ(get_block_height(runner.get_db(15).last_irreversible_block_id()), 1);
}