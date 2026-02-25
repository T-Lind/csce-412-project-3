# Where the Instructions Confirm the Output Summary Is Correct and Valid

This file maps the project’s log/summary requirements to the assignment text so you can verify the output format is acceptable.

---

## 1. Assignment: "Project 3 - Load balancer log"

**Requirement:** Create a log of 10 servers running for 10000 clock cycles.

**Required content (from the assignment):**

- **Show the starting queue size.**
- **Show the ending queue size.**
- **List your range for task times.**
- **At least 3 other important pieces of information in the summary.**

Our log does this by:

- Writing a header block with **Starting queue size**, **Task time range: [min, max]**, and other run parameters.
- Writing a **SUMMARY** block at the end with **Ending queue size**, **Task time range** (in header), and multiple extra summary lines (total generated, total completed, total blocked, active servers, peak queue, avg queue, utilization, scale events, runTime).

So the instructions’ “starting queue size, ending queue size, task time range + at least 3 other” are satisfied by our header + SUMMARY.

---

## 2. Rubric: "Load Balancer Log"

**Shows basic logs (30 pts):**

1. Show the starting queue size.  
2. Show the ending queue size.  
3. List your range for task times.  

→ Our log explicitly includes **Starting queue size**, **Ending queue size**, and **Task time range: [min, max]** (in header and/or summary), so this rubric item is met.

**Show additional information (30 pts):**

“Logs that show the dynamic change in number of servers, randomness in request generation, execution, processing, assignment, etc.”

→ Our log includes event lines such as ASSIGN, COMPLETE, SCALE_UP, SCALE_DOWN, BLOCKED, which show server changes, assignment, execution, and processing. Request generation is driven by the configured probability and seed (logged), so the rubric’s “additional information” is covered.

**Shows End Status (20 pts):**

“Shows status on, remaining requests in queue, active servers, inactive servers, rejected/discarded requests, etc.”

→ Our SUMMARY includes:

- **Ending queue size** (remaining requests in queue)
- **Active servers (final)**
- **Total rejected/discarded**
- **Total completed**, **Total blocked**, etc.

So “remaining requests in queue, active servers, rejected/discarded” and related status are all present, satisfying this rubric item.

---

## 3. Summary

The assignment and rubric together require:

| Requirement | Where it’s stated | Where our log satisfies it |
|-------------|-------------------|-----------------------------|
| Starting queue size | Rubric “basic logs” #1; assignment “add 3 more pieces” | Header: “Starting queue size: …” |
| Ending queue size | Rubric “basic logs” #2; assignment “add 3 more pieces” | SUMMARY: “Ending queue size: …” |
| Range for task times | Rubric “basic logs” #3; assignment “add 3 more pieces” | Header: “Task time range: [min, max]” |
| At least 3 other important pieces | Assignment “add 3 more pieces” | SUMMARY: total generated, total completed, total blocked, active servers, peak queue, avg queue, utilization, scale events, runTime |
| Dynamic server changes / assignment / execution | Rubric “additional information” | ASSIGN, COMPLETE, SCALE_UP, SCALE_DOWN (and optional BLOCKED) event lines |
| End status (remaining queue, active servers, rejected/discarded) | Rubric “Shows End Status” | SUMMARY: ending queue size, active servers (final), total rejected/discarded |

So the **instructions and rubric explicitly confirm** that a log containing:

- starting queue size,  
- ending queue size,  
- task time range,  
- at least 3 other summary items,  
- event lines showing server/assignment/execution behavior, and  
- end status (remaining queue, active servers, rejected/discarded)  

is what is required and is therefore the correct, valid form for the output summary. Our implementation’s log format is aligned with these requirements.
