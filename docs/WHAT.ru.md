<div align="center" style="margin: 20px 0; padding: 10px; background: #1c1917; border-radius: 10px;">
  <strong>🌐 Язык: </strong>
  
  <span style="color: #F5F752; margin: 0 10px;">
    ✅ 🇷🇺 Русский (текущий)
  </span>
  | 
  <a href="./WHAT.md" style="color: #0891b2; margin: 0 10px;">
    🇺🇸 English
  </a>
</div>

---

**Freelancer XML Project** — это золотой стандарт среди инструментов для работы с `.cmp`, `.3db`, `.mat` и другими бинарными файлами формата UTF. Его автор — **Jason Hood (Adoxa)**, а первоначально утилита была создана **Sir Lancelot**.

---

## Возможности Freelancer XML Project

### В комплект входят:

- `UTFXML.exe` — **распаковка** `.cmp`, `.3db`, `.mat`, `.txm` в **XML** с извлечением вложенных файлов (`.wav`, `.tga`, `.dds` и др.)
- `XMLUTF.exe` — **сборка обратно** в `.cmp` или `.3db` из XML

---

## Основные функции

| Функция          | Описание                                                           |
| ---------------- | ------------------------------------------------------------------ |
| Распаковка       | Извлекает структуру `UTF` файла в `XML`, включая вложенные ресурсы |
| Редактирование   | Позволяет редактировать данные в любом текстовом редакторе         |
| Обратная сборка  | Сборка XML обратно в `.cmp`, `.mat`, `.txm` и другие форматы       |
| Именованные хеши | Поддержка `hash="gcs_refer_fc_new_short"` с автоопределением ID    |
| Преобразования   | RGB → `#RRGGBB`, кватернионы → ось+угол, радианы → градусы         |
| Batch-режим      | Обработка нескольких файлов из директории                          |

---

## Примеры использования

### Распаковка `.cmp` в XML

```bash
utfxml -ag -o output_folder solar\dockring\ring_cmp.cmp
```

### Сборка XML обратно

```bash
xmlutf -o compiled_folder output_folder\ringer_cmp.xml
```

---

## Практическое применение: добавление анимации

1. Распаковать модель:

```bash
utfxml station.cmp > station.xml
```

2. Вставить нужный узел:

```xml
<Node name="solar_panel">
  <Animation>
    <Name>Sc_rotate</Name>
  </Animation>
</Node>
```

3. Собрать обратно:

```bash
xmlutf station.xml > station.cmp
```

4. Добавить в INI:

```ini
[InternalFX]
nickname = rotate_solar
use_animation = Sc_rotate solar_panel
```

После этого вращающаяся солнечная панель будет работать.

---
