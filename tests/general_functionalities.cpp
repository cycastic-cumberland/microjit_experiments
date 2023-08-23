//
// Created by cycastic on 8/17/23.
//

#include <gtest/gtest.h>
#include <microjit/orchestrator.h>

using namespace microjit;

class GeneralTestFixture : public ::testing::Test {
public:
    struct TestStruct {
        float a ;
        double b;
        static int count;
        TestStruct() : a(), b() {
            count++;
        }
        TestStruct(const float& p_a, const double& p_b) : a(p_a), b(p_b) {
            count++;
        }
        TestStruct(const TestStruct& p_other) : a(p_other.a), b(p_other.b){
            count++;
        }
        ~TestStruct(){
            count--;
        }

        TestStruct& operator=(const TestStruct& p_other) {
            a = p_other.a;
            b = p_other.b;
            return *this;
        }
        bool operator==(const TestStruct& p_other) const {
            return a == p_other.a && b == p_other.b;
        }
        bool operator!=(const TestStruct& p_other) const {
            return !(*this == p_other);
        }
    };

protected:
    typedef int T1;
    typedef long T2;
    typedef float T3;
    typedef double T4;
    typedef TestStruct Ts1;

    T1 c1 = 12;
    T2 c2 = 4221;
    T3 c3 = 3.14;
    T4 c4 = 4.13;
    Ts1* cs0;
#define cs1 *cs0

    std::function<T1()> f1;
    std::function<T2()> f2;
    std::function<T3()> f3;
    std::function<T4()> f4;
    std::function<Ts1()> fs1;

    std::function<T1(T1, T1)> f5;
    std::function<T2(T2, T2)> f6;
    std::function<T3(T3, T3)> f7;
    std::function<T4(T4, T4)> f8;
    std::function<Ts1(Ts1, Ts1)> fs2;

    Ref<MicroJITOrchestrator> orchestrator;

    void build(){
#define CREATE_INSTANCE(m_count) auto i##m_count = orchestrator->create_instance_from_model(f##m_count)
#define IMM_RETURN(m_count){ \
    auto function = i##m_count->get_function(); \
    auto main_scope = function->get_main_scope();\
    auto var1 = main_scope->create_variable<T##m_count>(); \
    main_scope->construct_from_immediate<T##m_count>(var1, c##m_count); \
    auto scope1 = main_scope->create_scope();   \
    auto var2 = scope1->create_variable<T##m_count>();     \
    scope1->default_construct<T##m_count>(var2);                         \
    scope1->assign_from_variable<T##m_count>(var2, var1);\
    scope1->function_return(var2);      \
    f##m_count = i##m_count->get_compiled_function(); \
}
#define ARG_RETURN(m_count){ \
    auto function = i##m_count->get_function(); \
    auto main_scope = function->get_main_scope();\
    auto var1 = main_scope->create_variable<decltype(f##m_count)::first_argument_type>(); \
    main_scope->construct_from_argument(var1, 1); \
    auto scope1 = main_scope->create_scope();\
    scope1->function_return(var1);              \
    auto junk = main_scope->create_variable<decltype(f##m_count)::first_argument_type>();\
    f##m_count = i##m_count->get_compiled_function(); \
}
        CREATE_INSTANCE(1);
        CREATE_INSTANCE(2);
        CREATE_INSTANCE(3);
        CREATE_INSTANCE(4);
        CREATE_INSTANCE(s1);
        CREATE_INSTANCE(5);
        CREATE_INSTANCE(6);
        CREATE_INSTANCE(7);
        CREATE_INSTANCE(8);
        CREATE_INSTANCE(s2);

        IMM_RETURN(1)
        IMM_RETURN(2)
        IMM_RETURN(3)
        IMM_RETURN(4)
        IMM_RETURN(s1)

        ARG_RETURN(5)
        ARG_RETURN(6)
        ARG_RETURN(7)
        ARG_RETURN(8)
        ARG_RETURN(s2)

#undef CREATE_INSTANCE
#undef IMM_RETURN
    }
public:
    void PostSetUp() {
        orchestrator = microjit::orchestrator();
        cs0 = new TestStruct{15, 9.25};
        build();
    }
    void PreTearDown() {
        delete cs0;
        orchestrator = Ref<MicroJITOrchestrator>::null();
    }
    bool test_imm_return() const {
#define CHECK(m_count) if (f##m_count() != c##m_count) return false
        CHECK(1);
        CHECK(2);
        CHECK(3);
        CHECK(4);
        CHECK(s1);
#undef CHECK
        return true;
    }
    bool test_arg_return() {
#define CHECK(m_count, m_arg1, m_arg2) if (std::abs(f##m_count(m_arg1, m_arg2) - m_arg2) > 0.01) return false
        CHECK(5, 12, 34);
        CHECK(6, 473292, 198265);
        CHECK(7, 3.7, 2.4);
        CHECK(8, 17.124, 29.124);
        auto s1 = TestStruct{23.13, 620.213};
        auto s2 = TestStruct{620.213, 23.13};
        auto s3 = fs2(s1, s2);
        if (s2 != s3) return false;
#undef CHECK
        return true;
    }
};

int GeneralTestFixture::TestStruct::count = 0;

TEST_F(GeneralTestFixture, OrchestratorGeneralFunctionalityTest) {
    auto post_setup_count = GeneralTestFixture::TestStruct::count;
    PostSetUp();
    EXPECT_TRUE(test_imm_return());
    EXPECT_TRUE(test_arg_return());
    PreTearDown();
    auto pre_teardown_count = GeneralTestFixture::TestStruct::count;
    EXPECT_EQ(post_setup_count, pre_teardown_count);
}
