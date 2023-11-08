#include <unity.h>

extern void setup();
extern void loop();

// The setup function which will run once before any tests
void setUp(void) {
    // This is where you can do any setup necessary for your tests,
    // It will run before each test
}

// The teardown function which will run after each test
void tearDown(void) {
    // This is where you can do any cleanup necessary after your tests
}

// A test that checks if the setup function can run without issues
void test_setup_runs(void) {
    setup(); // Call the setup function that should be defined in your main WLED code
    TEST_ASSERT_MESSAGE(true, "Setup completed");
}

// A test that checks if the loop function can run without issues
void test_loop_runs(void) {
    loop(); // Call the loop function that should be defined in your main WLED code
    TEST_ASSERT_MESSAGE(true, "Loop completed");
}

// The main entry point for the test runner
void setup() {
    UNITY_BEGIN(); // IMPORTANT LINE!

    RUN_TEST(test_setup_runs);
    RUN_TEST(test_loop_runs);

    UNITY_END(); // stop unit testing
}

void loop() {
    // Nothing here, tests will only run once
}
