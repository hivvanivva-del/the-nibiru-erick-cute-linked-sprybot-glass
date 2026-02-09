#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <intrin.h>

/*
    Custom Layered Obfuscation
    Layer 1: Constexpr string/API hashing
    Layer 2: Template metaprogramming for constants
    Layer 3: Control flow flattening with lambdas
    Layer 4: Inline assembly anti-debugging / Timing checks
    Extra: Instruction Substitution
*/

namespace Obfuscation {

    // =============================================================
    // Layer 1: Constexpr string/API hashing (FNV-1a)
    // =============================================================
    namespace Hashing {
        constexpr uint64_t FNV_PRIME = 1099511628211ULL;
        constexpr uint64_t FNV_OFFSET = 14695981039346656037ULL;

        // Compile-time hash calculation
        constexpr uint64_t Calculate(const char* str, uint64_t hash = FNV_OFFSET) {
            return (*str == '\0') ? hash : Calculate(str + 1, (hash ^ static_cast<uint64_t>(*str)) * FNV_PRIME);
        }

        // Runtime wrapper (for comparison)
        inline uint64_t Runtime(const char* str) {
            uint64_t hash = FNV_OFFSET;
            while (*str) {
                hash ^= static_cast<uint64_t>(*str++);
                hash *= FNV_PRIME;
            }
            return hash;
        }
    }

    // =============================================================
    // Layer 2: Template metaprogramming for constants
    // =============================================================
    namespace Meta {
        // Obfuscates integral constants at compile-time using XOR
        template <typename T, T Value, uint64_t Key>
        struct ObfuscatedConstant {
            // The encrypted value is computed at compile time
            static constexpr T Encrypted = Value ^ static_cast<T>(Key);
            
            // Decryption happens at runtime
            __forceinline static T Get() {
                // Volatile ensures the compiler doesn't pre-calculate the result back to the original constant in the binary
                volatile T v = Encrypted; 
                return v ^ static_cast<T>(Key);
            }
        };

        // Macro to generate a somewhat unique key based on line number
        #define OBF_INT(val) Obfuscation::Meta::ObfuscatedConstant<decltype(val), val, (__LINE__ * 0x811C9DC5)>::Get()
    }

    // =============================================================
    // Instruction Substitution (Math Obfuscation)
    // =============================================================
    namespace Math {
        // x + y = x - ~y - 1
        template <typename T>
        __forceinline T Add(T x, T y) {
            return x - ~y - 1;
        }

        // x - y = x + ~y + 1
        template <typename T>
        __forceinline T Sub(T x, T y) {
            return x + ~y + 1;
        }
        
        // x ^ y = (x | y) - (x & y)
        template <typename T>
        __forceinline T Xor(T x, T y) {
            return (x | y) - (x & y);
        }
    }

    // =============================================================
    // Layer 3: Control flow flattening with lambdas
    // =============================================================
    namespace Flow {
        // Executes a sequence of lambdas in a flattened control flow
        // This confuses decompilers that try to reconstruct the linear flow
        class Flattener {
        public:
            template<typename... Funcs>
            __forceinline static void Execute(Funcs&&... funcs) {
                // Create a list of tasks
                std::function<void()> tasks[] = { std::forward<Funcs>(funcs)... };
                size_t count = sizeof...(funcs);
                
                // State variable for the state machine
                volatile int state = 0;
                
                // "Flattened" loop
                while (state < count) {
                    // In a real obfuscator, this switch would be randomized or encrypted.
                    // Here we use a basic switch-dispatch structure.
                    switch (state) {
                        case 0: tasks[0](); state++; break;
                        case 1: if (count > 1) tasks[1](); state++; break;
                        case 2: if (count > 2) tasks[2](); state++; break;
                        case 3: if (count > 3) tasks[3](); state++; break;
                        case 4: if (count > 4) tasks[4](); state++; break;
                        default: 
                            if (state < count) tasks[state](); 
                            state++; 
                            break;
                    }
                }
            }
        };
    }

    // =============================================================
    // Layer 4: Inline assembly anti-debugging / Timing
    // =============================================================
    namespace Security {
        __forceinline void AntiDebugCheck() {
#ifdef _M_IX86
            // x86 Inline Assembly: Check PEB.BeingDebugged
            bool found = false;
            __asm {
                xor eax, eax
                mov eax, fs:[0x30]  // Get PEB
                movzx eax, byte ptr [eax + 2] // BeingDebugged flag
                test eax, eax
                jz not_found
                mov found, 1
            not_found:
            }
            if (found) {
                // Trigger crash or infinite loop
                __asm { int 3 }
            }
#else
            // x64: Use timing checks (RDTSC) as inline asm is not supported by MSVC x64
            // Detect single stepping
            unsigned __int64 t1 = __rdtsc();
            
            // Junk code to measure
            volatile int k = 0;
            for(int i = 0; i < 100; i++) k = Math::Add(k, i);

            unsigned __int64 t2 = __rdtsc();
            
            // If the difference is too large, a debugger might be attached/stepping
            if ((t2 - t1) > 0x10000) {
                 // Fast fail
                 // __fastfail(1); 
                 // Or simple crash
                 *(volatile int*)0 = 0;
            }
#endif
        }

        // Garbage code insertion macro to mess up static analysis
        #define JUNK_CODE \
            __if_exists(Obfuscation::Security::AntiDebugCheck) { \
                volatile int _junk = 0; \
                _junk = Obfuscation::Math::Xor(_junk, 0xDEAD); \
                _junk = Obfuscation::Math::Add(_junk, 1); \
            }
    }

    // =============================================================
    // Layer 5: Compile-time string obfuscation (XOR)
    // =============================================================
    namespace Strings {
        template <size_t N, uint8_t Key>
        class ObfuscatedString {
        private:
            uint8_t _data[N];

            constexpr uint8_t Encrypt(char c) const {
                return static_cast<uint8_t>(c) ^ Key;
            }

        public:
            template <size_t... Is>
            constexpr ObfuscatedString(const char* str, std::index_sequence<Is...>)
                : _data{ Encrypt(str[Is])... } {}

            __forceinline const char* Decrypt() {
                for (size_t i = 0; i < N; i++) {
                    _data[i] ^= Key;
                }
                return reinterpret_cast<const char*>(_data);
            }

            __forceinline char* GetData() {
                return reinterpret_cast<char*>(_data);
            }
        };

        #define OBF_STR(str) []() -> const char* { \
            static constexpr size_t len = sizeof(str); \
            static Obfuscation::Strings::ObfuscatedString<len, static_cast<uint8_t>(__LINE__ * 0x85)> obf(str, std::make_index_sequence<len>{}); \
            static bool decrypted = false; \
            if (!decrypted) { \
                obf.Decrypt(); \
                decrypted = true; \
            } \
            return obf.GetData(); \
        }()
    }
}
