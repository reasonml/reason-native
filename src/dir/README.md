# Dir

`Dir` provides a consistent API for common system, user and application
directories consistently on all platforms.

All directories are returned as `Path.t`.

Dir uses the directory conventions borrowed from
[directories-rs](https://github.com/soc/directories-rs/blob/master/README.md).
It was used a a reference guide. The description of the variables below, was
coppied from that repo and adapted to this Reason implementation.


### `Dir`

| Function name    | Value on Linux                                                                             | Value on Windows            | Value on macOS                      |
| ---------------- | ------------------------------------------------------------------------------------------ | --------------------------- | ----------------------------------- |
| `Dir.home`       | `$HOME`                                                                                    | `{FOLDERID_Profile}`        | `$HOME`                             |
| `Dir.cache`      | `$XDG_CACHE_HOME`             or `$HOME/.cache`                                            | `{FOLDERID_LocalAppData}`   | `$HOME /Library/Caches`             |
| `Dir.config`     | `$XDG_CONFIG_HOME`            or `$HOME/.config`                                           | `{FOLDERID_RoamingAppData}` | `$HOME/Library/Preferences`         |
| `Dir.data`       | `$XDG_DATA_HOME`              or `$HOME/.local/share`                                      | `{FOLDERID_RoamingAppData}` | `$HOME/Library/Application Support` |
| `Dir.dataLocal`  | `$XDG_DATA_HOME`              or `$HOME/.local/share`                                      | `{FOLDERID_LocalAppData}`   | `$HOME/Library/Application Support` |
| `Dir.executable` | `Some($XDG_BIN_HOME/../bin)`  or `Some($XDG_DATA_HOME/../bin)` or `Some($HOME/.local/bin)` | `None`                      | `None`                              |
| `Dir.runtime`    | `Some($XDG_RUNTIME_DIR)`      or `None`                                                    | `None`                      | `None`                              |

### `Dir.User`

The intended use case for `UserDirs` is to query the paths of user-facing standard directories
that have been defined according to the conventions of the operating system the library is running on.

| Function name        | Value on Linux                                                     | Value on Windows                 | Value on macOS               |
| -------------------- | ------------------------------------------------------------------ | -------------------------------- | ---------------------------- |
| `Dir.User.home`      | `$HOME`                                                            | `{FOLDERID_Profile}`             | `$HOME`                      |
| `Dir.User.audio`     | `Some(XDG_MUSIC_DIR)`         or `None`                            | `Some({FOLDERID_Music})`         | `Some($HOME/Music/)`         |
| `Dir.User.desktop`   | `Some(XDG_DESKTOP_DIR)`       or `None`                            | `Some({FOLDERID_Desktop})`       | `Some($HOME/Desktop/)`       |
| `Dir.User.document`  | `Some(XDG_DOCUMENTS_DIR)`     or `None`                            | `Some({FOLDERID_Documents})`     | `Some($HOME/Documents/)`     |
| `Dir.User.download`  | `Some(XDG_DOWNLOAD_DIR)`      or `None`                            | `Some({FOLDERID_Downloads})`     | `Some($HOME/Downloads/)`     |
| `Dir.User.font`      | `Some($XDG_DATA_HOME/fonts/)` or `Some($HOME/.local/share/fonts/)` | `None`                           | `Some($HOME/Library/Fonts/)` |
| `Dir.User.picture`   | `Some(XDG_PICTURES_DIR)`      or `None`                            | `Some({FOLDERID_Pictures})`      | `Some($HOME/Pictures/)`      |
| `Dir.User.public`    | `Some(XDG_PUBLICSHARE_DIR)`   or `None`                            | `Some({FOLDERID_Public})`        | `Some($HOME/Public/)`        |
| `Dir.User.template`  | `Some(XDG_TEMPLATES_DIR)`     or `None`                            | `Some({FOLDERID_Templates})`     | `None`                       | 
| `Dir.User.video`     | `Some(XDG_VIDEOS_DIR)`        or `None`                            | `Some({FOLDERID_Videos})`        | `Some($HOME/Movies/)`        |

### `Dir.App`

The intended use case for `ProjectDirs` is to compute the location of cache, config or data directories for your own application or project,
which are derived from the standard directories.

| Function name    | Value on Linux                                                                 | Value on Windows                                  | Value on macOS                                     |
| ---------------- | ------------------------------------------------------------------------------ | ------------------------------------------------- | -------------------------------------------------- |
| `cache_dir`      | `$XDG_CACHE_HOME/<project_path>`        or `$HOME/.cache/<project_path>`       | `{FOLDERID_LocalAppData}/<project_path>/cache`    | `$HOME/Library/Caches/<project_path>`              |
| `config_dir`     | `$XDG_CONFIG_HOME/<project_path>`       or `$HOME/.config/<project_path>`      | `{FOLDERID_RoamingAppData}/<project_path>/config` | `$HOME/Library/Preferences/<project_path>`         |
| `data_dir`       | `$XDG_DATA_HOME/<project_path>`         or `$HOME/.local/share/<project_path>` | `{FOLDERID_RoamingAppData}/<project_path>/data`   | `$HOME/Library/Application Support/<project_path>` |
| `data_local_dir` | `$XDG_DATA_HOME/<project_path>`         or `$HOME/.local/share/<project_path>` | `{FOLDERID_LocalAppData}/<project_path>/data`     | `$HOME/Library/Application Support/<project_path>` |
| `runtime_dir`    | `Some($XDG_RUNTIME_DIR/_project_path_)`                                        | `None`                                            | `None`                                             |


## Platforms

This Reason implementation also allows using other platform's conventions where
it makes sense. For example, you can use the XDG convention on Mac or Windows.

```
let path = Dir.Linux.config;
let path = Dir.Linux.User.config;
```
