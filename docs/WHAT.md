<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Language: </strong>
  
  <a href="./WHAT.ru.md" style="color: #F5F752; margin: 0 10px;">
    🇷🇺 Russian
  </a>
  | 
  <span style="color: #0891b2; margin: 0 10px;">
    ✅ 🇺🇸 English (current)
  </span>
</div>

---

**Freelancer XML Project** is considered a gold standard among tools for working with `.cmp`, `.3db`, `.mat`, and other binary files in UTF format. The main author is **Jason Hood (Adoxa)**, while the original utility was first created by **Sir Lancelot**.

---

## Freelancer XML Project Capabilities

### Included Tools

- `UTFXML.exe` — **unpacks** `.cmp`, `.3db`, `.mat`, `.txm` into **XML** with extraction of embedded resources (`.wav`, `.tga`, `.dds`, etc.)
- `XMLUTF.exe` — **rebuilds files back** into `.cmp` or `.3db` from XML

---

## Core Features

| Feature         | Description                                                            |
| --------------- | ---------------------------------------------------------------------- |
| Unpacking       | Extracts `UTF` file structure into `XML`, including embedded resources |
| Editing         | Allows editing data in any text editor                                 |
| Rebuild         | Rebuilds XML back into `.cmp`, `.mat`, `.txm`, and other formats       |
| Named Hashes    | Supports `hash="gcs_refer_fc_new_short"` with automatic ID resolution  |
| Transformations | RGB → `#RRGGBB`, quaternions → axis-angle, radians → degrees           |
| Batch Mode      | Process multiple files from a directory                                |

---

## Usage Examples

### Unpack `.cmp` into XML

```bash id="f0j8wp"
utfxml -ag -o output_folder solar\dockring\ring_cmp.cmp
```

### Build XML back into binary format

```bash id="2g7uoc"
xmlutf -o compiled_folder output_folder\ringer_cmp.xml
```

---

## Practical Example: Adding Animation

1. Unpack the model:

```bash id="j0a8yz"
utfxml station.cmp > station.xml
```

2. Insert the required node:

```xml id="s2l3mn"
<Node name="solar_panel">
  <Animation>
    <Name>Sc_rotate</Name>
  </Animation>
</Node>
```

3. Rebuild the file:

```bash id="d9x6rt"
xmlutf station.xml > station.cmp
```

4. Add to INI:

```ini id="n8p4hv"
[InternalFX]
nickname = rotate_solar
use_animation = Sc_rotate solar_panel
```

After that, the rotating solar panel will function correctly.

---
