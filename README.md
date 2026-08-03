

| Signal | Consumer | Purpose |
|---|---|---|
| `load_inputs` | Input/working registers | Capture values supplied by software |
| `load_result` | Required-part result registers | Capture the result of the required calculation |
| `bonus_ready` | Bonus module | Tell bonus logic that its input values are valid |
| `load_output` | Final output registers | Capture the completed bonus result |
| `hw_done` | Software | Tell software it may read the final outputs |
