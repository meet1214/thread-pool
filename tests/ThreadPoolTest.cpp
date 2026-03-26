#include <atomic>
#include <future>
#include<gtest/gtest.h>
#include <stdexcept>
#include <vector>
#include "../include/ThreadPool.h"

class ThreadPoolTest : public testing::Test {
protected:
    void SetUp() override {
        pool = std::make_unique<ThreadPool>(4);
    }
    void TearDown() override {
        pool->shutdown();
    }
    std::unique_ptr<ThreadPool> pool;
};

TEST(Construction, ValidThreadCount) {
    ThreadPool pool(4);
    EXPECT_EQ(pool.thread_count(), 4);
    EXPECT_FALSE(pool.is_stopped());
}

TEST(Construction, ZeroThreadThrows){
    EXPECT_THROW(ThreadPool(0), std::invalid_argument);
}

TEST(Construction, SingleThread) {
    ThreadPool pool(1);
    EXPECT_EQ(pool.thread_count(), 1);
}

TEST_F(ThreadPoolTest,SubmitReturnsCorrectValue){
    auto fut = pool->submit([]{ return 42; });
    ASSERT_EQ(fut.get(), 42);
}

TEST_F(ThreadPoolTest,SubmitWithArgument){
    auto fut = pool->submit([](int a, int b){ return a + b; }, 3, 4);
    EXPECT_EQ(fut.get(), 7);
}

TEST_F(ThreadPoolTest,SubmitMultipleTasks){
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    for(int i = 0; i < 100; i++){
        futures.push_back(pool->submit([&counter]{ counter++; }));
    }

    for(auto& f : futures)
        f.get();

    EXPECT_EQ(counter, 100);
}

TEST_F(ThreadPoolTest, SubmitVoidTask){
    std::atomic<int> counter{0};

    auto fut = pool->submit([&counter]{ counter++; });
    fut.get(); 

    EXPECT_EQ(counter, 1);
}

TEST_F(ThreadPoolTest, ExceptionPropagatedThroughFuture){
    auto fut = pool->submit([]() -> int {
        throw std::runtime_error("boom");
        return 0;
    });
    EXPECT_THROW(fut.get(), std::runtime_error);
}

TEST_F(ThreadPoolTest, PoolSurvivesException){
    auto fut = pool->submit([]() -> int {
        throw std::runtime_error("boom");
        return 0;
    });   
    try { fut.get(); } catch(const std::runtime_error&) {}

    auto norm = pool->submit([]{ return 42; });
    ASSERT_EQ(norm.get(), 42);
}

TEST_F(ThreadPoolTest, ShutdownIsIdempotent){
    EXPECT_NO_THROW(pool->shutdown());
    EXPECT_NO_THROW(pool->shutdown());
}

TEST_F(ThreadPoolTest, SubmitAfterShutdownThrows){
    pool->shutdown();
    EXPECT_THROW(pool->submit([]{ return 1; }), std::runtime_error);  
}

TEST_F(ThreadPoolTest, DestructorCallsShutdown){
    EXPECT_NO_THROW({
        ThreadPool local_pool(4);
    });
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}