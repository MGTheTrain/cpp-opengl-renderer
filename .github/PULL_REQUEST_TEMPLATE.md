# Checklist

- [ ] Pre-commit hook is installed and all checks pass locally (`pre-commit run --all-files`)
- [ ] clang-tidy passes locally (`find modules apps -name '*.cpp' | grep -v third-party | xargs clang-tidy -p build --config-file=.clang-tidy`)
- [ ] Tests pass locally for the same OS as the CI target
- [ ] All merge conflicts are resolved
- [ ] `version` file is updated if this PR prepares a release
- [ ] PR title follows conventional commits (e.g. `feat:`, `fix:`, `chore:`, `docs:`)
- [ ] Code follows the project [coding guidelines](../docs/coding-guidelines.md)

# Description

<!-- Briefly describe what this PR changes and why -->

# Reference

<!-- Link to the issue this PR resolves, if any. e.g. Closes #42 -->
