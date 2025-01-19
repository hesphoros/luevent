Google Test 提供了许多宏来帮助你编写和管理单元测试。除了 `EXPECT_STREQ`，还有许多其他常用的断言宏和功能宏。下面是一些常见的 Google Test 宏及其用途：

### 断言宏

1. **`EXPECT_EQ(val1, val2)`**  
   检查 `val1` 和 `val2` 是否相等（使用 `operator==` 比较）。如果不相等，测试失败。
   ```cpp
   EXPECT_EQ(1 + 1, 2);  // 通过
   EXPECT_EQ(1 + 1, 3);  // 失败
   ```

2. **`EXPECT_NE(val1, val2)`**  
   检查 `val1` 和 `val2` 是否不相等（使用 `operator!=` 比较）。如果相等，测试失败。
   ```cpp
   EXPECT_NE(1 + 1, 3);  // 通过
   EXPECT_NE(1 + 1, 2);  // 失败
   ```

3. **`EXPECT_LT(val1, val2)`**  
   检查 `val1` 是否小于 `val2`（使用 `<` 比较）。如果 `val1` 不小于 `val2`，测试失败。
   ```cpp
   EXPECT_LT(1, 2);  // 通过
   EXPECT_LT(3, 2);  // 失败
   ```

4. **`EXPECT_LE(val1, val2)`**  
   检查 `val1` 是否小于或等于 `val2`（使用 `<=` 比较）。如果 `val1` 大于 `val2`，测试失败。
   ```cpp
   EXPECT_LE(1, 2);  // 通过
   EXPECT_LE(2, 2);  // 通过
   EXPECT_LE(3, 2);  // 失败
   ```

5. **`EXPECT_GT(val1, val2)`**  
   检查 `val1` 是否大于 `val2`（使用 `>` 比较）。如果 `val1` 不大于 `val2`，测试失败。
   ```cpp
   EXPECT_GT(3, 2);  // 通过
   EXPECT_GT(2, 3);  // 失败
   ```

6. **`EXPECT_GE(val1, val2)`**  
   检查 `val1` 是否大于或等于 `val2`（使用 `>=` 比较）。如果 `val1` 小于 `val2`，测试失败。
   ```cpp
   EXPECT_GE(3, 2);  // 通过
   EXPECT_GE(2, 2);  // 通过
   EXPECT_GE(1, 2);  // 失败
   ```

7. **`EXPECT_TRUE(condition)`**  
   检查 `condition` 是否为 `true`。如果不为 `true`，测试失败。
   ```cpp
   EXPECT_TRUE(1 + 1 == 2);  // 通过
   EXPECT_TRUE(1 + 1 == 3);  // 失败
   ```

8. **`EXPECT_FALSE(condition)`**  
   检查 `condition` 是否为 `false`。如果不为 `false`，测试失败。
   ```cpp
   EXPECT_FALSE(1 + 1 == 3);  // 通过
   EXPECT_FALSE(1 + 1 == 2);  // 失败
   ```

9. **`EXPECT_NULL(ptr)`**  
   检查 `ptr` 是否为 `nullptr`。如果不是，测试失败。
   ```cpp
   EXPECT_NULL(nullptr);  // 通过
   EXPECT_NULL((void*)0);  // 通过
   ```

10. **`EXPECT_NOTNULL(ptr)`**  
    检查 `ptr` 是否不为 `nullptr`。如果是，测试失败。
    ```cpp
    EXPECT_NOTNULL((void*)0x1234);  // 通过
    EXPECT_NOTNULL(nullptr);  // 失败
    ```

11. **`EXPECT_THROW(statement, exception_type)`**  
    检查 `statement` 是否抛出指定类型的异常 `exception_type`。
    ```cpp
    EXPECT_THROW(throw std::runtime_error("error"), std::runtime_error);  // 通过
    EXPECT_THROW(throw 42, std::runtime_error);  // 失败
    ```

12. **`EXPECT_ANY_THROW(statement)`**  
    检查 `statement` 是否抛出任何异常。如果没有抛出异常，测试失败。
    ```cpp
    EXPECT_ANY_THROW(throw std::runtime_error("error"));  // 通过
    EXPECT_ANY_THROW(throw 42);  // 通过
    EXPECT_ANY_THROW(1 + 1);  // 失败
    ```

13. **`EXPECT_NO_THROW(statement)`**  
    检查 `statement` 是否没有抛出任何异常。如果抛出了异常，测试失败。
    ```cpp
    EXPECT_NO_THROW(1 + 1);  // 通过
    EXPECT_NO_THROW(throw std::runtime_error("error"));  // 失败
    ```

### 宏集合

1. **`ASSERT_*` vs `EXPECT_*`**  
   - `ASSERT_*` 和 `EXPECT_*` 的区别在于：
     - `ASSERT_*` 失败时会立即停止当前测试函数的执行，不再执行后续的代码。
     - `EXPECT_*` 允许后续代码继续执行，即使前面的断言失败。适用于你想继续执行其他检查的情况。
   
   例如：
   ```cpp
   ASSERT_EQ(x, 5);  // 如果 x != 5，后续代码不会执行
   EXPECT_EQ(x, 5);  // 如果 x != 5，后续代码仍然会执行
   ```

2. **`SUCCEED()`**  
   这个宏用于强制通过测试。通常用于测试中断或某些情况下没有明显的断言。
   ```cpp
   SUCCEED();  // 强制测试通过
   ```

3. **`FAIL()`**  
   这个宏用于强制失败测试。可以用于某些特定场景，作为占位符或特殊失败条件。
   ```cpp
   FAIL() << "This test is expected to fail";  // 强制失败
   ```

4. **`INSTANTIATE_TEST_SUITE_P`**  
   用于为测试套件创建多组测试用例。通过这个宏，你可以为相同的测试编写多个实例。
   ```cpp
   INSTANTIATE_TEST_SUITE_P(MyTestInstance, MyTest, ::testing::Values(1, 2, 3));
   ```

### 测试生命周期相关宏

1. **`TEST`**  
   定义一个测试用例。每个 `TEST` 都是一个单独的测试函数。
   ```cpp
   TEST(MyTestSuite, TestName) {
       EXPECT_EQ(1 + 1, 2);
   }
   ```

2. **`TEST_F`**  
   定义一个使用测试夹具的测试用例。`TEST_F` 用于类定义的测试，提供了一个构造函数来初始化成员。
   ```cpp
   class MyTest : public ::testing::Test {
   protected:
       void SetUp() override {
           // 设置代码
       }
       void TearDown() override {
           // 清理代码
       }
   };

   TEST_F(MyTest, TestName) {
       EXPECT_EQ(1 + 1, 2);
   }
   ```

### 其他常用宏

1. **`GTEST_SKIP()`**  
   用于跳过某个测试。
   ```cpp
   GTEST_SKIP() << "Skipping this test";
   ```

2. **`::testing::Test::SetUp()` 和 `::testing::Test::TearDown()`**  
   `SetUp` 和 `TearDown` 是测试夹具的方法，用于在每个测试前后执行初始化和清理工作。

---

### 总结

Google Test 提供了大量的宏来处理断言、测试控制流、异常处理、测试生命周期等，灵活地组合这些宏可以帮助你编写高效且可维护的单元测试。对于基本的断言，常用的宏如 `EXPECT_EQ`、`EXPECT_TRUE`、`EXPECT_THROW` 等已经能满足大部分需求。