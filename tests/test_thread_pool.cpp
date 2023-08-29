//
// Created by cycastic on 8/1/2023.
//
#include <gtest/gtest.h>
#include <microjit/thread_pool.h>

using namespace microjit;

class ThreadPoolTestFixture : public ::testing::Test {
private:
    ThreadPool *thread_pool{};
    int num = 0;
    void set_num(const int& p_num){
        num = p_num;
    }
public:
    void SetUp() override {
        thread_pool = new ThreadPool(3);
    }

    void TearDown() override {
        delete thread_pool;
    }

    std::future<void> queue_task(int p_num){
        return thread_pool->queue_task_method(ThreadPool::MEDIUM, this, &ThreadPoolTestFixture::set_num, p_num);
    }
    bool is_zero() const {
        return num == 0;
    }
    bool is_one() const {
        return num == 1;
    }
    bool allocation_test(){
        auto old_size = thread_pool->get_thread_count();
        thread_pool->allocate_worker();
        return thread_pool->get_thread_count() == old_size + 1;
    }
    bool deallocation_test(){
        auto old_size = thread_pool->get_thread_count();
        if (old_size == 0) return true;
        thread_pool->terminate_worker();
        // Wait for it to take effect
        ManagedThread::sleep(1000);
        return thread_pool->get_thread_count() == old_size - 1;
    }
    static bool integrity_check(const char* p_left, const char* p_right, const size_t& size) {
        for (size_t i = 0; i < size; i++){
            if (p_left[i] != p_right[i]) return false;
        }
        return true;
    }
    ThreadPool* get_pool() { return thread_pool; }
};

static void batch_copy_job(uint8_t p_thread_no, uint8_t p_thread_count, void* p_dst, const void* p_src, size_t p_size){
    size_t partition_size = p_size / p_thread_count;
    size_t offset = partition_size * p_thread_no;
    if (p_thread_no == p_thread_count - 1)
        memcpy((void*)((size_t)p_dst + offset), (const void*)((size_t)p_src + offset), p_size - offset);
    else
        memcpy((void*)((size_t)p_dst + offset), (const void*)((size_t)p_src + offset), partition_size);
}
static void batch_copy(ThreadPool *p_pool, ThreadPool::Priority p_priority, uint8_t p_thread_count, void* p_dst, const void* p_src, size_t p_size){
    constexpr auto wait_time = 10;
    if (p_thread_count == 0) p_thread_count = p_pool->get_thread_count();
    auto promise = p_pool->queue_group_task(p_priority, p_thread_count, batch_copy_job,
                                            p_dst, p_src, p_size);
    promise.wait();
}

TEST_F(ThreadPoolTestFixture, TestThreadPool){
    static constexpr auto size = 1024 * 1024 * 64;
    EXPECT_TRUE(allocation_test());
    EXPECT_TRUE(deallocation_test());
    EXPECT_TRUE(is_zero());
    queue_task(1).wait();
    EXPECT_TRUE(is_one());
    queue_task(0).wait();
    EXPECT_TRUE(is_zero());

    char* a = new char[size];
    char* b = new char[size];

    for (size_t i = 0; i < size; i++)
        a[i] = 42;

    batch_copy(get_pool(), ThreadPool::MEDIUM, get_pool()->get_thread_count() * 2, b, a, size);
    EXPECT_TRUE(integrity_check(a, b, size));

    delete[] a;
    delete[] b;
}
