"""Convierte web/index.html en src/index_html.h (la página va dentro del firmware).

PlatformIO lo ejecuta solo antes de compilar (extra_scripts en platformio.ini).
También se puede correr a mano:  python tools/embed_web.py
"""
import os

try:
    Import("env")  # noqa: F821  (definido por PlatformIO/SCons)
    ROOT = env["PROJECT_DIR"]  # noqa: F821
except NameError:
    ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

src = os.path.join(ROOT, "web", "index.html")
dst = os.path.join(ROOT, "src", "index_html.h")

with open(src, encoding="utf-8") as f:
    html = f.read()
if ')rawliteral"' in html:
    raise SystemExit("index.html no puede contener la secuencia )rawliteral\"")

out = (
    "// ARCHIVO GENERADO por tools/embed_web.py a partir de web/index.html. No editar.\n"
    "#pragma once\n"
    'static const char INDEX_HTML[] = R"rawliteral(' + html + ')rawliteral";\n'
)
old = open(dst, encoding="utf-8").read() if os.path.exists(dst) else None
if old != out:
    with open(dst, "w", encoding="utf-8") as f:
        f.write(out)
    print("embed_web: src/index_html.h actualizado (%d bytes de HTML)" % len(html.encode()))
