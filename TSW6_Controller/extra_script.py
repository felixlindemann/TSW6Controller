# extra_script.py
# -------------------------------------------------------------
#   1. Erhöht automatisch die Buildnummer
#   2. Lädt das Filesystem-Image (LittleFS) hoch, wenn sich Dateien geändert haben
# -------------------------------------------------------------

import os, time
from SCons.Script import Import, DefaultEnvironment # type: ignore

Import("env")
env = DefaultEnvironment()

# -------------------------------------------------------------
# === Build Number Handling ===
# -------------------------------------------------------------
build_file = os.path.join(env["PROJECT_DIR"], "build_number.txt")

if os.path.exists(build_file):
    with open(build_file, "r+") as f:
        num = int((f.read() or "0").strip() or 0) + 1
        f.seek(0)
        f.write(str(num))
        f.truncate()
else:
    num = 1
    with open(build_file, "w") as f:
        f.write("1")

env.Append(BUILD_FLAGS=[f"-DBUILD_NUMBER={num}"])
print(f"==> Build number set to {num}")

# -------------------------------------------------------------
# === Filesystem Auto-Upload ===
# -------------------------------------------------------------
project_dir = env["PROJECT_DIR"]
data_dir = os.path.join(project_dir, "data")
marker_file = os.path.join(project_dir, ".pio", "data_upload_time")

def need_uploadfs():
    """Prüft, ob Dateien in /data/ neuer sind als der letzte Upload."""
    if not os.path.isdir(data_dir):
        return False
    last_upload = 0
    if os.path.exists(marker_file):
        last_upload = os.path.getmtime(marker_file)
    for root, _, files in os.walk(data_dir):
        for f in files:
            path = os.path.join(root, f)
            if os.path.getmtime(path) > last_upload:
                print(f"  ⟳ geänderte Datei erkannt: {os.path.relpath(path, project_dir)}")
                return True
    return False

def before_upload(source, target, env):
    """Hook: Vor jedem Firmware-Upload ausführen."""
    if need_uploadfs():
        print("\n=== Detected changes in /data → uploading LittleFS ===")
        env.Execute("pio run --target uploadfs")
        os.makedirs(os.path.dirname(marker_file), exist_ok=True)
        with open(marker_file, "w") as mf:
            mf.write(time.ctime())
    else:
        print("No changes in /data detected → skip LittleFS upload")

env.AddPreAction("upload", before_upload)
