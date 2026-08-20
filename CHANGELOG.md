<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./CHANGELOG.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

# 19.08.2026 ver. 3.0

- Added a separate `.sln` file for `Visual Studio 2022` to make form editing more convenient
- Added folder selection buttons for `XML` `import` and `export`
- Updated the contributors list and added the original creators found in `.rc` files to the main `README.md`
- Added a startup configuration for `Visual Studio Code` for code debugging
- Added support for building with `Visual Studio 2022`
  - Updated parts of the code for compatibility
- Added the [`build.bat`](build.bat) build script
- Replaced the game folder selection process with a modern folder selection approach using `FileInfo`
- Added authorship information and updated the application version to `3.0.0`
- Author: **Dvurechensky**

# 2025 ver. 2.2

- Added support for exporting the `Animation` block → `XML`
- Tested with `.cmp` files
- Other formats are **potentially supported**, but have not been fully tested
- Author: **Dvurechensky**

## 2010 ver. 2.2

- Fixed integer `ALE RGB` conversions
- The utility now continues running even if a Freelancer installation is not detected
- Added recognition of names for Alchemy type values
- Added recognition of `Q` for quaternion values
- Added command-line argument support
- Conversion summary statistics are now added to the log
- Added creation of the complete destination path
- The log is written to the system temporary directory
- Author: **Jason Hood**

## 2010 ver. 2.1

- Removed compatibility code for older versions
- The `stringfirst` and `prepaddata` attributes are still supported
- Added support for ignoring tab characters
- Fixed multi-line comment handling
- Author: **Jason Hood**

## 2010 ver. 2.0

- This version is **not compatible** with previous versions of the utility
- The utility no longer exits automatically after completing its work
- Added creation of the final destination directory
- Case is now ignored in more situations
- Added support for multi-line comments
- Added the new `hash` attribute for generating hash codes for new audio entries
- Author: **Jason Hood**

## 2004 ver. 1.1

- Added a check comparing the resulting `UTF` file size against the original file size
- Added parsing of the `unk234` attribute
- Author: **Sir Lancelot**

## 2004 ver. 1.0a

- Initial version of the utility
- Original design and core implementation
- Author: **Sir Lancelot**
