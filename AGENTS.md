# Agent Coordination Guidelines

## Shared Activity Log

All AI agents (Codex, Claude, etc.) working on this project must maintain a shared activity log to coordinate their work and avoid conflicts.

### Log File Location
- **File**: `ai_log.txt` (in project root)

### Logging Protocol

#### Before ANY action:
1. **Always READ the log first** using `Read` tool on `ai_log.txt`
   - Check what other agents have been doing
   - Understand the current state of the project
   - Avoid duplicate or conflicting work

#### After completing work:
2. **APPEND to the log** (never overwrite!)
   - Use `Read` tool first to get current content
   - Use `Write` tool to append new entry to existing content

### Log Entry Format

Each log entry must include:

```
================================================================================
AGENT: [codex/claude/other]
DATE: [YYYY-MM-DD]
TIME: [HH:MM:SS]
USER REQUEST: [Original user request in quotes]
--------------------------------------------------------------------------------
ACTIONS TAKEN:
- [Action 1]
- [Action 2]
- [Action 3]

FILES MODIFIED:
- [file path 1] - [description of changes]
- [file path 2] - [description of changes]

OUTCOME:
[Brief summary of what was accomplished]

NOTES FOR OTHER AGENTS:
[Any important information, warnings, or context for other agents]
================================================================================

```

### Example Log Entry

```
================================================================================
AGENT: claude
DATE: 2025-11-22
TIME: 14:35:20
USER REQUEST: "Add error handling to the boot process"
--------------------------------------------------------------------------------
ACTIONS TAKEN:
- Read kernel/boot.c
- Added try-catch blocks in initializeSystem()
- Added logging for boot errors
- Tested boot process with error injection

FILES MODIFIED:
- kernel/boot.c - Added error handling and recovery logic
- kernel/logger.c - Added new error level constants

OUTCOME:
Boot process now gracefully handles initialization errors and logs them
appropriately. System can recover from non-critical boot errors.

NOTES FOR OTHER AGENTS:
The new error codes are defined in kernel/logger.h. Any future boot-related
code should use these standard error codes for consistency.
================================================================================

```

### Important Rules

1. **NEVER overwrite** the log file - always append
2. **ALWAYS read first** before writing
3. **Include all required fields** in every entry
4. **Be specific** about what was changed and why
5. **Communicate** important information to other agents via NOTES section
6. **Use local time** for timestamps
7. **Quote user requests** exactly as received

### Coordination Benefits

- Avoid duplicate work
- Track project history
- Share context between agents
- Debug issues by reviewing past actions
- Maintain consistency across agent sessions

### First Time Setup

If `ai_log.txt` does not exist, the first agent to work should create it with:

```
AI AGENTS ACTIVITY LOG
Project: My-OS-Project2
Started: [current date]
================================================================================

[First log entry here]
```
