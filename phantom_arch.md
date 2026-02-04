# Phantom C2: Linux User-Land Architectural Specification

## 1. Executive Summary
The Phantom C2 is a high-sophistication, user-land implant designed for the Linux ecosystem. It prioritizes absolute stealth, environmental anchoring, and cryptographic integrity. Unlike traditional implants that rely on persistent disk presence or predictable communication patterns, Phantom operates as a transient memory-resident entity. It employs a multi-stage loading process, environmental keying to prevent sandbox analysis, and leverages HTTP/3 (QUIC) to blend seamlessly into modern enterprise network traffic.

## 2. Attack Vector
Phantom is designed to be delivered via a lightweight, benign-looking stager. This stager performs no malicious actions initially; its sole purpose is to validate the execution environment and securely pull the 'Core' payload.
- **Delivery:** Typically via social engineering or as a payload in a secondary stage of an exploit chain.
- **Validation:** The stager will not execute the core unless the hardware-user-environment hash matches the target profile exactly.

## 3. Payload Architecture
The architecture is strictly modular and memory-only.

### A. The Loader (Stager)
- **Size:** Optimized for minimal footprint (< 50KB).
- **Function:**
    1. Collects hardware IDs (`/etc/machine-id`, CPUID).
    2. Gathers user-land entropy (creation time of `~/.bashrc`, specific env vars).
    3. Derives the **Master Key (MK)** using Argon2 or a similar memory-hard KDF.
    4. Establishes a QUIC connection to the C2.
    5. Receives the encrypted Core, decrypts it using the register-reconstructed MK, and maps it into memory using `memfd_create`.

### B. The Core
- **Residency:** Exists only in a fileless state. It is executed via `fexecve` on the file descriptor returned by `memfd_create`, or via reflective SO loading if implemented as a library.
- **Modularity:** Functionalities (Exfiltration, Lateral Movement, Shell Access) are downloaded as encrypted BLOBs, loaded into memory, executed, and immediately wiped.

### C. Register-Only Key Reconstruction
- **Mechanism:** The Master Key is never stored as a contiguous block in the heap or stack.
- **Implementation:** Using inline Assembly (`asm`), the key components are moved into CPU registers (e.g., `%rax`, `%rbx`, `%rcx`, `%rdx` on x86_64). The decryption algorithm (AES-NI or ChaCha20) is invoked directly on these registers.
- **Volatilization:** Immediately following the decryption operation, `XOR` or `PXOR` instructions are used to zero out the registers, ensuring no cryptographic material remains in memory for forensic extraction.

## 4. Persistence Strategy
To maintain stealth, traditional persistence (crontabs, systemd units) is avoided unless explicitly requested.
- **Process Hollowing/Masquerading:** Phantom can inject into or masquerade as long-running user processes (e.g., `dbus-daemon`, `pulseaudio`, or `gnome-shell`).
- **Environmental Persistence:** Instead of disk persistence, it may rely on re-infection or living within user-session-scoped services that are less scrutinized than system-wide daemons.

## 5. Implementation Details
### Communication (HTTP/3 QUIC)
- **Protocol:** UDP/443.
- **Profile:** Malleable HTTP/3 headers mimicking Chrome/Chromium traffic.
- **Jitter:** Poisson distribution-based heartbeats to defeat statistical anomaly detection.
- **Analysis Defense:** If a 'TLS Fingerprint Mismatch' or unusual latency is detected (indicating an intercepting proxy or sandbox), the implant enters a "Deep Sleep" or triggers the Suicide Protocol.

### Environmental Keying Hash (Conceptual)
```cpp
// Pseudocode for Key Derivation
string hardware_id = read_file("/etc/machine-id");
string cpu_info = execute_cpuid();
string user_entropy = get_stat("~/.bashrc").st_ctime;
string master_seed = hash(hardware_id + cpu_info + user_entropy);
```

## 6. Obfuscation & Evasion
- **Dynamic Symbol Resolution:** No direct calls to `libc`. Symbols are resolved at runtime via custom `dlopen`/`dlsym` wrappers or by parsing the ELF link-map directly.
- **Polymorphism:** The Loader's code structure is mutated for every build using LLVM-based obfuscation (OLLVM), including instruction substitution and bogous control flow.
- **Anti-Forensics:**
    - No `/tmp` files.
    - Wiping of `argv[0]` to hide process name.
    - Monitoring for `ptrace` or `gdb` attachment.

## 7. Hygiene: The 'Cryptographic Suicide'
1. **Trigger:** Received 'Kill' command or detection of analysis.
2. **Key Shredding:** The register-based key reconstruction routine is "broken" by overwriting the key components in memory with random data.
3. **Memory Overwrite:** The memory region containing the Core and any modules is overwritten with a single pass of `0xFF`, then `0x00`, then random bytes.
4. **Self-Unlink:** If any stager existed on disk, it is shredded using a custom `unlink` wrapper that overwrites sectors before releasing.
