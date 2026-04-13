# SkipList Search Visual Trace (Search for "C")

This is a frame-by-frame simulation of how the pointers move in a SkipList.

Nodes in our City: 
[Sentinel] -> [A] (L0) -> [B] (L1) -> [C] (L0) -> [D] (L2)

---

### Phase 1: The Initial State
Each node is a tower of different heights. 

(L2) [S] ------------------------------> [D]
(L1) [S] --------------> [B] ----------> [D]
(L0) [S] --> [A] ------> [B] --> [C] --> [D]

---

### Phase 2: Start Search (Top Floor)
- Current: [S] (Sentinel)
- Floor (i): 2

[S] ---- (Next is D) ----> [D]
Wait! "D" is bigger than "C"! I can't move right!

Action: Close Floor 2 loop. Drop to Floor 1. ⏬

---

### Phase 3: The Movement (Express Lane)
- Current: [S] (Sentinel)
- Floor (i): 1

[S] ---- (Next is B) ----> [B]
"B" is smaller than "C"! I CAN move right!

Action: Move 'current' from [S] to [B]. 🏎️💨

---

### Phase 4: Standing at [B] (Floor 1)
- Current: [B]
- Floor (i): 1

[B] ---- (Next is D) ----> [D]
"D" is bigger than "C"! I can't move right anymore!

Action: Close Floor 1 loop. Drop to Floor 0. ⏬

---

### Phase 5: The Final Look (Floor 0)
- Current: [B]
- Floor (i): 0

[B] ---- (Next is C) ----> [C]
"C" is NOT smaller than "C"! Stop here.

Action: Close Floor 0 loop. The Search Ends! 🏁

---

## 🏆 Final Result:
- 'current' pointer is now at **[B]** (the node BEFORE C).
- Your search function will then check if `current->tower_of_pointers[0]` is C. 

The 'C' was found! Notice we **skipped** Node [A] entirely! 🚀
