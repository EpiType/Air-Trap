# 🧪 Tests Module

Unit tests for the Air-Trap project using Google Test framework.

## Directory Structure

```
tests/
├── ecs/
│   ├── test_components.cpp   # Component functionality
│   ├── test_registry.cpp     # Registry operations
│   ├── test_sparsearray.cpp  # SparseArray data structure
│   └── test_zipview.cpp      # ZipView iteration
├── logger/
│   └── test_logger.cpp       # Logging system
├── network/
│   └── test_protocol.cpp     # Network protocol
└── CMakeLists.txt            # Test configuration
```

## Running Tests

### All Tests

```bash
./scripts/build.sh test
```

### Manual Execution

```bash
cd build
ctest --output-on-failure
```

### Specific Test Suite

```bash
cd build
./bin/rtype_tests --gtest_filter="RegistryTest.*"
```

## Test Coverage

### ECS Tests

| Test File | Coverage |
|-----------|----------|
| `test_registry.cpp` | Entity spawn/kill, component add/remove |
| `test_sparsearray.cpp` | Insert, erase, access, iteration |
| `test_zipview.cpp` | Multi-component iteration |
| `test_components.cpp` | Component default values |

### Network Tests

| Test File | Coverage |
|-----------|----------|
| `test_protocol.cpp` | Packet serialization, OpCode handling |

## Example Test

```cpp
// tests/ecs/test_registry.cpp
TEST_F(RegistryTest, SpawnEntity) {
    auto result = registry->spawn();
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(result.value().isNull());
}

TEST_F(RegistryTest, AddComponent) {
    registry->subscribe<Transform>();
    auto entity = registry->spawn().value();
    
    auto result = registry->add<Transform>(entity, Vec2f{100.f, 200.f});
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().get().position.x, 100.f);
}
```

## Code Coverage

Generate coverage report:

```bash
./scripts/build.sh coverage
```

Report saved to `build/coverage.xml`.

## Adding New Tests

1. Create test file in appropriate directory
2. Include Google Test:
   ```cpp
   #include <gtest/gtest.h>
   ```
3. Add to `CMakeLists.txt`:
   ```cmake
   target_sources(rtype_tests PRIVATE
       new_test.cpp
   )
   ```

## Dependencies

- **Google Test 1.14.0**: Testing framework (via Conan)
