import os
import shutil
import time
import glob

v2_dir = r"C:\Users\Administrator\Desktop\Dll Arabic Chat\z\L2ArabicProxy\V2"
game_dirs = glob.glob(r"E:\Games\*Samurai Crow*")
if not game_dirs:
    print("Error: Samurai Crow folder not found in E:\\Games")
    exit(1)

target_system = os.path.join(game_dirs[0], "Samurai Crow", "system")
if not os.path.exists(target_system):
    print(f"Error: system folder does not exist: {target_system}")
    exit(1)

files = ["d3d9.dll", "l2arabic.ini", "Cairo.ttf", "Cairo-Bold.ttf"]
stamp = int(time.time())

for fname in files:
    src = os.path.join(v2_dir, fname)
    dst = os.path.join(target_system, fname)
    if not os.path.exists(src):
        continue
    
    # If dst exists and might be locked, rename it first
    if os.path.exists(dst):
        try:
            shutil.copy2(src, dst)
            print(f"Deployed {fname} successfully")
        except PermissionError:
            bak = dst + f".old-{stamp}"
            try:
                os.rename(dst, bak)
                shutil.copy2(src, dst)
                print(f"Deployed {fname} via hot-swap rename (old -> {os.path.basename(bak)})")
            except Exception as e:
                print(f"Failed to deploy {fname}: {e}")
    else:
        shutil.copy2(src, dst)
        print(f"Deployed {fname} (new)")

print(f"All files deployed successfully to: {target_system}")
