# Commit Message Format
Los mensajes de los commits deben categorizarse según la tabla que se encuentra más abajo. En general, los mensajes tendrán el siguiente formato:

`git commit -m "[<type>]: <description>"`

## Commit types

| Commit Type | Title                    | Description                                                                                                 |
| ----------- | ------------------------ | ----------------------------------------------------------------------------------------------------------- |
| `[feat]:`      | Features                 | A new feature                                                                                               |
| `[fix]:`       | Bug Fixes                | A bug Fix                                                                                                   |
| `[docs]:`      | Documentation            | Documentation only changes                                                                                  |
| `[style]:`     | Styles                   | Changes that do not affect the meaning of the code (white-space, formatting, missing semi-colons, etc)      |
| `[refactor]:`  | Code Refactoring         | A code change that neither fixes a bug nor adds a feature                                                   |
| `[test]:`      | Tests                    | Adding missing tests or correcting existing tests                                                           |
| `[build]:`     | Builds                   | Changes that affect the build system or external dependencies (example scopes: gulp, broccoli, npm)         |
| `[chore]:`     | Chores                   | Other changes that don't modify src or test files                                                           |
| `[revert]:`    | Reverts                  | Reverts a previous commit                                                                                   |