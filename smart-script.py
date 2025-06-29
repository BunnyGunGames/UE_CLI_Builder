import os
import time
import subprocess

PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))
SOURCE_DIR = os.path.join(PROJECT_DIR, "Source")

HEADER_CACHE = os.path.join(PROJECT_DIR, ".last_header_build.txt")
CPP_CACHE = os.path.join(PROJECT_DIR, ".last_cpp_reload.txt")

BUILD_SCRIPT = os.path.join(PROJECT_DIR, "build.bat")
RELOAD_SCRIPT = os.path.join(PROJECT_DIR, "live-reload.bat")

def get_last_time(path):
    return os.path.getmtime(path) if os.path.exists(path) else 0

def any_changed_since(folder, ext, since):
    for root, _, files in os.walk(folder):
        for file in files:
            if file.endswith(ext):
                path = os.path.join(root, file)
                if os.path.getmtime(path) > since:
                    print(f"[smart-script] Changed: {file}")
                    return True
    return False

def is_unreal_running():
    result = subprocess.run(
        ["tasklist", "/FI", "IMAGENAME eq UnrealEditor.exe"],
        stdout=subprocess.PIPE,
        text=True
    )
    return "UnrealEditor.exe" in result.stdout

def wait_for_user_to_close_unreal():
    print("[smart-script] ⚠️  Unreal Editor is currently running.")
    print("[smart-script] Please close Unreal Editor to continue with the build...")
    
    while is_unreal_running():
        print("[smart-script] Still waiting for Unreal Editor to close...")
        time.sleep(2)  # Check every 2 seconds
    
    print("[smart-script] ✅ Unreal Editor has been closed. Proceeding with build...")

def run_script(label, path):
    print(f"[smart-script] Running {label}...")
    subprocess.run(["cmd.exe", "/c", path], check=True)

def main():
    header_changed = any_changed_since(SOURCE_DIR, ".h", get_last_time(HEADER_CACHE))
    cpp_changed = any_changed_since(SOURCE_DIR, ".cpp", get_last_time(CPP_CACHE))

    if header_changed:
        if is_unreal_running():
            wait_for_user_to_close_unreal()

        run_script("build.bat", BUILD_SCRIPT)
        timestamp = time.ctime()
        with open(HEADER_CACHE, "w") as f:
            f.write(timestamp)
        with open(CPP_CACHE, "w") as f:
            f.write(timestamp)

    elif cpp_changed:
        run_script("live-reload.bat", RELOAD_SCRIPT)
        with open(CPP_CACHE, "w") as f:
            f.write(time.ctime())

    else:
        print("[smart-script] No relevant changes detected. Nothing to do.")

if __name__ == "__main__":
    main()
