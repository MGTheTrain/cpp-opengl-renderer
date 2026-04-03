# Coding Guidelines

This project follows modern C++17 idioms. The standards below reflect what is enforced via code review and pre-commit hooks.

## Error Handling

Use `Result<T, E>` (defined in `modules/common/include/result.h`) instead of exceptions for recoverable errors. Exceptions are reserved for unrecoverable initialisation failures (e.g. GLEW/GLFW init in constructors).

```cpp
// Good
[[nodiscard]] Mgtt::Common::Result<void> Load(std::string_view path);

// Bad
void Load(std::string_view path); // throws on failure
```

## Resource Management

- Prefer RAII — resources must be owned by an object whose destructor releases them
- Use `std::unique_ptr` for single ownership, `std::shared_ptr` only when shared ownership is genuinely required
- **Prefer stack allocation for plain data.** Reach for `std::unique_ptr` only when the object must outlive its scope, is too large for the stack, or has genuinely dynamic lifetime. Wrapping a small struct in `unique_ptr` purely out of habit adds indirection with no benefit.

```cpp
// Good — plain data belongs on the stack
ViewMatrices matrices_{};

// Bad — unnecessary heap allocation for a trivial struct
std::unique_ptr<ViewMatrices> matrices_;
```

- Delete copy constructors and copy assignment on types that own GPU handles or file descriptors
- Explicitly default move constructors and move assignment

```cpp
MyType(const MyType&)            = delete;
MyType& operator=(const MyType&) = delete;
MyType(MyType&&)                 = default;
MyType& operator=(MyType&&)      = default;
```

**RAII applies only to types that directly own GL handles** (GL ids stored as member fields). For such types, the destructor must call `Clear()`:

```cpp
~OpenGlShader() override { Clear(); }  // deletes GL program
~Mesh()                  { Clear(); }  // deletes VAO/VBO/EBO
~Scene()                 { Clear(); }  // deletes shader + textures
```

Types that own GL handles only transitively through members (`PbrMaterial`, `MeshPrimitive`, `Node`) use `= default` destructors. Their `Clear()` methods exist for explicit reset semantics and are called through the `Scene::Clear()` chain, not from destructors, to avoid double-free when materials are moved between containers during scene loading.

**Virtual destructors are required only when deletion through a base pointer occurs.** The two cases in this codebase are distinct:

- **Polymorphic interfaces** (`IShader`, `IWindow`, `ISceneImporter`, `ICamera`) are held by `unique_ptr<IBase>` or referenced as `IBase&`. They must have `virtual ~IBase() = default`.
- **Data hierarchies** (`TextureBase`/`Texture`, `Material`/`PbrMaterial`) are always stored and destroyed by value. They must not have virtual destructors, the vtable pointer wastes 8 bytes per instance and signals a polymorphism contract that does not exist.

```cpp
// Good — genuine interface, deleted through base pointer
class IShader {
public:
    virtual ~IShader() = default;
};

// Good — data hierarchy, never deleted through base pointer
struct Material {
    ~Material() = default;  // no virtual
    std::string name;
};

// Bad — unnecessary vtable on a value type
struct Material {
    virtual ~Material() = default;  // adds vtable pointer to every instance
    std::string name;
};
```

## Attributes and Qualifiers

- `[[nodiscard]]` on all functions returning `Result<T>`, handles, or computed values whose discard would silently hide errors
- `noexcept` on move constructors, move assignment, and non-throwing accessors
- `const` on all member functions that do not mutate state

```cpp
[[nodiscard]] uint32_t GetProgramId() const noexcept;
```

## Naming

- Types and classes: `PascalCase`
- Member functions: `PascalCase`
- Private member variables: `trailing_underscore_` (e.g. `window_`)
- Local variables and parameters: `camelCase`
- Constants: `kPascalCase`

## String Parameters

- Use `std::string_view` for read-only string parameters
- Convert to `std::string` only once at the point where a C API or owning copy is needed

```cpp
// Good
void Compile(std::string_view path);

// Bad
void Compile(const std::string& path);
```

## Includes

Order includes as follows, each group separated by a blank line:

1. Corresponding header (in `.cpp` files)
2. System/platform headers (`GL/glew.h`, `GLFW/glfw3.h`)
3. Third-party headers (`glm`, `imgui`, `tinygltf`)
4. Project headers (`result.h`, `opengl-shader.h`)
5. Standard library headers (`<string>`, `<memory>`, `<vector>`)

## Formatting

Code is formatted with `clang-format` using the Google style. Run before committing:

```bash
pre-commit run --all-files
```

## Further Reading

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Effective Modern C++ — Scott Meyers](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)
